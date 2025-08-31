#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>



#include "defs.h"
#include "vec.h"
#include "objfile.h"
#include "brdf.h"

#define PI  3.14159265358979323846


int main(){
  printf( "hello world!\n" );

  obj_t obj;
  if( !obj_load( &obj, "hemi.obj", 0 ) ){
    printf( "error -hemisphere OBJ model not found\n" );
    return 0;
  }
  brdf_expr_t brdf;
  brdf_expr_load( &brdf, "brdf.txt", 0 );

  if( !brdf.e ){
    printf( "error - invalid BRDF\n" );
    return 0;
  }
  else
    brdf_expr_print( &brdf );


  double a = 0.0;
  for( uint32_t i = 0; i < obj.num_fs; i++ )
    a += obj.fs[i].area * obj.fs[i].c[2];

  printf( "PI approximation: %f\n", (float)a );
  printf( "accuracy........: %f\n", PI / a );

  double h = 0.0;
  for( uint32_t i = 0; i < obj.num_vs; i++ ){
    h += obj.vs[i].z;
  }
  h /= (double)obj.num_vs;

  printf( "sine sum........: %f\n", (float)h );
  printf( "accuracy........: %f\n", 0.5f / (float)h );

  a = 0.0;
  for( uint32_t i = 0; i < obj.num_fs; i++ ){
    obj_f_t *f = &obj.fs[i];
    a += ( f->area * brdf_expr_eval( &brdf, f->c[2] ) * f->c[2] );
  }
  printf( "area............: %f\n", (float)a );

  double max_sq = 0.0;
  for( uint32_t i = 0; i < obj.num_vs; i++ ){
    dvec3_muls( obj.vs[i].p, brdf_expr_eval( &brdf, obj.vs[i].z ) * obj.vs[i].z );
    float len_sq = dvec3_lensq( obj.vs[i].p );
    if( len_sq > max_sq ){
      max_sq = len_sq;
    }
  }
  if( max_sq > 0.0 ){
    double scale = 1.0 / sqrt( max_sq );
    for( uint32_t i = 0; i < obj.num_vs; i++ ){
      dvec3_muls( obj.vs[i].p, scale );
      float len_sq = dvec3_lensq( obj.vs[i].p );
      if( len_sq <= 0.001 * 0.001 )
        dvec3_zero( obj.vs[i].p );
    }
  }
  obj_write( &obj, "lobe.obj" );

  brdf_expr_term( &brdf );
  obj_term( &obj );

  printf( "goodbye!\n" );
  return 0;
}
