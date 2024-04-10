#include <QCoreApplication>

#include <QList>
#include <QSize>
#include <QtPositioning/QGeoCoordinate>
#include <QPointF>

#include <boost/geometry.hpp>
#include <boost/geometry/srs/epsg.hpp>
#include <boost/geometry/srs/projection.hpp>
#include <boost/geometry/srs/projections/dpar.hpp>

#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/formulas/karney_direct.hpp>

#include "dmltmsp/latlong.h"

#include <QDebug>

/*
test commit with GIT
*/




namespace bg = boost::geometry;

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QGeoCoordinate, double, cs::geographic<degree>, longitude, latitude, setLongitude, setLatitude)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(LatLong, double, cs::geographic<degree>, longitude, latitude, setLongitude, setLatitude)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QPointF, double, bg::cs::cartesian, x, y, setX, setY)

typedef boost::geometry::formula::karney_direct<double, true, false, false, false, 8> karney_direct;

enum GEOPROJ
{
    World_mercator = 3395,
    Web_Mercator = 3785,
    Plate_Carree = 32662,
    OSGB36 = 27700,
};

#ifdef SPAR
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);


    LatLong radar(53, -3);
    LatLong viewportCentre(52.0, -2.8);

    auto k = (1/cos(viewportCentre.latitude(true)));

    QSize bscan(8/*1024*/, 8/*1024*/);
    auto metresPerSample = 10000; //28;
    auto radiansPerAzimuth = 2 * M_PI / bscan.width();

    bg::srs::projection<> proj = bg::srs::epsg(World_mercator);

    using namespace  bg::srs::spar;
    using MercParams = parameters<proj_merc, ellps_wgs84, k_0<>>;
    bg::srs::projection<MercParams> proj1 = MercParams(proj_merc(), ellps_wgs84(), k_0(k));

    using WebMercParams = parameters<proj_merc, ellps_sphere, k_0<>>;
    bg::srs::projection<WebMercParams> proj2 = WebMercParams(proj_merc(), ellps_sphere(), k_0(k));

    QPointF p;
    proj.forward(radar, p);

    QPointF pp;
    proj1.forward(radar, pp);

    QPointF ppp;
    proj2.forward(radar, ppp);

    qDebug() << Qt::fixed << p*k << pp << ppp;

    return 0;
}

