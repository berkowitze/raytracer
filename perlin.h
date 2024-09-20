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
      randvec[i] = vec3::random(-1, 1).normalize();
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

    int i = int(std::floor(p.x()));
    int j = int(std::floor(p.y()));
    int k = int(std::floor(p.z()));

    vec3 c[2][2][2]; // c is noise values at vertices of points in cube around p
    // populate c with random values determined by a hash of the position and cube vertex
    for (int di = 0; di < 2; di++)
    {
      for (int dj = 0; dj < 2; dj++)
      {
        for (int dk = 0; dk < 2; dk++)
        {
          c[di][dj][dk] = randvec[perm_x[(i + di) & 255] ^
                                  perm_y[(j + dj) & 255] ^
                                  perm_z[(k + dk) & 255]];
        }
      }
    }
    return perlin_interp(c, u, v, w);
  }

  // Generate noise value with num_frequencies, with frequency doubling and amplitude
  // halving for each iteration
  double turb(const point3 &p, int num_frequencies) const
  {
    double accum = 0.0;
    vec3 temp_p = p;
    double weight = 1.0;

    for (int i = 0; i < num_frequencies; i++)
    {
      accum += weight * noise(temp_p);
      weight *= 0.5;
      temp_p *= 2;
    }

    return std::fabs(accum);
  }

private:
  static const int dimension = 256;
  vec3 randvec[perlin::dimension];
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

  static double perlin_interp(const vec3 c[2][2][2], double u, double v, double w)
  {
    // hermite interpolation for smoother transitions
    auto uu = u * u * (3 - 2 * u);
    auto vv = v * v * (3 - 2 * v);
    auto ww = w * w * (3 - 2 * w);
    auto accum = 0.0;

    for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
        for (int k = 0; k < 2; k++)
        {
          vec3 weight_v(u - i, v - j, w - k);
          accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
        }

    return accum;
  }
};

#endif