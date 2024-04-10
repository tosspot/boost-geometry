
#include <string>
#include <QDebug>
#include <QtGui>

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/geometries.hpp>


#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/geometries/register/multi_polygon.hpp>

#include <boost/geometry/geometries/polygon.hpp>


// Adapt a QPointF such that it can be handled by Boost.Geometry
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QPointF, double, cs::cartesian, x, y, setX, setY)

// Adapt a QPolygonF as well.
// A QPolygonF has no holes (interiors) so it is similar to a Boost.Geometry ring
BOOST_GEOMETRY_REGISTER_RING(QPolygonF)


namespace bg = boost::geometry;


void test1()
{
    typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
    typedef bg::model::polygon<point_t> polygon_t;
    polygon_t polygon;


    QPolygonF outerPolygon = QPolygonF({QPointF(55.6645,-5.0822),QPointF(55.2845,-5.83005),QPointF(55.1139,-5.30193),QPointF(55.4257,-4.37422)});
    boost::geometry::correct(outerPolygon);

    bg::assign(polygon.outer(), outerPolygon);

    QPolygonF innerPolygon = QPolygonF({QPointF(55.5859,-5.05333),QPointF(55.4515,-5.24408),QPointF(55.3386,-4.97146),QPointF(55.5257,-4.23997)});
    boost::geometry::correct(innerPolygon);
    polygon.inners().resize(1);

    bg::assign(polygon.inners()[0], innerPolygon);


    boost::geometry::correct(polygon);

    std::string message;

    bool valid = boost::geometry::is_valid(polygon, message);

    if (!valid)
    {
        qDebug() << "why not valid? " << QString::fromStdString(message);
    }
}

void test2()
{
    typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
    typedef bg::model::ring<point_t> ring_t;
    typedef bg::model::polygon<ring_t> polygon_t;

    auto outerPolygon = QPolygonF({QPointF(55.6645,-5.0822),QPointF(55.2845,-5.83005),QPointF(55.1139,-5.30193),QPointF(55.4257,-4.37422)});
    auto innerPolygon = QPolygonF({QPointF(55.5859,-5.05333),QPointF(55.4515,-5.24408),QPointF(55.3386,-4.97146),QPointF(55.5257,-4.23997)});

//    polygon_t polygon;

//    std::string message;

//    bool valid = boost::geometry::is_valid(polygon, message);

//    if (!valid)
//    {
//        qDebug() << "why not valid? " << QString::fromStdString(message);
//    }


    bg::model::point<double, 2, bg::cs::cartesian> point1;
    bg::set<0>(point1, 1.0);
    point1.set<1>(2.0);

    double x = bg::get<0>(point1), y = point1.get<1>();
    qDebug() << x << ", " << y ;

    point_t centre = boost::geometry::return_centroid<point_t>(outerPolygon);

}


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    //test1();

    test2();
    return app.exec();
}

