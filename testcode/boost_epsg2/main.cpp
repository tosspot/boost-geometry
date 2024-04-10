#include <QCoreApplication>

#include <iostream>
#include <boost/geometry.hpp>
#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <boost/geometry/srs/epsg.hpp>
#ifdef COLIRU
#include <boost/geometry/srs/projection.hpp>
#endif



namespace bg = boost::geometry;
namespace bm = bg::model::d2;
namespace srs = bg::srs;

using LongLat = bm::point_xy<double, bg::cs::geographic<bg::degree> >;
using UTM     = bm::point_xy<double/*, srs::static_epsg<3785>*/>;

constexpr LongLat Amsterdam() { return { 4.897, 52.371}; }
constexpr LongLat Barcelona() { return { 2.1734, 41.3851 }; }
constexpr LongLat fpr() { return { 3.0408, 53.4427 }; }
constexpr UTM utmfpr() { return { -338501.42, 7065303.37 }; }


template <typename T> void report(LongLat src, T const &prj) {
    UTM r{};
    prj.forward(src, r);


    std::cout << std::fixed << bg::wkt(src) << " -> " << bg::wkt(r) << "\n";
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
#ifndef COLIRU
    // dynamic projection factory too heavy on Coliru
//    srs::projection<> zone31 = srs::proj4("+proj=utm +ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=31");
//    report(Amsterdam(), zone31);
//    report(Barcelona(), zone31);
#endif

    srs::projection<srs::static_epsg<3785> > epsg3785;

    LongLat r;
    epsg3785.inverse(utmfpr(), r);
    std::cout << std::fixed << bg::wkt(utmfpr()) << " -> " << bg::wkt(r) << "\n";


//    report(Amsterdam(), epsg3785);
//    report(Barcelona(), epsg3785);
 //   report(fpr(), epsg3785);

    return a.exec();
}
