/* Comms:LatLong2.c	*/

/*
 * A more correct implementation of Lat Long to flatworld (and back)
 * conversion using the Airy spheroid model of the Earth.
*/

#include <string.h>
#include <math.h>

#include "LatLong2.h"

/*
 * Physical constants of the Earth
*/

/*
 * Scale on central meridian at longitude 2 degrees west, as applicable
 * to British National Grid with origin at 49N 2W
*/
#define Fo_BNG		0.999601271775

/*
 * Scale factor for all UTM zones is defined as 0.9996
*/
#define Fo_UTM		0.99960

/*
 * Airy Spheriod Semi-Major Axis in metres
 * WGS84 spheroid Semi-Major Axis is 6378137 m
*/
#define MajorAxis_Airy	(Fo_BNG * 6377563.396)		// "a" in original code
#define MajorAxis_WGS84	(Fo_UTM * 6378137)


/*
 * Airy Spheriod Semi-Minor Axis in metres
 * This is equal to the Major axis * (1 - 1 / f) where f is the flattening
 * factor, which for Airy is 299.3249645938.
 * For WGS84 flattening is 298.257223563, hence the semi-minor axis is 6356752.3141 m
*/
#define MinorAxis_Airy	(Fo_BNG * 6356256.9092)		// "b" in original code
#define MinorAxis_WGS84	(Fo_UTM * 6356752.3141)


static int Projection = LATLONG2_WGS84;
static double MajorAxis = MajorAxis_Airy/*WGS84*/;
static double MinorAxis = MinorAxis_Airy/*WGS84*/;

static double N = 0.0, N_2, N_3, A_2, B_2, E_2;

/* Details about the origin of our flatworld system	*/
static double Latitude0 = 0, Longitude0 = 0;


static double arc_meridian(double Pd, double Ps) {
  return  MinorAxis * (
    (1 + N + 5 / 4 * N_2 + 5 / 4 * N_3) * Pd
    - (3 * N + 3 * N_2 + 21 / 8 * N_3) * sin(Pd) * cos(Ps)
    + (15 / 8 * N_2 + 15 / 8 * N_3) * sin(2.0 * Pd) * cos(2.0 * Ps)
    - 35 / 24 * N_3 * sin(3.0 * Pd) * cos(3.0 * Ps));
}

#if 0
/*
 * Converts lat / long in radians to a flatworld offset from the origin,
 * as set by LatLong2_set_origin_from_dbl_rad().
*/
int LatLong2_llrad_to_fw(double lat, double lon, int *x_fw, int *y_fw) {
  double Ld, Pd, Ps, ArcOfMeridian;
  double tanPp, sinPp, cosPp, t1, tanPp2, cosPp3, cosPp5, tanPp4, v, p;

  if (Latitude0 == INVALIDLL || Longitude0 == INVALIDLL) {
    DEBUG(DEBUGLEVEL, ("LatLong2_llrad_to_fw: Origin not set\n"));
    return 1;
  }

  DEBUG(DEBUGLEVEL+1, ("LatLong2_llrad_to_fw: %g, %g ->", lat, lon));

  /* longitudinal delta in radians	*/
  Ld = lon - Longitude0;

  /* Sum and difference of Latitudes	*/
  Pd = lat - Latitude0;
  Ps = lat + Latitude0;

  /* Calculate arc of meridian		*/
  ArcOfMeridian = arc_meridian(Pd, Ps);

  /* tangent of latitude to the meridian	*/
  tanPp = tan(lat);	// is this correct? if so, variable name is dumb

  /* temporary terms			*/
  sinPp = sin(lat);
  cosPp = cos(lat);
  t1 = 1.0 - E_2 * pow(sinPp, 2.0);
  tanPp2 = pow(tanPp, 2.0);
  cosPp3 = pow(cosPp, 3.0);
  cosPp5 = pow(cosPp, 5.0);
  tanPp4 = pow(tanPp, 4.0);

  /* Radius of curvature in prime vertical	*/
  v = MajorAxis / sqrt(t1);

  /* Radius of curvature of Meridian		*/
  p = v * (1 - E_2) / t1;

  *x_fw = (int) round(10.0 * (Ld * v * cosPp
    + pow(Ld, 3.0) * (v / 6.0 * cosPp3 * (v / p - tanPp2))
    + pow(Ld, 5.0) * (v / 120.0 * cosPp5 * (5.0 - 18.0 * tanPp2 + tanPp4 + 14.0 * E_2 - 58.0 * tanPp2 * E_2)))
  );

  *y_fw = (int) round(10.0 * (ArcOfMeridian
    + pow(Ld, 2.0) * (v / 2.0 * sinPp * cosPp)
    + pow(Ld, 4.0) * (v / 24.0 * sinPp * cosPp3 * (5.0 - tanPp2 + 9.0 * E_2))
    + pow(Ld, 6.0) * (v / 720.0 * sinPp * cosPp5 * (61.0 - 58.0 * tanPp2 + tanPp4)))
  );

  DEBUG(DEBUGLEVEL+1, ("%d, %d\n", *x_fw, *y_fw));

  return 0;
}
#endif


