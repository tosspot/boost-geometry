#if 1
#include <string>
#include <QDebug>
#include <QtGui>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/geometries.hpp>

#include <boost/geometry/io/io.hpp>
#include <fstream>
#include <iostream>

int main()
{
    //ftp://188.44.46.157/boost_1_55_0b1/libs/geometry/example/


    typedef double coordinate_type;
    typedef boost::geometry::model::d2::point_xy<coordinate_type> point;
    typedef boost::geometry::model::polygon<point> polygon;
    namespace bg = boost::geometry;

    // Declare strategies
    const double buffer_distance = 1.0;
    const int points_per_circle = 36;
    boost::geometry::strategy::buffer::distance_symmetric<coordinate_type> distance_strategy(buffer_distance);
    boost::geometry::strategy::buffer::join_round join_strategy(points_per_circle);
    boost::geometry::strategy::buffer::end_round end_strategy(points_per_circle);
    boost::geometry::strategy::buffer::point_circle circle_strategy(points_per_circle);
    boost::geometry::strategy::buffer::side_straight side_strategy;

    // Declare output
    boost::geometry::model::multi_polygon<polygon> result;

    // Declare/fill a linestring
    boost::geometry::model::linestring<point> ls;

    boost::geometry::read_wkt("LINESTRING(0 0,4 5,7 4,10 6)", ls);

    // Create the buffer of a linestring
    boost::geometry::buffer(ls, result,
                distance_strategy, side_strategy,
                join_strategy, end_strategy, circle_strategy);

    std::string message;
//    bool isValid = boost::geometry::is_valid(result, message);


    qCritical("%s %s", Q_FUNC_INFO, qPrintable(QString::fromStdString(message)));

    //dump the points
    polygon outputPolygon = result[0]; //there is only one polygon
    qDebug() << "Polys" << result.size();
    for(unsigned int pointID = 0; pointID < outputPolygon.outer().size(); ++pointID)
    {
          std::cout << boost::geometry::get<0>(outputPolygon.outer()[pointID]) << " " << boost::geometry::get<1>(outputPolygon.outer()[pointID]) << std::endl;
    }


/*
    // Declare/fill a multi point
    boost::geometry::model::multi_point<point> mp;
    boost::geometry::read_wkt("MULTIPOINT((3 3),(4 4),(6 2))", mp);

    // Create the buffer of a multi point
    boost::geometry::buffer(mp, result,
                distance_strategy, side_strategy,
                join_strategy, end_strategy, circle_strategy);


    // Declare/fill a multi_polygon
    boost::geometry::model::multi_polygon<polygon> mpol;
    boost::geometry::read_wkt("MULTIPOLYGON(((0 1,2 5,5 3,0 1)),((1 1,5 2,5 0,1 1)))", mpol);

    // Create the buffer of a multi polygon
    boost::geometry::buffer(mpol, result,
                distance_strategy, side_strategy,
                join_strategy, end_strategy, circle_strategy);*/

    {
        std::ofstream svg("output.svg");
        boost::geometry::svg_mapper<point> mapper(svg, 1024, 1024);
        mapper.add(ls);
        mapper.add(result);

        mapper.map(ls, "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
        mapper.map(result, "fill-opacity:0.5;fill:rgb(204,153,0);stroke:rgb(202,153,0);stroke-width:2");
    }
    return 0;
}
#else

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/io/io.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <fstream>
#include <iostream>

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<float> BoostPoint;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;

int main() {
    BoostPolygon input;
    BoostMultipolygon output;

    boost::geometry::read_wkt("POLYGON((61.2101898 81.9854202, 61.3715706 82.0616913, 61.4335442 82.1924744, 61.4778328 82.2606735, 61.5202942 82.3236465, 61.5283432 82.3527832, 61.5431557 82.4063950, 61.5221367 82.4381790, 61.3944855 82.4706116, 61.3497124 82.4679184, 61.3284111 82.4674301, 61.1539803 82.3401947, 61.1297760 82.2854843, 61.0671043 82.1489639, 61.0682831 82.0264740, 61.0667953 82.0112915, 61.0663414 82.0066376, 61.0707321 81.9976196, 61.0998306 81.9980850, 61.2101898 81.9854202))", input);
    {
        std::string reason;
        if (!bg::is_valid(input, reason))
            std::cout << "Input is not valid: " << reason << "\n";
    }
    bg::correct(input);
    {
        std::string reason;
        if (!bg::is_valid(input, reason))
            std::cout << "Input is not valid: " << reason << "\n";
        else
            std::cout << "Input is valid";
    }

    // Declare boost strategies for buffer function.
    bg::strategy::buffer::distance_symmetric<double> distance_strategy(-0.05);
    bg::strategy::buffer::join_round join_strategy;
    bg::strategy::buffer::end_round end_strategy;
    bg::strategy::buffer::point_circle point_strategy;
    bg::strategy::buffer::side_straight side_strategy;
    // Perform polygon buffering.
    bg::buffer(input, output, distance_strategy, side_strategy, join_strategy, end_strategy, point_strategy);

    {
        std::ofstream svg("output.svg");
        boost::geometry::svg_mapper<BoostPoint> mapper(svg, 400, 400);
        mapper.add(output);
        mapper.add(input);

        mapper.map(input, "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
        mapper.map(output, "fill-opacity:0.5;fill:rgb(204,153,0);stroke:rgb(202,153,0);stroke-width:2");
    }
}

#endif
