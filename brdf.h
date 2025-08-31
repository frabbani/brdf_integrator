#pragma once

#include "expr.h"

real brdf_cosine;

real dotr_func(struct expr_func *f, vec_expr_t *args, void *c ){
  return brdf_cosine;
}

real acos_func(struct expr_func *f, vec_expr_t *args, void *c ){
  double a = (double)expr_eval( &vec_nth( args, 0 ) );
  return (real)acos( a );
}

real exp_func(struct expr_func *f, vec_expr_t *args, void *c ){
  double a = (double)expr_eval( &vec_nth( args, 0 ) );
  return (real)exp( a );
}

real pow_func(struct expr_func *f, vec_expr_t *args, void *c ){
  double a = (double)expr_eval( &vec_nth( args, 0 ) );
  double b = (double)expr_eval( &vec_nth( args, 1 ) );
  return (real)pow( a, b );
}


static struct expr_func brdf_user_funcs[] = {
  { "dotr", dotr_func, NULL, 0 },
  { "acos", acos_func, NULL, 0 },
  { "exp", exp_func, NULL, 0 },
  { "pow", pow_func, NULL, 0 },
  { NULL, NULL, NULL, 0 },
};


typedef struct{
  char  file[256];
  char *src;
  expr_t *e;
  expr_var_list_t l;
  expr_var_t *o;

}brdf_expr_t;

void brdf_expr_term( brdf_expr_t *brdf ){
  if( brdf ){
    if( brdf->src ){
      free( brdf->src );
      brdf->src = NULL;
    }
    expr_destroy( brdf->e, &brdf->l );
    memset( &brdf->l, 0, sizeof(expr_var_list_t) );
    brdf->e = NULL;
    brdf->o = NULL;
  }
}

void brdf_expr_strip_src( char **src ){
  if( !src || !*src )
    return;

  size_t len = strlen( *src );
  char *newsrc = malloc( len + 1 );

  newsrc[0] = '\0';
  char *s = *src;
  while( 1 ){
    int i = 0;
    char line[256];

    memset( line, '\0', sizeof(line) );
    while( *s != '\0' && *s != '\n' ){
      line[i++] = *s;
      s++;
    }
    line[i] = '\0';

    if( '#' == line[0] || ('/' == line[0] && '/' == line[1] ) ){
      goto LOOP;
    }
    for( i = 0; line[i] != '\0'; i++ )
      if( ' ' != line[i] && '\t' != line[i]  )
        break;
    if( '\0' == line[i] ){
      goto LOOP;
    }
    for( i = 0; line[i] != '\0'; i++ ){
      if( '/' == line[i] && '/' == line[i+1] ){
        line[i] = '\0';
        break;
      }
    }

    strcat( newsrc, line );
    strcat( newsrc, "\n" );
LOOP:
    if( *s == '\0' )
      break;
    s++;
  }

  free( *src );
  *src = newsrc;
}

int brdf_expr_load( brdf_expr_t *brdf, const char exprfile[], int term ){
  if( !brdf ){
    printf( "%s:error - you has no BRDF\n", __FUNCTION__ );
    return 0;
  }
  if( !exprfile ){
    printf( "%s:error - BRDF expression file file not specified\n", __FUNCTION__ );
    return 0;
  }
  if( term )
    brdf_expr_term( brdf );
  else
    memset( brdf, 0, sizeof(brdf_expr_t) );

  FILE *fp = fopen( exprfile, "r" );
  if( !fp ){
    printf( "%s:error - failed to read expression file '%s'\n", __FUNCTION__, exprfile );
    return 0;
  }

  int size = 0;
  char *src = NULL;
  char line[256];
  while( fgets(line, sizeof(line), fp ) )
    size += strlen(line);

  src = malloc( size + 1 );
  src[0] = '\0';

  fseek( fp, 0, SEEK_SET );
  while( fgets(line, sizeof(line), fp ) ){
    strcat( src, line );
  }
  fclose( fp );
  brdf_expr_strip_src( &src );


  expr_var_list_t l = {0};
  expr_t *e = expr_create( src, strlen(src), &l, brdf_user_funcs );
  if( !e ){
    printf ("%s:error - expression file '%s' syntax error\n", __FUNCTION__, exprfile  );
    printf( "%s", src );
    free( src );
    return 0;
  }

  expr_var_t *o = l.head;
  while( o ){
    if( 0 == strcmp( o->name, "out") )
      break;
    o = o->next;
  }
  if( !o ){
    printf ("%s:error - 'brdf.txt' does not contain output variable 'out'\n", __FUNCTION__  );
    printf( "%s\n", src );
    free( src );
    expr_destroy( e, &l );
    return 0;
  }

  brdf->src = src;
  brdf->e = e;
  brdf->l = l;
  brdf->o = o;
  return 1;
}



double brdf_expr_eval( const brdf_expr_t *brdf, double cosine ){
  brdf_cosine = (real)cosine;
  if( brdf->e )
    expr_eval( brdf->e );
  if( brdf->o )
    return (double)brdf->o->value;
  return 0.0;
}


void brdf_expr_print( const brdf_expr_t *brdf ){
  if( !brdf ){
    printf( "%s:error - you has no BRDF\n", __FUNCTION__ );
    return;
  }
  if( !brdf->src ){
    printf( "%s:error - BRDF not initialized\n", __FUNCTION__ );
  }

  const char *s = brdf->src;
  printf( "******************************\n");
  while(1){
    int i = 0;
    char line[256];
    while( *s != '\0' && *s != '\n' ){
      line[i++] = *s;
      s++;
    }
    line[i] = '\0';
    printf( "   %s\n", line );
    if( *s == '\0' )
      break;
    s++;
  }
  printf( "******************************\n");
}
