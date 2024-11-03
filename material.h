#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "texture.h"

class material
{
public:
  virtual ~material() = default;

  // Return whether or not the ray scatters
  virtual bool scatter(
      const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered_ray) const
  {
    return false;
  }

  virtual color emitted(const ray &r_in, const hit_record &rec, double u, double v, const point3 &p) const
  {
    return color(0);
  }
};

class lambertian : public material
{
public:
  lambertian(const color &albedo) : tex(make_shared<solid_color>(albedo)) {}
  lambertian(shared_ptr<texture> tex) : tex(tex) {}

  bool scatter(
      const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered_ray) const override
  {
    vec3 scatter_direction = rec.normal + random_unit_vector();
    if (scatter_direction.near_zero())
    {
      // This prevents degenerate cases of the direction being ~0
      // but it feels weird to always turn those into the normal vector direction -
      // will that reflect too much light in just that one direction?
      scatter_direction = rec.normal;
    }
    scattered_ray = ray(rec.p, scatter_direction, r_in.time());
    attenuation = tex->value(rec.u, rec.v, rec.p);
    return true;
  }

private:
  shared_ptr<texture> tex;
};

/*
class open_pbr_bsdf : public material
{
public:
  open_pbr_bsdf(texture *albedo, texture *normal, texture *metallic, texture *roughness, texture *ao) : albedo(albedo), normal(normal), metallic(metallic), roughness(roughness), ao(ao) {}
  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
  {
    vec3 scatter_direction = rec.normal + random_unit_vector();
    if (scatter_direction.near_zero())
    {
      scatter_direction = rec.normal;
    }
    scattered = ray(rec.p, scatter_direction, r_in.time());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }
  color emitted(const ray &r_in, const hit_record &rec, double u, double v, const point3 &p) const override
  {
    return color(0);
  }

private:
  texture *albedo;
  texture *normal;
  texture *metallic;
  texture *roughness;
  texture *ao;
};*/

class metal : public material
{
public:
  metal(const color &albedo, double fuzz_factor) : albedo(albedo), fuzz_factor(fuzz_factor) {}
  bool scatter(
      const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered_ray) const override
  {
    vec3 scatter_direction = reflect(r_in.direction(), rec.normal);
    scatter_direction.normalize();
    scatter_direction += (fuzz_factor * random_unit_vector());
    scattered_ray = ray(rec.p, scatter_direction, r_in.time());
    attenuation = albedo;
    // Make sure the scattered direction is not now on the opposite side of the surface
    return (dot(scattered_ray.direction(), rec.normal) > 0);
  }

private:
  color albedo;
  double fuzz_factor;
};

class dielectric : public material
{
public:
  dielectric(double refraction_index) : refraction_index(refraction_index) {}

  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
      const override
  {

    // dielectric material does not absorb any light
    attenuation = color(1.0);
    double refractive_index_ratio = rec.front_face ? (1.0 / refraction_index) : refraction_index;
    vec3 unit_r_in_direction = unit_vector(r_in.direction());

    double cos_theta = std::fmin(dot(-unit_r_in_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
    // Some cases disobey snell's law, and so those should always reflect not refract
    bool cannot_refract = refractive_index_ratio * sin_theta > 1.0;

    // At shallow angles, light reflects more often than is transmitted - hence the reflectance test
    vec3 refracted_direction = cannot_refract || reflectance(cos_theta, refractive_index_ratio) > random_double() ? reflect(unit_r_in_direction, rec.normal) : refract(unit_r_in_direction, rec.normal, refractive_index_ratio);

    scattered = ray(rec.p, refracted_direction, r_in.time());
    return true;
  }

private:
  double refraction_index;

  static double reflectance(double cos_angle, double ri)
  {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ri) / (1 + ri);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cos_angle), 5);
  }
};

class diffuse_light : public material
{
public:
  diffuse_light(shared_ptr<texture> tex) : texture(tex) {}
  diffuse_light(const color &emission_color) : texture(make_shared<solid_color>(emission_color)) {}

  color emitted(const ray &r_in, const hit_record &rec, double u, double v, const point3 &p) const override
  {
    if (!rec.front_face)
    {
      return color();
    }
    return texture->value(u, v, p);
  }

private:
  shared_ptr<texture> texture;
};

class isotropic : public material
{
public:
  isotropic(const color &albedo) : tex(make_shared<solid_color>(albedo)) {}
  isotropic(shared_ptr<texture> tex) : tex(tex) {}

  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
  {
    scattered = ray(rec.p, random_unit_vector(), r_in.time());
    attenuation = tex->value(rec.u, rec.v, rec.p);
    return true;
  }

private:
  shared_ptr<texture> tex;
};

#endif
