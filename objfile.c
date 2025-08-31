#include "objfile.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "vec.h"

void obj_term(obj_t* obj) {
  if (obj->vs)
    free(obj->vs);
  if (obj->fs)
    free(obj->fs);
  memset(obj, 0, sizeof(obj_t));
}

int obj_load(obj_t* obj, const char objfile[], int term) {
  if (!obj) {
    printf("%s:error - you has no OBJ\n", __FUNCTION__);
    return 0;
  }
  if (!objfile) {
    printf("%s:error - OBJ file not specified\n", __FUNCTION__);
    return 0;
  }
  if (term)
    obj_term(obj);
  else
    memset(obj, 0, sizeof(obj_t));

  FILE* fp = fopen(objfile, "r");
  if (!fp) {
    printf("%s:error - failed to read OBJ file '%s'\n", __FUNCTION__, objfile);
    return 0;
  }

  char line[256];

  uint32_t num_fs = 0;
  uint32_t num_vs = 0;
  while (fgets(line, sizeof(line), fp)) {
    if ('f' == line[0] && ' ' == line[1])
      num_fs++;
    if ('v' == line[0] && ' ' == line[1])
      num_vs++;
  }
  if (!num_fs || !num_vs) {
    fclose(fp);
    printf("%s:error - OBJ file '%s' is invalid\n", __FUNCTION__, objfile);
    return 0;
  }

  strcpy(obj->file, objfile);
  obj->vs = malloc(num_vs * sizeof(obj_v_t));
  obj->fs = malloc(num_fs * sizeof(obj_f_t));
  fseek(fp, 0, SEEK_SET);
  while (fgets(line, sizeof(line), fp)) {
    if ('f' == line[0] && ' ' == line[1]) {
      char *toks[3], *p;
      toks[0] = strtok(&line[2], " \n");
      toks[1] = strtok(NULL, " \n");
      toks[2] = strtok(NULL, " \n");
      if (!toks[0] || !toks[1] || !toks[2]) {
        printf("%s:warning - invalid face %u\n", __FUNCTION__, obj->num_fs);
        continue;
      }
      p = strchr(toks[0], '/');
      if (p)
        *p = '\0';
      p = strchr(toks[1], '/');
      if (p)
        *p = '\0';
      p = strchr(toks[2], '/');
      if (p)
        *p = '\0';

      obj_f_t* f = &obj->fs[obj->num_fs++];
      f->i0 = (uint32_t)atoi(toks[0]) - 1;
      f->i1 = (uint32_t)atoi(toks[1]) - 1;
      f->i2 = (uint32_t)atoi(toks[2]) - 1;
    }
    if ('v' == line[0] && ' ' == line[1]) {
      obj_v_t* v = &obj->vs[obj->num_vs++];
      sscanf(&line[2], "%lf %lf %lf", &v->x, &v->y, &v->z);
    }
  }
  fclose(fp);

  for (uint32_t i = 0; i < obj->num_fs; i++) {
    obj_f_t* f = &obj->fs[i];
    f->vs[0] = obj->vs[f->i0];
    f->vs[1] = obj->vs[f->i1];
    f->vs[2] = obj->vs[f->i2];

    dvec3 u, v;
    dvec3_make(u, f->vs[0].p, f->vs[1].p);
    dvec3_make(v, f->vs[0].p, f->vs[2].p);
    dvec3_cross(u, v, f->n);
    f->area = 0.5 * dvec3_norm(f->n);
    dvec3_copy(f->c, f->vs[0].p);
    dvec3_add(f->c, f->vs[1].p);
    dvec3_add(f->c, f->vs[2].p);
    dvec3_muls(f->c, 1.0 / 3.0);
    dvec3_norm(f->c);
  }

  return 1;
}

int obj_write(const obj_t* obj, const char objfile[]) {
  if (!obj) {
    printf("%s:error - you has no OBJ\n", __FUNCTION__);
    return 0;
  }

  const char* file = objfile;
  if (!file)
    file = obj->file;

  if (!obj->num_fs || !obj->num_vs || !obj->fs || !obj->vs) {
    printf("%s:error - invalid OBJ\n", __FUNCTION__);
    return 0;
  }

  FILE* fp = fopen(objfile, "w");
  fprintf(fp, "# %u verts:\n", obj->num_vs);
  for (uint32_t i = 0; i < obj->num_vs; i++)
    fprintf(fp, "v %f %f %f\n", (float)obj->vs[i].x, (float)obj->vs[i].y, (float)obj->vs[i].z);

  fprintf(fp, "# %u faces:\n", obj->num_fs);
  for (uint32_t i = 0; i < obj->num_fs; i++) {
    obj_f_t* f = &obj->fs[i];

    // skip degenerate
    int num_zeros = 0;
    if (0.0 == dvec3_lensq(obj->vs[f->i0].p))
      num_zeros++;
    if (0.0 == dvec3_lensq(obj->vs[f->i1].p))
      num_zeros++;
    if (0.0 == dvec3_lensq(obj->vs[f->i1].p))
      num_zeros++;
    if (num_zeros >= 2)
      continue;

    fprintf(fp, "f %u %u %u\n", f->i0 + 1, f->i1 + 1, f->i2 + 1);
  }

  fclose(fp);
  return 1;
}
