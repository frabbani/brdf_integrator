#include "vec.h"

#include <math.h>

void dvec3_zero(dvec3 dv3) { dv3[0] = dv3[1] = dv3[2] = 0.0; }

void dvec3_copy(dvec3 dv3, const dvec3 dv3u) {
  dv3[0] = dv3u[0];
  dv3[1] = dv3u[1];
  dv3[2] = dv3u[2];
}

void dvec3_make(dvec3 dv3, const dvec3 dv3u, const dvec3 dv3v) {
  dv3[0] = dv3v[0] - dv3u[0];
  dv3[1] = dv3v[1] - dv3u[1];
  dv3[2] = dv3v[2] - dv3u[2];
}

void dvec3_muls(dvec3 dv3, double s) {
  dv3[0] *= s;
  dv3[1] *= s;
  dv3[2] *= s;
}

void dvec3_add(dvec3 dv3, const dvec3 dv3u) {
  dv3[0] += dv3u[0];
  dv3[1] += dv3u[1];
  dv3[2] += dv3u[2];
}

float dvec3_dot(const dvec3 dv3u, const dvec3 dv3v) {
  return (dv3u[0] * dv3v[0] + dv3u[1] * dv3v[1] + dv3u[2] * dv3v[2]);
}

float dvec3_lensq(const dvec3 dv3) { return (dv3[0] * dv3[0] + dv3[1] * dv3[1] + dv3[2] * dv3[2]); }

void dvec3_cross(const dvec3 dv3u, const dvec3 dv3v, dvec3 dv3n) {
  dv3n[0] = (dv3u[1] * dv3v[2] - dv3u[2] * dv3v[1]);
  dv3n[1] = -(dv3u[0] * dv3v[2] - dv3u[2] * dv3v[0]);
  dv3n[2] = (dv3u[0] * dv3v[1] - dv3u[1] * dv3v[0]);
}

float dvec3_norm(dvec3 dv3) {
  double msq = dvec3_dot(dv3, dv3);
  if (msq > 0.0) {
    double m = sqrt(msq);
    dvec3_muls(dv3, 1.0 / m);
    return m;
  }
  dvec3_zero(dv3);
  return 0.0;
}
