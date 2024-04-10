#include "projection.h"

#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/srs/transformation.hpp>
#include <boost/geometry/strategies/distance.hpp>
#include <boost/geometry/algorithms/distance.hpp>

#include <cmath>

namespace bg = boost::geometry;

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QGeoCoordinate, double, cs::geographic<degree>, longitude, latitude, setLongitude, setLatitude)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(LatLong, double, cs::geographic<degree>, longitude, latitude, setLongitude, setLatitude)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QPointF, double, bg::cs::cartesian, x, y, setX, setY)

//#define USECHARTS_TRANSFORM

#ifdef USECHARTS_TRANSFORM


namespace  {
    double earthRadius = 6378137.0;

    int PseudoMercatorForward(double latitude, double longitude, double *easting, double *northing, void */*userData*/)
    {
        *northing = std::log(std::tan((latitude) / 2 + M_PI/4 )) * earthRadius;
        *easting =  longitude * earthRadius;
        return true;
    }

    int PseudoMercatorInverse(double easting, double northing, double *latitude, double *longitude, void */*userData*/)
    {
        *latitude = 2 * std::atan(std::exp(northing / earthRadius)) - M_PI/2;
        *longitude = easting / earthRadius;
        return true;
    }

    double distanceTo(const QGeoCoordinate &p1, const QGeoCoordinate &p2)
    {
        auto degreesToRads = M_PI / 180.0;
        auto dy = p2.latitude() * degreesToRads - p1.latitude()*degreesToRads;
        auto dx = p2.longitude()*degreesToRads - p1.longitude()*degreesToRads;
        auto x = earthRadius * dx * std::cos(p2.latitude()*degreesToRads);
        auto y = earthRadius * dy;
        return sqrt(x*x + y*y); //metres
    }
}
#endif


Projection& Projection::instance()
{
    static Projection instance;
    return instance;


}


Projection::Projection() :
    Projection(QGeoRectangle())
{
}


Projection::Projection(const QGeoRectangle &geoRect) :
    m_d(new Private)
{
    setGeographicBoundingBox(geoRect);
}

Projection::~Projection()
{
    delete m_d;
}


void Projection::setGeographicBoundingBox(const QGeoRectangle &geoRectangle)
{
    if (geoRectangle.isValid())
    {
        m_d->m_geoRect = geoRectangle;
        m_d->m_projectedRect = geoToProj(geoRectangle);
        auto geo  = projToGeo(m_d->m_projectedRect);
        //Q_ASSERT(geoRectangle == geo);
        qDebug("%s %s", Q_FUNC_INFO, qPrintable(geoRectangle.toString()));
    }
}


QRectF Projection::projectedBoundingBox(const QGeoRectangle &geoRectangle) const
{
    auto rectangle = geoRectangle.isValid() ? geoToProj(geoRectangle) : m_d->m_projectedRect;
    //Q_ASSERT(rectangle.isValid());
    return rectangle;
}


QPointF Projection::geoToProj(const LatLong &geoPosition) const
{
    qDebug()<<Q_FUNC_INFO<<geoPosition<<m_d->m_geoRect.toString()<<m_d->m_geoRect;
    return geoToProj(QGeoCoordinate(geoPosition.latitude(), geoPosition.longitude()));
}


QPointF Projection::geoToProj(const QGeoCoordinate &geoPosition) const
{
    qDebug()<<Q_FUNC_INFO<<geoPosition << m_d->m_geoRect.toString() << m_d->m_geoRect;
    //Q_ASSERT(m_d->m_geoRect.contains(geoPosition));

    QPointF point;
    m_d->m_transform.forward(geoPosition, point);
    point.setY(-point.y());

    /*if (!m_d->m_projectedRect.isNull())
        Q_ASSERT(m_d->m_projectedRect.contains(point));*/

#ifdef USECHARTS_TRANSFORM
    double x, y;
    PseudoMercatorForward(geoPosition.latitude() * M_PI/180.0, geoPosition.longitude()* M_PI/180.0, &x, &y, nullptr);

    auto p = point - QPointF(x, -y);
    double trueLength = std::sqrt(std::pow(p.x(), 2) + std::pow(p.y(), 2));
    Q_ASSERT(trueLength < 1);

    return {x,-y};
#endif

    return point;
}


