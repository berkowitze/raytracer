#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include "color.h"
#include <algorithm>
#include "rtw_stb_image.h"

class texture
{
public:
  virtual ~texture() = default;

  virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public texture
{
public:
  solid_color(const color &albedo) : albedo(albedo) {}

  solid_color(double red, double green, double blue) : albedo(color(red, green, blue)) {}

  color value(double u, double v, const point3 &p) const override
  {
    return albedo;
  }

private:
  color albedo;
};

class checker_texture : public texture
{
public:
  checker_texture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd) : inv_scale(1.0 / scale), even(even), odd(odd) {}
  checker_texture(
      double scale, const color &c1, const color &c2) : inv_scale(1.0 / scale), even(std::make_shared<solid_color>(c1)), odd(std::make_shared<solid_color>(c2)) {}

  color value(double u, double v, const point3 &p) const override
  {
    int x = int(std::floor(p.x() * inv_scale));
    int y = int(std::floor(p.y() * inv_scale));
    int z = int(std::floor(p.z() * inv_scale));

    bool isEven = (x + y + z) % 2;

    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
  }

private:
  double inv_scale;
  std::shared_ptr<texture> even;
  std::shared_ptr<texture> odd;
};

class image_texture : public texture
{
public:
  image_texture(const char *filename) : image(filename) {}

  color value(double u, double v, const point3 &p) const override
  {
    if (image.height() <= 0)
    {
      return color(0, 1, 1);
    }

    u = interval(0, 1).clamp(u);
    v = 1.0 - interval(0, 1).clamp(v); // flip v to be image coordinates

    int i = u * image.width();
    int j = v * image.height();
    auto pixel = image.pixel_data(i, j);
    double color_scale = 1.0 / 255.0;
    return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
  }

private:
  rtw_image image;
};

#endif