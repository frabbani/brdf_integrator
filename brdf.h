#pragma once

// https://github.com/zserge/expr/blob/master/README.md
#include "expr.h"

float brdf_cosine;

float dot_func(struct expr_func *f, vec_expr_t *args, void *c ){
  return brdf_cosine;
}

static float acos_func(struct expr_func *f, vec_expr_t *args, void *c ){
  float a = expr_eval (&vec_nth(args, 0));
  return acosf(a);
}

float exp_func(struct expr_func *f, vec_expr_t *args, void *c ){
  float a = expr_eval (&vec_nth(args, 0));
  return exp(a);
}

float pow_func(struct expr_func *f, vec_expr_t *args, void *c ){
  return brdf_cosine;
}

static struct expr_func brdf_user_funcs[] = {
  { "dot",  dot_func,   NULL, 0 },
  { "acos", acos_func,  NULL, 0 },
  { "exp",  exp_func,   NULL, 0 },
  //{ "pow",  exp_func,   NULL, 0 },
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

double brdf_expr_eval( const brdf_expr_t *brdf, float cosine ){
  brdf_cosine = cosine;
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
/*

  struct expr_var_list vars = {0};
  struct expr *e = expr_create( s, strlen(s), &vars, user_funcs );
  if( e == NULL ){
    printf ("Syntax error");
    return 1;
  }
  float result = expr_eval( e );
  printf("result: %f\n", result);

  expr_var_t *v = vars.head;
  while(v){
    printf( "%s: %f\n", v->name, v->value );
    v = v->next;
  }

  expr_var_t *r = expr_var( &vars, "r", 1 );


  for( int i = 0; i <= 10; i++ ){
    x = (float)i / 10.0f;
    expr_eval( e );
    printf( "%f\n", r->value );
    //expr_var_t *v = vars.head;
    //while(v){
    //  printf( "%s: %f\n", v->name, v->value );
    //  v = v->next;
    //}
    printf( "***\n");
  }

  expr_destroy( e, &vars );
 */
