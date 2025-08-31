#pragma once

typedef double dvec3_t[3];
typedef dvec3_t dvec3;

void dvec3_zero(dvec3 dv3);
void dvec3_copy(dvec3 dv3, const dvec3 dv3u);
void dvec3_make(dvec3 dv3, const dvec3 dv3u, const dvec3 dv3v);
void dvec3_muls(dvec3 dv3, double s);
void dvec3_add(dvec3 dv3, const dvec3 dv3u);
float dvec3_dot(const dvec3 dv3u, const dvec3 dv3v);
float dvec3_lensq(const dvec3 dv3);
void dvec3_cross(const dvec3 dv3u, const dvec3 dv3v, dvec3 dv3n);
float dvec3_norm(dvec3 dv3);
