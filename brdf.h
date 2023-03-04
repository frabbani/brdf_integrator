#pragma once

// https://github.com/zserge/expr/blob/master/README.md
#include "expr.h"

real brdf_cosine;

real in_func(struct expr_func *f, vec_expr_t *args, void *c ){
  return brdf_cosine;
}

real acos_func(struct expr_func *f, vec_expr_t *args, void *c ){
  real a = expr_eval( &vec_nth( args, 0 ) );
  return (real)acos( (double)a );
}

real exp_func(struct expr_func *f, vec_expr_t *args, void *c ){
  real a = expr_eval( &vec_nth( args, 0 ) );
  return (real)exp( (double)a );
}


static struct expr_func brdf_user_funcs[] = {
  { "in",   in_func,    NULL, 0 },
  { "acos", acos_func,  NULL, 0 },
  { "exp",  exp_func,   NULL, 0 },
  { NULL,   NULL,       NULL, 0 },
};


typedef struct{
  char *src;
  expr_t *e;
  expr_var_list_t l;
  expr_var_t *o;

}brdf_expr_t;


brdf_expr_t brdf_expr_load(){
  brdf_expr_t brdf = {0};
  FILE *fp = fopen( "brdf.txt", "r" );
  if( !fp )
    return brdf;

  int size = 0;
  char *src = NULL;
  char line[256];
  while( fgets(line, sizeof(line), fp ) ){
    if( '#' == line[0] ||
        ('/' == line[0] && '/' == line[1] ) )
      continue;
    size += strlen(line);
  }
  src = malloc( size + 1 );
  src[0] = '\0';

  fseek( fp, 0, SEEK_SET );
  while( fgets(line, sizeof(line), fp ) ){
    if( '#' == line[0] ||
        ('/' == line[0] && '/' == line[1] ) )
      continue;
    strcat( src, line );
  }
  fclose( fp );

  expr_var_list_t l = {0};
  expr_t *e = expr_create( src, strlen(src), &l, brdf_user_funcs );
  if( !e ){
    printf ("error - 'brdf.txt' has syntax error:\n" );
    printf( "%s\n", src );
    free( src );
    return brdf;
  }

  expr_var_t *o = expr_var( &l, "out", 3 );
  if( !o ){
    printf ("error - 'brdf.txt' does not contain output variable 'out'\n" );
    free( src );
    expr_destroy( e, &l );
    return brdf;
  }

  brdf.src = src;
  brdf.e = e;
  brdf.l = l;
  brdf.o = o;
  return brdf;
}

void brdf_expr_term( brdf_expr_t brdf ){
  free( brdf.src );
  expr_destroy( brdf.e, &brdf.l );
}

double brdf_expr_eval( const brdf_expr_t *brdf, double cosine ){
  brdf_cosine = (real)cosine;
  if( brdf->e )
    expr_eval( brdf->e );
  if( brdf->o )
    return (double)brdf->o->value;
  return 0.0;
}


void brdf_expr_print( brdf_expr_t brdf ){
  if( !brdf.src )
    return;

  char *s = brdf.src;
  printf( "******************************\n");
  while(1){
    int i = 0;
    char line[256];
    while( *s != '\0' && *s != '\n' ){
      line[i++] = *s;
      s++;
    }
    line[i++] = '\0';
    printf( "   %s\n", line );

    if( *s == '\0' )
      break;
    s++;
  }
  printf( "******************************\n");
}
