#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define CLAMP(a,min,max) { if( (a) < (min) ) (a)=(min); else if( (a) > (max) ) (a)=(max); }

// 9/15/2022

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;


#include "vec.h"
#include "objfile.h"
#include "brdf.h"

#define PI  3.14159265358979323846

const double twopi = 2.0f * PI;

float gauss_distrib( float proj ){
  const float m = 0.5f;
  //static int  first = 1;
  //if( first ){
  //  printf( "gaussian distribution roughness: %f\n", m );
  //  first = 0;
  //}
  CLAMP( proj, 0.0f, 1.0f );
  float  w = acosf( proj ) / m;
  return exp( -(w*w) );
}

float lamb_distrib( float proj ){
  return 1.0f;
}


int main(){
  printf( "hello world!\n" );


  obj_t obj;
  if( !obj_load( &obj, "hemi.obj", 0 ) ){
    printf( "aborting. missing hemisphere object\n" );
    return 0;
  }

  brdf_expr_t brdf = brdf_expr_load();
  if( !brdf.e ){
    printf( "aborting. invalid brdf definition\n" );
    return 0;
  }
  else
    brdf_expr_print(brdf);


  double a = 0.0;
  for( uint32 i = 0; i < obj.num_fs; i++ )
    a += obj.fs[i].area * obj.fs[i].c[2];

  printf( "PI approximation: %f\n", (float)a );
  printf( "accuracy........: %f\n", PI / a );

  double h = 0.0;
  for( uint32 i = 0; i < obj.num_vs; i++ ){
    h += lamb_distrib( obj.vs[i].z ) * obj.vs[i].z;
  }
  h /= (double)obj.num_vs;

  printf( "sine sum........: %f\n", (float)h );
  printf( "accuracy........: %f\n", 0.5f / (float)h );

  a = 0.0;
  for( uint32 i = 0; i < obj.num_fs; i++ ){
    obj_f_t *f = &obj.fs[i];
    a += ( f->area * brdf_expr_eval( &brdf, f->c[2] ) * f->c[2] );
  }

  printf( "area............: %f\n", (float)a );

  double max_sq = 0.0;
  for( uint32 i = 0; i < obj.num_vs; i++ ){
    dvec3_muls( obj.vs[i].p, brdf_expr_eval( &brdf, obj.vs[i].z ) * obj.vs[i].z );
    float len_sq = dvec3_lensq( obj.vs[i].p );
    if( len_sq > max_sq ){
      max_sq = len_sq;
    }
  }
  if( max_sq > 0.0 ){
    double scale = 1.0 / sqrt( max_sq );
    for( uint32 i = 0; i < obj.num_vs; i++ ){
      dvec3_muls( obj.vs[i].p, scale );
      float len_sq = dvec3_lensq( obj.vs[i].p );
      if( len_sq <= 0.001 * 0.001 )
        dvec3_zero( obj.vs[i].p );
    }
  }

  obj_write( &obj, "lobe.obj" );

  obj_term( &obj );
  printf( "goodbye!\n" );
  return 0;

  /*
  a = 0.0;
  for( size_t i = 0; i < obj.num_fs; i++ ){
    obj_f_t *f = &obj.fs[i];
    dvec3 u, v, n;
    dvec3_make ( u, obj.vs[ f->i0 ].p, obj.vs[ f->i1 ].p );
    dvec3_make ( v, obj.vs[ f->i0 ].p, obj.vs[ f->i2 ].p );
    dvec3_cross( u, v, n );
    a += ( dvec3_norm( n ) / 2.0f * gauss_distrib( f->c[2] ) * f->c[2] );
  }
  printf( "area (method 2).: %f (BAD?)\n", a );
  */


  /*
  fp = fopen( "lobeclipped.obj", "w" );
  for( size_t i = 0; i < nvs; i++ )
    fprintf( fp, "v %f %f %f\n",
             vs[i].p[0], vs[i].p[1], vs[i].p[2] );
  float3 n;
  float  f = sinf( 5.0f * PI / 180.0f );
  n[2] = f;
  n[1] = 0.0f;
  n[0] = sqrtf( 1.0f - n[2]*n[2] );
  for( size_t i = 0; i < nfs; i++ ){
    if( f3dot( fs[i].c, n ) >= 0.0f )
      fprintf( fp, "f %d %d %d\n",
               fs[i].i0+1, fs[i].i1+1 ,fs[i].i2+1 );
  }
  fclose( fp );
  */

}
