/* Comms:LatLong2.h	*/

#ifndef __comms_latlong2_h
#define __comms_latlong2_h

#ifdef __cplusplus
extern "C" {
#endif

/**fn:*/
extern void LatLong2_set_projection(
  int proj
);

enum {
  LATLONG2_WGS84 = 0,
  LATLONG2_BNG
};
/*
 * Choose which projection / spheroid to use. The default (and correct)
 * choice is WGS84. BNG (British National Grid) was used until RTT version
 * 2473 and is provided now only for comparison purposes.
**/


/**fn:*/
extern void LatLong2_set_origin_from_dbl_rad(
  double lat,
  double lon
);
/*
 * Sets the origin of the flatworld coordinate system.
 * IE the Lat/Long that corresponds to 0,0 flatworld.
**/


extern int LatLong2_fw_to_llrad(int x_fw, int y_fw, double *lat, double *lon);



/**fn:*/
extern int LatLong2_llrad_to_fw(
  double lat,
  double lon,
  int *x_fw,
  int *y_fw
);
/*
 * Converts a lat/long in radians to flatworld.
 + Return non 0 if there was an error (eg origin not set)
**/


#ifdef __cplusplus
}
#endif


#endif	 // __comms_latlong2_h