/* Converts a flatworld offset from the origin to Lat Long in radians. 	*/
/* The origin is set by LatLong_set_origin_from_dbl_rad().		*/
int LatLong2_fw_to_llrad(int x_fw, int y_fw, double *lat, double *lon) {
  double Ed, t0, t1, v, v3, v5, v7, p, eta2;
  double Pp, Ps, Pd, ArcOfMeridian;
  double tanPp, tanPp2, tanPp4, tanPp6, cosPp;




 // DEBUG(DEBUGLEVEL+1, ("LatLong2_fw_to_llrad: %d, %d ->", x_fw, y_fw));

  Ed = (double) x_fw / 10.0;		// convert fw to metres
  t0 = (double) y_fw / 10.0;

  Pp = t0 / MajorAxis + Latitude0;

  do {

    /* Sum and difference of latitudes	*/
    Pd = Pp - Latitude0;
    Ps = Pp + Latitude0;

    /* Calculate arc of meridian	*/
    ArcOfMeridian = arc_meridian(Pd, Ps);

    Pp += (t0 - ArcOfMeridian) / MajorAxis;

  } while (fabs(t0 - ArcOfMeridian) > 0.001);


  /* Radius of curvature in prime vertical	*/

  t1 = 1 - E_2 * pow(sin(Pp), 2);

  v = MajorAxis / sqrt(t1);
  v3 = pow(v, 3.0);
  v5 = pow(v, 5.0);
  v7 = pow(v, 7.0);

  /* Radius of curvature of Meridian	*/
  p = v * (1 - E_2) / t1;

  /* Ratio of radius of curvatures	*/
  eta2 = v / p - 1;

  /* Tangent of latitude		*/
  tanPp = tan(Pp);
  tanPp2 = pow(tanPp, 2.0);
  tanPp4 = pow(tanPp, 4.0);
  tanPp6 = pow(tanPp, 6.0);

  cosPp = cos(Pp);

  *lat = Pp
    - pow(Ed, 2.0) * tanPp / (2.0 * p * v)
    + pow(Ed, 4.0) * (5.0 + 3.0 * tanPp2 + eta2 - 9.0 * tanPp2 * eta2) * tanPp / (24.0 * p * v3)
    - pow(Ed, 6.0) * (61.0 + 90.0 * tanPp2 + 45.0 * tanPp4) * tanPp / (720.0 * p * v5);

  *lon = Longitude0
    + (Ed / (cosPp * v))
    - (pow(Ed, 3.0) * (v / p + 2.0 * tanPp2) / (cosPp * 6.0 * v3))
    + (pow(Ed, 5.0) * (5.0 + 28.0 * tanPp2 + 24.0 * tanPp4) / (cosPp * 120.0 * v5))
    + (pow(Ed, 7.0) * (61.0 + 662.0 * tanPp2 + 1320.0 * tanPp4 + 720.0 * tanPp6) / (cosPp * 5040.0 * v7));

  //DEBUG(DEBUGLEVEL+1, ("%g, %g\n", *lat, *lon));

  return 0;
}




void LatLong2_set_projection(int p) {

  switch (p) {
    case LATLONG2_BNG :
      MajorAxis = MajorAxis_Airy;
      MinorAxis = MinorAxis_Airy;
      break;

    default:
      MajorAxis = MajorAxis_WGS84;
      MinorAxis = MinorAxis_WGS84;
      p = LATLONG2_WGS84;
      break;
  }

  Projection = p;

  N = (MajorAxis-MinorAxis) / (MajorAxis+MinorAxis);
  N_2 = N * N;
  N_3 = N * N * N;
  A_2 = MajorAxis * MajorAxis;
  B_2 = MinorAxis * MinorAxis;
  E_2 = (A_2 - B_2) / A_2;

  //DEBUG(0, ("LatLong2_set projection/spheroid: %s\n", debug_proj(p)));
}


/* Set the origin for lat/long conversion, in radians	*/
void LatLong2_set_origin_from_dbl_rad(double lat, double lon) {

  LatLong2_set_projection(Projection);

  Latitude0 = lat;
  Longitude0 = lon;
}


