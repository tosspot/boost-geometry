#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <iostream>
#include <boost/geometry.hpp>
#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <boost/geometry/srs/epsg.hpp>

namespace bg = boost::geometry;
namespace bm = bg::model::d2;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    bg::srs::projection<bg::srs::static_epsg<3785> > epsg3857;

    bm::point_xy<double, bg::cs::geographic<bg::degree> > longlat;
    bm::point_xy<double> fpr_EN{ -338501.42, 7065303.37};

    QElapsedTimer t;
    t.start();

    auto iters = 10000000;


    for (int i = 0; i < iters; ++i)
    {
        //qDebug() << "Cunt" << i;
        epsg3857.inverse(fpr_EN, longlat);
    }

    printf("Elapsed %lld\n", t.nsecsElapsed() / iters);

    qDebug() <<  longlat.y() << longlat.x();

    return a.exec();
}
