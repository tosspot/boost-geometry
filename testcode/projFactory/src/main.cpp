//#include <geometry_test_common.hpp>
#include <QList>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/srs/epsg.hpp>
#include <boost/geometry/srs/projection.hpp>
#include <QDebug>

#include <boost/geometry/srs/transformation.hpp>
#include <boost/geometry/strategies/transform/srs_transformer.hpp>

#include <boost/geometry/io/wkt/read.hpp>

#include "projection.h"
//#include "check_geometry.hpp"

#if 1

int main(int, char*[])
{
    using namespace boost::geometry;
    using namespace boost::geometry::model;
    using namespace boost::geometry::srs;

    using point_ll = point<double, 2, cs::geographic<degree> >;
    using point_xy = point<double, 2, cs::cartesian>;

    using namespace boost::geometry::srs::dpar;

    point_ll pt_ll(-3, 54);
    point_ll pt_ll2(0, 0);
    point_xy pt_xy(0, 0);

    projection<> prj = parameters<>(proj_tmerc)(ellps_wgs84)(units_m)(lat_0, 33.4);
    projection<> prj1 = parameters<>(proj_merc)(ellps_wgs84)(units_m);

    projection<> prj2;



    //projection< proj_tmerc, ellps_wgs84, units_m> prje1;





    QList<projection<>> prjList;
    prjList.append(prj);
    prjList.append(prj1);

    prj.forward(pt_ll, pt_xy);
    //  test::check_geometry(pt_xy, "POINT(111308.33561309829 110591.34223734379)", 0.001);


    qDebug() << Q_FUNC_INFO << pt_xy.get<0>() << pt_xy.get<1>();
    prj1.forward(pt_ll, pt_xy);

    qDebug() << Q_FUNC_INFO << pt_xy.get<0>() << pt_xy.get<1>();

    prj.inverse(pt_xy, pt_ll2);
    //test::check_geometry(pt_ll2, "POINT(1 1)", 0.001);



    // Initialize projection parameters. For construction using a factory the projection name is required.
 //   projections::parameters par = parameters<>(proj_tmerc)(ellps_wgs84)(units_m)(lat_0, 33.4);; //projections::init("+proj=robin +ellps=WGS84 +units=m");

    // Construct the specified projection, using specified point types
    // Note that this is the only difference from p01_projection_example. It constructs a projection
    // with virtual methods, which can be used polymorphically. Therefore it is a pointer. For
    // convenience we use a boost shared pointer here.

  //  projections::detail::factory<point_ll, point_xy, par> fac;




    //boost::shared_ptr<projections::projection<point_ll_deg, point_xy> > prj(fac.create_new(par));

    // Define Amsterdam / Barcelona in decimal degrees / degrees/minutes







    return 0;
}
#else

// Boost.Geometry
// Unit Test

// Copyright (c) 2017-2018, Oracle and/or its affiliates.
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


//#include <geometry_test_common.hpp>

#include <QList>



//#include "check_geometry.hpp"
namespace bg = boost::geometry;


int main(int, char*[])
{
    using namespace boost::geometry;
    using namespace boost::geometry::model;
    using namespace boost::geometry::srs;
    using namespace boost::geometry::strategy::transform;

    typedef point<double, 2, cs::geographic<degree> > point_ll;
    typedef point<double, 2, cs::cartesian> point_xy;
    //typedef polygon<point_ll> polygon_ll;
    //typedef polygon<point_xy> polygon_xy;


    bg::srs::projection<> prj = bg::srs::proj4("+proj=tmerc +lat_0=48.693665 +lon_0=9.002912 +ellps=sphere +k_0=1.0 +x_0=4567.9 +y_0=4488.4");


    using namespace srs::spar;
    parameters<proj_tmerc> toss;



    using namespace boost::geometry::srs;

    {
        point_ll pt_ll(1, 1);
        point_ll pt_ll2(0, 0);
        point_xy pt_xy(0, 0);
        point_xy pt_xy2(0, 0);

        boost::geometry::srs::transformation<> x{
            bg::srs::proj4("+proj=latlon +datum=WGS84 +no_defs"),
            bg::srs::proj4("+proj=lcc +lat_1=51.16666723333333 +lat_2=49.8333339 "
                           "+lat_0=90 +lon_0=4.367486666666666 "
                           "+x_0=150000.013 +y_0=5400088.438 +ellps=intl "
                           "+towgs84=­106.869,52.2978,­103.724,0.3366,"
                                    "­0.457,1.8422,­1.2747 "
                           "+units=m +no_defs")
        };
        using namespace boost::geometry::srs::spar;
        
        projection<parameters<proj_merc> > prj2;
        
        QList<projection<>> prjx;



        prjx.append(prj);
        boost::geometry::srs::projection<boost::geometry::srs::static_epsg<3395> > xx;

        using namespace boost::geometry::srs::dpar;


        //prjx.append(prj2);


        //projection<parameters<proj_merc> > prj2;





        srs_forward_transformer<projection<> > strategy_pf = proj4("+proj=tmerc +ellps=WGS84 +units=m");
        srs_inverse_transformer<projection<> > strategy_pi = proj4("+proj=tmerc +ellps=WGS84 +units=m");
        srs_forward_transformer<transformation<> > strategy_tf(proj4("+proj=tmerc +ellps=WGS84 +units=m"),
                                                               proj4("+proj=tmerc +ellps=clrk66 +units=m"));
        srs_inverse_transformer<transformation<> > strategy_ti(proj4("+proj=tmerc +ellps=WGS84 +units=m"),
                                                               proj4("+proj=tmerc +ellps=clrk66 +units=m"));

        transform(pt_ll, pt_xy, strategy_pf);
        //test::check_geometry(pt_xy, "POINT(111308.33561309829 110591.34223734379)", 0.0001);

        transform(pt_xy, pt_ll2, strategy_pi);
        //test::check_geometry(pt_ll2, "POINT(1 1)", 0.0001);

        transform(pt_xy, pt_xy2, strategy_tf);
        //test::check_geometry(pt_xy2, "POINT(111309.54843459482 110584.27813586517)", 0.0001);

        transform(pt_xy2, pt_xy, strategy_ti);
        //test::check_geometry(pt_xy, "POINT(111308.33561309829 110591.34223734379)", 0.0001);
    }

    int epsgCode = 27700;


  //  auto *proj = Projection::instance():


      //  proj->

    {
        srs_forward_transformer<projection<> > strategy_pf = epsg(epsgCode);
        srs_inverse_transformer<projection<> > strategy_pi = epsg(epsgCode);


        srs_forward_transformer<transformation<> > strategy_tf(epsg(2000), epsg(2001));
        srs_inverse_transformer<transformation<> > strategy_ti(epsg(2000), epsg(2001));
    }


    {
        srs_forward_transformer<projection<static_epsg<2000> > > strategy_pf;
        srs_forward_transformer<projection<static_epsg<2000> > > strategy_pi;
        srs_forward_transformer<transformation<static_epsg<2000>, static_epsg<2001> > > strategy_tf;
        srs_forward_transformer<transformation<static_epsg<2000>, static_epsg<2001> > > strategy_ti;
    }
    
    

    return 0;
}
#endif