QGeoCoordinate Projection::projToGeo(const QPointF &projPosition) const
{
    /*if (!m_d->m_projectedRect.isNull())
        Q_ASSERT(m_d->m_projectedRect.contains(projPosition));*/

    QGeoCoordinate coord;
    m_d->m_transform.inverse(QPointF(projPosition.x(), -projPosition.y()), coord);
    qDebug("%s %s", Q_FUNC_INFO, qPrintable(coord.toString(QGeoCoordinate::Degrees)));

#ifdef USECHARTS_TRANSFORM
    double lat,lon;
    PseudoMercatorInverse(projPosition.x(), -projPosition.y(), &lat, &lon, nullptr);
    QGeoCoordinate geo{lat*180/M_PI,lon*180/M_PI};

    //Q_ASSERT(geo==coord);

    return geo;
#endif
    //Q_ASSERT(m_d->m_geoRect.contains(coord));
    return coord;
}


LatLong Projection::projToLatlong(const QPointF &projPosition) const
{
    LatLong latlong;
    m_d->m_transform.inverse(QPointF(projPosition.x(), -projPosition.y()), latlong);

#ifdef USECHARTS_TRANSFORM
    double lat,lon;
    PseudoMercatorInverse(projPosition.x(), -projPosition.y(), &lat, &lon, nullptr);
    LatLong geo{lat*180/M_PI,lon*180/M_PI};
    auto p = latlong-geo;
    double trueLength = std::sqrt(std::pow(p.x(), 2) + std::pow(p.y(), 2));
    Q_ASSERT(trueLength < 1);
    return geo;
#endif

    qDebug("%s %f %f", Q_FUNC_INFO, latlong.latitude(), latlong.longitude());
    return latlong;
}


QRectF Projection::geoToProj(const QGeoRectangle &geoRectangle) const
{
    auto topLeft = geoToProj(geoRectangle.topLeft());
    auto bottomRight = geoToProj(geoRectangle.bottomRight());
    auto rect = QRectF(topLeft, bottomRight);
    //Q_ASSERT(rect.isValid());
    return rect;
}


QGeoRectangle Projection::projToGeo(const QRectF &projRectangle) const
{
    Q_ASSERT(projRectangle.isValid());
    auto topLeft = projToGeo(projRectangle.topLeft());
    auto bottomRight = projToGeo(projRectangle.bottomRight());

    return QGeoRectangle(topLeft, bottomRight);
}


double Projection::distance(const QPointF &projFrom, const QPointF &projTo) const
{
#ifdef USECHARTS_TRANSFORM
    return distanceTo(projToGeo(projFrom), projToGeo(projTo));
#else
    return distance(projToGeo(projFrom), projToGeo(projTo));
#endif
}


double Projection::distance(const QGeoCoordinate &geoFrom, const QGeoCoordinate &geoTo) const
{
    qDebug("%s from %s to %s", Q_FUNC_INFO, qPrintable(geoFrom.toString(QGeoCoordinate::Degrees)), qPrintable(geoTo.toString(QGeoCoordinate::Degrees)));
    bg::srs::spheroid<double> wgs84;
#ifdef USECHARTS_TRANSFORM
    return distanceTo(geoFrom, geoTo);
#else
    return bg::distance(geoFrom, geoTo, bg::strategy::distance::haversine<>(wgs84));
#endif
}


QPointF Projection::projectedAtDistanceAndAzimuth(const QGeoCoordinate &geoPos, double distance, double azimuth)
{
#pragma ("Check this - which algorithm (point at distance and azimuth) do we want to use for this ?")
    auto geoPoint = geoPos.atDistanceAndAzimuth(distance, azimuth);
    return geoToProj(geoPoint);
}


QPointF Projection::projectedAtDistanceAndAzimuth(const LatLong &geoPos, double distance, double azimuth)
{
    return projectedAtDistanceAndAzimuth(QGeoCoordinate(geoPos.latitude(), geoPos.longitude()), distance, azimuth);
}

double Projection::distance(const LatLong &geoFrom, const LatLong &geoTo) const
{
    return distance(QGeoCoordinate(geoFrom.latitude(), geoFrom.longitude()), QGeoCoordinate(geoTo.latitude(), geoTo.longitude()));
}