#else
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);


    LatLong radar(53, -3);
    LatLong viewportCentre(52.0, -2.8);
    auto k = (1/cos(viewportCentre.latitude(true)));

    static const boost::geometry::srs::spheroid<double> wgs84; // a=6378137.0, b=6356752.3142451793

    bg::srs::projection<> proj = bg::srs::epsg(World_mercator);

    boost::geometry::srs::projection<>* x = nullptr;





    using namespace  bg::srs::dpar;
    bg::srs::projection<> proj1 = parameters<>(proj_tmerc)(ellps_airy)(units_m)(lat_0, viewportCentre.latitude())(lon_0, viewportCentre.longitude()); //bg::srs::epsg(OSGB36);
    x = &proj1;


    auto targetLatLong = karney_direct::apply(radar.longitude(), radar.latitude(), 50000, 45, wgs84);
    auto tmerc_offsetMetres = LatLong(targetLatLong.lat2, targetLatLong.lon2) - viewportCentre;

    QPointF centre, target;
    x->forward(viewportCentre, centre);
    x->forward( LatLong(targetLatLong.lat2, targetLatLong.lon2), target);

    qDebug() << centre << target << (target-centre) << tmerc_offsetMetres << targetLatLong.lat2 << targetLatLong.lon2;

    QSize bscan(8/*1024*/, 8/*1024*/);
    auto metresPerSample = 10000; //28;
    auto radiansPerAzimuth = 2 * M_PI / bscan.width();




    boost::geometry::srs::projection<> proj2 = parameters<>(proj_merc)(ellps_wgs84)(units_m)(k_0, k);
    x = &proj2;



    QPointF p;
    x->forward(radar, p);

    QPointF pp;
    x->forward(radar, pp);

    QPointF ppp;
    x->forward(radar, ppp);



    qDebug() << Qt::fixed << p*k << pp << ppp;

    x = &proj;

    x->forward(viewportCentre, centre);
    x->forward( LatLong(targetLatLong.lat2, targetLatLong.lon2), target);

    qDebug() << centre << target << (target-centre) << tmerc_offsetMetres << targetLatLong.lat2 << targetLatLong.lon2;



    // {
    //     using namespace boost::geometry::srs::dpar;
    //     typedef point<double, 2, cs::geographic<degree> > point_ll;
    //     typedef point<double, 2, cs::cartesian> point_xy;

    //     point_ll pt_ll(-3, 54);
    //     point_ll pt_ll2(0, 0);
    //     point_xy pt_xy(0, 0);

    //   //  bg::srs::projection<> prj = parameters<>(proj_tmerc)(ellps_wgs84)(units_m);
    //   //  bg::srs::projection<> prj1 = parameters<>(proj_merc)(ellps_wgs84)(units_m);


    //     //projection< proj_tmerc, ellps_wgs84, units_m> prje1;





    //     QList<projection<>> prjList;
    //     prjList.append(prj);
    //     prjList.append(prj1);

    //     prj.forward(pt_ll, pt_xy);
    //     //  test::check_geometry(pt_xy, "POINT(111308.33561309829 110591.34223734379)", 0.001);


    //     qDebug() << Q_FUNC_INFO << pt_xy.get<0>() << pt_xy.get<1>();
    //     prj1.forward(pt_ll, pt_xy);

    //     qDebug() << Q_FUNC_INFO << pt_xy.get<0>() << pt_xy.get<1>();

    //     prj.inverse(pt_xy, pt_ll2);
    //     //test::check_geometry(pt_ll2, "POINT(1 1)", 0.001);
    // }


    proj.forward(viewportCentre, p);
    return 0;


    /*using namespace boost::geometry;
    using namespace boost::geometry::model;
    using namespace boost::geometry::srs;
    typedef point<double, 2, cs::geographic<degree> > point_ll;
    typedef point<double, 2, cs::cartesian> point_xy;
    point_ll pt_ll(-3, 53);
    point_xy pt_xy(0, 0);
    prj.forward(pt_ll, pt_xy);
    qDebug() << Q_FUNC_INFO << pt_xy.get<0>() << pt_xy.get<1>();*/

    auto calculateAzimuth = [proj, p, radar, wgs84, viewportCentre, radiansPerAzimuth, bscan, metresPerSample](int azimuth)
    {
        //bg::srs::sphere<double> wgs84Sphere;
        //bg::strategy::distance::haversine<> haversine(wgs84Sphere);


        auto samples = bscan.height();
        double north = cos((azimuth * radiansPerAzimuth)), east = sin((azimuth * radiansPerAzimuth));

        for(int sample=0; sample < samples; sample++)
        {
            double northing = north * sample * metresPerSample;
            double easting = east * sample * metresPerSample;

            auto result = karney_direct::apply(radar.longitude(), radar.latitude(), sample * metresPerSample, ((azimuth * radiansPerAzimuth)) * 180 / M_PI, wgs84);
            auto geoLatLong = LatLong(result.lat2, result.lon2);

            auto tmerc_offsetMetres = geoLatLong - viewportCentre;

            QPointF merc_offsetMetres;

            proj.forward(geoLatLong, merc_offsetMetres);

            qDebug() <<  "azi" << azimuth << sample*metresPerSample << northing << easting  << geoLatLong.latitude() << geoLatLong.longitude() << tmerc_offsetMetres<< (merc_offsetMetres-p) / (1/cos(viewportCentre.latitude(true)));

        }
    };

    for(int i = 0; i < bscan.width(); ++i)
        calculateAzimuth(i);


    QPointF  prev, current;

    for (int i = 5; i < 85; i+=5)
    {
        LatLong latlong(i, -3);
        proj.forward(latlong, current);
        prev = current;
    }


    return app.exec();

    return 0;
}
#endif
