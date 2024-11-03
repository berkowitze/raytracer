#ifndef VERTEX_H
#define VERTEX_H

#include "vec3.h"

class vertex
{
public:
  vertex(vec3 position, vec3 normal, vec3 uv) : position(position), normal(normal), uv(uv) {}

  vec3 position;
  vec3 normal;
  vec3 uv;
};

#endif