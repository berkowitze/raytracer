#ifndef TEXTURE_H
#define TEXTURE_H
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "gltf/tiny_gltf.h"

#include <memory>
#include "color.h"
#include <algorithm>
// #include "rtw_stb_image.h"

using namespace tinygltf;

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
  image_texture(Image image) : image(image) {}

  color value(double u, double v, const point3 &p) const override
  {
    u = interval(0, 1).clamp(u);
    v = 1.0 - interval(0, 1).clamp(v); // flip v to be image coordinates

    int i = u * image.width;
    int j = v * image.height;
    int start_index = 4 * i + 4 * image.width * j;
    double color_scale = 1.0 / 255.0;
    double r = color_scale * image.image[start_index];
    double g = color_scale * image.image[start_index + 1];
    double b = color_scale * image.image[start_index + 2];
    return color(r, g, b);
  }

private:
  Image image;
};

class noise_texture : public texture
{
public:
  noise_texture() : noise_texture(1.0) {}
  noise_texture(double scale) : scale(scale), perlin_generator() {}

  color value(double u, double v, const point3 &p) const override
  {
    return color(1) * 0.5 * (1.0 + perlin_generator.noise(p * scale));
    // return color(1) * perlin_generator.turb(p, 7);
    // return color(0.5) * (1 + std::sin(scale * p.z() + 10 * perlin_generator.turb(p + vec3(p.x(), 0, 0), 7)));
  }

private:
  double scale;
  perlin perlin_generator;
};

#endif