#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QSize>
#include <QtConcurrent>

#include "dmltmsp/latlong.h"
#include <boost/iterator/counting_iterator.hpp>

#include "LatLong2.h"

#include <X11/Intrinsic.h>
#include "eckernel.h"



int main(int argc, char *argv[])
{
    EcKernelRegisterSetMode(EC_REGISTER_REALLY_QUIET);
    EcKernelRegisterSetMode(EC_REGISTER_CHECK_NETCARD);

    QCoreApplication app(argc, argv);


    auto chartsDirectory(qgetenv("RMSL_HPDP_CHARTS"));
    EcDictInfo *dictionary = EcDictionaryReadModule(EC_MODULE_MAIN, nullptr);
    auto *view = EcChartViewCreate(dictionary, EC_RESOLUTION_HIGH);

    EcChartSetErrorLog(view, stderr);


    QSize bscan(256/*1024*/, 256/*1024*/);

    LatLong radar(53, -3);

    LatLong2_set_projection(LATLONG2_WGS84);
    LatLong2_set_origin_from_dbl_rad(radar.latitude(true), radar.longitude(true));

    auto metresPerSample = 400; //28.937;
    auto radiansPerAzimuth = 2 * M_PI / bscan.width();


    EcDrawSetProjection(view, EC_GEO_PROJECTION_TRANSVERS_MERCATOR_EXT, radar.latitude(), radar.longitude(), 1.0, 0.0);
    EcDrawSetDatum(view, EC_GEO_DATUM_WGS84, 0.0, 0.0);



    auto calculateAzimuth = [view, radar, radiansPerAzimuth, bscan, metresPerSample](int azimuth)
    {
        auto samples = bscan.height();
        double north = cos((azimuth * radiansPerAzimuth)), east = sin((azimuth * radiansPerAzimuth));
        QList<QPointF> values;

        for(int sample=0; sample < samples; sample++)
        {
            double latitude, longitude;
            double _latitude, _longitude;

            double northing = north * sample * metresPerSample;
            double easting = east * sample * metresPerSample;


            auto latlong = radar + QPointF{easting, northing};

            //flatworld offsets in metres
            EcDrawCartesianToLatLon(view, northing, easting, &latitude, &longitude);

            LatLong2_fw_to_llrad(easting, northing, &_latitude, &_longitude);

            qDebug() <<  "azi" << azimuth << sample << north * sample * metresPerSample << east * sample * metresPerSample << latitude << longitude << _latitude*180.0/M_PI << _longitude*180/M_PI << latlong.latitude() << latlong.longitude();

            //projected  in metres
            //values.append(LatLong(latitude, longitude) - m_viewportCentre);
        }

        //return values;
    };

    //QtConcurrent::mapped(boost::counting_iterator<int>(0), boost::counting_iterator<int>(bscan.width()), calculateAzimuth);


    for(int i = 0; i < bscan.width(); ++i)
        calculateAzimuth(i);



    return app.exec();
}
