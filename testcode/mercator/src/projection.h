#ifndef PROJECTION_H
#define PROJECTION_H

#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCoordinate>
#include <QPointF>
#include <QRectF>

#include "dmltmsp/latlong.h"

#include <boost/geometry/srs/epsg.hpp>

class Projection
{
public:
    static Projection &instance();
    ~Projection();

    void setGeographicBoundingBox(const QGeoRectangle &geoRectangle);
    QRectF projectedBoundingBox(const QGeoRectangle &geoRectangle = QGeoRectangle()) const;

    QPointF geoToProj(const LatLong &geoPosition) const;
    QPointF geoToProj(const QGeoCoordinate &geoPosition) const;
    QRectF geoToProj(const QGeoRectangle &geoRectangle) const;

    QGeoCoordinate projToGeo(const QPointF &projPosition) const;
    QGeoRectangle projToGeo(const QRectF &projRectangle) const;
    LatLong projToLatlong(const QPointF &projPosition) const;

    double distance(const QPointF &projFrom, const QPointF &projTo) const;
    double distance(const QGeoCoordinate &geoFrom, const QGeoCoordinate &geoTo) const;
    double distance(const LatLong &geoFrom, const LatLong &geoTo) const;

    QPointF projectedAtDistanceAndAzimuth(const QGeoCoordinate &geoPos, double distance, double azimuth);
    QPointF projectedAtDistanceAndAzimuth(const LatLong &geoPos, double distance, double azimuth);


private:
    Projection();
    Projection(const QGeoRectangle &geoRect);

protected:
    struct Private;
    Private *m_d;
};

struct Projection::Private
{
    QGeoRectangle m_geoRect;
    //EPSG3785 (3857) Pseudo mercator (Web mercator)
    //EPSG3395 World mercator
    //EPSG32662 Plate Carree projection

    enum GEOPROJ
    {
        World_mercator = 3395,
        Web_Mercator = 3785,
        Plate_Carree = 32662,
        OSGB36 = 27700,
    };

    //boost::geometry::srs::projection<boost::geometry::srs::static_epsg<Plate_Carree> > m_transform;
    boost::geometry::srs::projection<boost::geometry::srs::static_epsg<World_mercator> > m_transform;





    QRectF m_projectedRect;
};

#endif // PROJECTION_H
