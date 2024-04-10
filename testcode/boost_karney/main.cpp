#include <QCoreApplication>
#include <QDebug>
#include <QtMath>
#include <QElapsedTimer>
#include <iostream>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/algorithms/detail/azimuth.hpp>
#include <boost/geometry/geometries/register/point.hpp>

#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/formulas/karney_direct.hpp>
#include <boost/geometry/formulas/andoyer_inverse.hpp>
#include <boost/geometry/formulas/vincenty_direct.hpp>
#include <boost/geometry/formulas/vincenty_direct.hpp>

#include <boost/geometry/strategies/geographic/azimuth.hpp>
#include <boost/geometry/algorithms/detail/azimuth.hpp>

#include <boost/geometry/strategies/distance.hpp>
#include <boost/geometry/algorithms/distance.hpp>

#include <X11/Intrinsic.h>
#include "eckernel.h"

#include <QElapsedTimer>

#include <boost/geometry/srs/epsg.hpp>

#include "dmltmsp/latlong.h"

#include <iostream>

namespace bg = boost::geometry;
namespace bm = boost::geometry::model::d2;

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(LatLong, double, cs::geographic<degree>, longitude, latitude, setLongitude, setLatitude)

int main(int argc, char *argv[])
{

    EcKernelRegisterSetMode(EC_REGISTER_REALLY_QUIET);
    EcKernelRegisterSetMode(EC_REGISTER_CHECK_NETCARD);

    QCoreApplication a(argc, argv);

    auto chartsDirectory(qgetenv("RMSL_HPDP_CHARTS"));
    EcDictInfo *dictionary = EcDictionaryReadModule(EC_MODULE_MAIN, nullptr);
    auto *view = EcChartViewCreate(dictionary, EC_RESOLUTION_HIGH);
    EcChartSetErrorLog(view, stderr);
    //N 57.81550 W 8.59131

    LatLong radar(57.81550, -8.59131);
    EcDrawSetProjection(view, EC_GEO_PROJECTION_TRANSVERS_MERCATOR_EXT, radar.latitude(), radar.longitude(), 1.0, 0.0);
    EcDrawSetDatum(view, EC_GEO_DATUM_WGS84, 0.0, 0.0);

    bg::srs::spheroid<double> wgs84;

    QElapsedTimer t;

    double distance_m = 100000;

    // Create an alias of the formula.
    typedef bg::formula::karney_direct<double, true, true, true, true, 8> karney_direct;
    typedef boost::geometry::formula::vincenty_direct<double, true, false, false, false> vincenty_direct;

    // Structure to hold the resulting values.
    bg::formula::result_direct<double> result;

    auto iters = 1e6;

#if 0
    for (double azimuth : {0, 45, 90, 135, 180, 225, 270, 315})
    {
        //auto result = karney_direct::apply(radar.longitude(), radar.latitude(), distance_m, azimuth, wgs84);
        auto result = vincenty_direct::apply(radar.longitude(true), radar.latitude(true), distance_m, qDegreesToRadians(azimuth), wgs84);

        double north = cos(qDegreesToRadians(azimuth)) *distance_m, east = sin(qDegreesToRadians(azimuth))*distance_m;

        double latitude, longitude;
        EcDrawCartesianToLatLon(view, north, east, &latitude, &longitude);

        auto start = LatLong(latitude, longitude);
        auto end = LatLong(result.lat2, result.lon2, true);

        auto distance = bg::distance(start, end, bg::strategy::distance::andoyer<>(wgs84));
        qDebug() <<"Azi" << azimuth<<  "Start" << start << "end" << end << distance << "metres";
    }
#endif

    t.start();

    for (double azimuth : {0, 45, 90, 135, 180, 225, 270, 315})
    {
        t.restart();

        for(int i = 0; i < iters; ++i)
        {
            result = vincenty_direct::apply(radar.longitude(true), radar.latitude(true), distance_m, qDegreesToRadians(azimuth), wgs84);
        }
        printf("vincenty_direct azi %f %f %f elapsed %f nano seconds\n", azimuth, qRadiansToDegrees(result.lat2), qRadiansToDegrees(result.lon2), t.nsecsElapsed()/iters);

    }
    for (double azimuth : {0, 45, 90, 135, 180, 225, 270, 315})
    {
        t.restart();

        for(int i = 0; i < iters; ++i)
        {
            result = karney_direct::apply(radar.longitude(), radar.latitude(), distance_m, azimuth, wgs84);
        }
        printf("karney_direct azi %f %f %f elapsed %f nano seconds\n", azimuth, result.lat2, result.lon2, t.nsecsElapsed()/iters);
    }

    for (double azimuth : {0, 45, 90, 135, 180, 225, 270, 315})
    {
        t.restart();
        double latitude, longitude;

        for(int i = 0; i < iters; ++i)
        {
            double north = cos(qDegreesToRadians(azimuth)) *distance_m, east = sin(qDegreesToRadians(azimuth))*distance_m;
            EcDrawCartesianToLatLon(view, north, east, &latitude, &longitude);
        }

        printf("Seevncs azi %f %f %f elapsed %f nano seconds\n", azimuth, latitude, longitude, t.nsecsElapsed()/iters);
    }

    return 0;
}
