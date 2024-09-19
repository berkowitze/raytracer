#ifndef PERLIN_H
#define PERLIN_H

#include "util.h"

class perlin
{
public:
  perlin()
  {
    for (int i = 0; i < dimension; i++)
    {
      randfloat[i] = random_double();
    }

    perlin_generate_perm(perm_x);
    perlin_generate_perm(perm_y);
    perlin_generate_perm(perm_z);
  }

  double noise(const point3 &p) const
  {
    // get fractional parts of position vector to get (u, v, w) in the unit cube
    double u = p.x() - std::floor(p.x());
    double v = p.y() - std::floor(p.y());
    double w = p.z() - std::floor(p.z());
    // hermite interpolation (smoothstep)
    // https://www.desmos.com/calculator/ymxuhk2oqn
    u = u * u * (3 - 2 * u);
    v = v * v * (3 - 2 * v);
    w = w * w * (3 - 2 * w);

    int i = int(std::floor(p.x()));
    int j = int(std::floor(p.y()));
    int k = int(std::floor(p.z()));

    double c[2][2][2]; // c is noise values at vertices of points in cube around p
    // populate c with random values determined by a hash of the position and cube vertex
    for (int di = 0; di < 2; di++)
    {
      for (int dj = 0; dj < 2; dj++)
      {
        for (int dk = 0; dk < 2; dk++)
        {
          c[di][dj][dk] = randfloat[perm_x[(i + di) & 255] ^
                                    perm_y[(j + dj) & 255] ^
                                    perm_z[(k + dk) & 255]];
        }
      }
    }
    return trilinear_interp(c, u, v, w);
  }

private:
  static const int dimension = 256;
  double randfloat[perlin::dimension];
  int perm_x[perlin::dimension];
  int perm_z[perlin::dimension];
  int perm_y[perlin::dimension];

  static void perlin_generate_perm(int *perm)
  {
    for (int i = 0; i < dimension; i++)
    {
      perm[i] = i;
    }
    permute(perm);
  }

  static void permute(int *perm)
  {
    // move around all the elements of the array
    for (int i = (dimension - 1); i > 0; i--)
    {
      int target = random_int(0, i);
      int tmp = perm[i];
      perm[i] = target;
      perm[target] = tmp;
    }
  }

  // Compute the interpolated value at the point (u, v, w) in a unit cube based on the weights
  // of the 8 vertices defined by c. The value will be weighted more towards the noise values
  // of closer cube vertices.
  static double trilinear_interp(double c[2][2][2], double u, double v, double w)
  {
    auto accum = 0.0;
    for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
        for (int k = 0; k < 2; k++)
          accum += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) * (k * w + (1 - k) * (1 - w)) * c[i][j][k];

    return accum;
  }
};

#endif