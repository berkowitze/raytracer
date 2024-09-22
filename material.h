#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "texture.h"

class scatter_record
{
public:
  color attenuation;
  shared_ptr<pdf> pdf_ptr;
  bool skip_pdf;
  ray skip_pdf_ray;
};

class material
{
public:
  virtual ~material() = default;

  // Return whether or not the ray scatters
  virtual bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const
  {
    return false;
  }

  virtual color emitted(const ray &r_in, const hit_record &rec, double u, double v, const point3 &p) const
  {
    return color(0);
  }

  virtual double scattering_pdf(const ray &r_in, const hit_record &hit_rec, const ray &scattered) const
  {
    return 0;
  }
};

class lambertian : public material
{
public:
  lambertian(const color &albedo) : tex(make_shared<solid_color>(albedo)) {}
  lambertian(shared_ptr<texture> tex) : tex(tex) {}

  bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const override
  {
    srec.attenuation = tex->value(rec.u, rec.v, rec.p);
    srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal); // scatter in cosine-weighted direction
    srec.skip_pdf = false;
    return true;
  }

  double scattering_pdf(const ray &r_in, const hit_record &hit_rec, const ray &scattered) const override
  {
    double cos_theta = dot(hit_rec.normal, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / pi;
  }

private:
  shared_ptr<texture> tex;
};

class metal : public material
{
public:
  metal(const color &albedo, double fuzz_factor) : albedo(albedo), fuzz_factor(fuzz_factor) {}
  bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const override
  {
    vec3 scatter_direction = reflect(r_in.direction(), rec.normal);
    scatter_direction.normalize();
    scatter_direction += (fuzz_factor * random_unit_vector());

    srec.attenuation = albedo;
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;
    srec.skip_pdf_ray = ray(rec.p, scatter_direction, r_in.time());

    return true;

    // Make sure the scattered direction is not now on the opposite side of the surface
    // return (dot(scattered_ray.direction(), rec.normal) > 0);
  }

private:
  color albedo;
  double fuzz_factor;
};

class dielectric : public material
{
public:
  dielectric(double refraction_index) : refraction_index(refraction_index) {}

  bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const override
  {
    srec.attenuation = color(1.0); // dielectric material does not absorb any light
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;

    double refractive_index_ratio = rec.front_face ? (1.0 / refraction_index) : refraction_index;
    vec3 unit_r_in_direction = unit_vector(r_in.direction());

    double cos_theta = std::fmin(dot(-unit_r_in_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
    // Some cases disobey snell's law, and so those should always reflect not refract
    bool cannot_refract = refractive_index_ratio * sin_theta > 1.0;

    // At shallow angles, light reflects more often than is transmitted - hence the reflectance test
    vec3 refracted_direction = cannot_refract || reflectance(cos_theta, refractive_index_ratio) > random_double() ? reflect(unit_r_in_direction, rec.normal) : refract(unit_r_in_direction, rec.normal, refractive_index_ratio);

    srec.skip_pdf_ray = ray(rec.p, refracted_direction, r_in.time());

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

  bool scatter(const ray &r_in, const hit_record &rec, scatter_record &srec) const override
  {
    srec.attenuation = tex->value(rec.u, rec.v, rec.p);
    srec.pdf_ptr = make_shared<sphere_pdf>(); // scatter in random direction
    srec.skip_pdf = false;
    return true;
  }

  double scattering_pdf(const ray &r_in, const hit_record &hit_rec, const ray &scattered) const override
  {
    return 1 / (4 * pi);
  }

private:
  shared_ptr<texture> tex;
};

#endif
