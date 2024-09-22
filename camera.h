#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

class camera
{
public:
    double aspect_ratio = 1.0; // width to height ratio
    double vfov = 90;          // fov in degrees
    point3 lookfrom = vec3(0, 0, 0);
    point3 lookat = vec3(0, 0, -1);
    point3 vup = vec3(0, 1, 0);
    color background;

    double defocus_angle = 0;   // angle of ray offset from lookfrom in the defocus disk, to the rendered pixel
    double focus_distance = 10; // distance from lookfrom to plane of perfect focus

    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10; // max number of bounces for each ray

    void render(const hittable &world, const hittable &lights, int chunk, bool use_background = false)
    {
        initialize();

        if (chunk < 0)
        {
            std::cout << "P3\n"
                      << image_width << ' ' << image_height << "\n255\n";
            if (chunk == -2)
            {
                return;
            }
        }

        int num_chunks = 30;
        int rows_per_chunk = (image_height + num_chunks - 1) / num_chunks;
        int chunk_start = chunk == -1 ? 0 : rows_per_chunk * chunk;
        int chunk_end = chunk == -1               ? image_height
                        : chunk == num_chunks - 1 ? image_height
                                                  : std::min(chunk_start + rows_per_chunk, image_height);

        for (int j = chunk_start; j < chunk_end; j++)
        {
            if (chunk == -1)
            {
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            }
            for (int i = 0; i < image_width; i++)
            {
                color pixel_color = color();
                for (int s_i = 0; s_i < sqrt_samples_per_pixel; s_i++)
                {
                    for (int s_j = 0; s_j < sqrt_samples_per_pixel; s_j++)
                    {
                        ray r = get_ray(i, j, s_i, s_j);
                        pixel_color += ray_color(r, world, lights, max_depth, use_background);
                    }
                }

                write_color(std::cout, pixel_color / samples_per_pixel);
            }
        }
    }

private:
    int image_height;
    point3 camera_center;
    point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u, v, w; // camera frame basis vectors
    vec3 defocus_disk_u, defocus_disk_v;
    int sqrt_samples_per_pixel;
    double reciprocal_sqrt_samples_per_pixel;

    void initialize()
    {
        image_height = std::max(1, int(image_width / aspect_ratio));
        sqrt_samples_per_pixel = int(std::sqrt(samples_per_pixel));
        reciprocal_sqrt_samples_per_pixel = 1.0 / sqrt_samples_per_pixel;
        camera_center = lookfrom;

        // Camera
        double vfov_rad = degrees_to_radians(vfov);
        double h = std::tan(vfov_rad / 2);
        double viewport_height = 2 * h * focus_distance;
        // Don't reuse aspect_ratio here, because the min() and int() used to calculate
        // the image_height makes the effective aspect ratio different. So recalculate
        // an aspect ratio.
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        w = unit_vector(lookfrom - lookat); // point opposite way of way camera is facing
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Vectors spanning axes of the viewport
        vec3 viewport_u = viewport_width * u;
        point3 viewport_v = viewport_height * -v; // go down viewport, not up (hence negative v)

        pixel_delta_u = viewport_u / image_width;  // vector for one pixel-step in the u direction
        pixel_delta_v = viewport_v / image_height; // vector for one pixel-step in the v direction

        // Go from camera center, to viewport (using focal_length), to left side, to top left side of viewport
        vec3 viewport_upper_left = lookfrom - (focus_distance * w) - viewport_u / 2 - viewport_v / 2;

        // get basis vectors for defocus disk - first get radius of the disk
        double defocus_radius = focus_distance * std::tan(degrees_to_radians(defocus_angle / 2));
        // then reuse u and v to get the correct basis vectors
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

        // First pixel is half a (du, dv) from the top left
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    color ray_color(const ray &r, const hittable &world, const hittable &lights, int bounces_remaining, bool use_background) const
    {
        if (bounces_remaining <= 0)
        {
            return color();
        }

        // if (use_background)
        // {
        //     return background;
        // }
        // else
        // {
        //     vec3 unit_direction = unit_vector(r.direction());
        //     double a = 0.5 * (unit_direction.y() + 1.0); // scale from (-1, 1) to (0, 1)
        //     return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        // }
        // vec3 unit_direction = unit_vector(r.direction());
        // double a = 0.5 * (unit_direction.y() + 1.0); // scale from (-1, 1) to (0, 1)
        // return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);

        hit_record rec;
        // Ignore very close intersections since that could be "shadow acne" (close intersections due to rounding error)
        if (!world.hit(r, interval(0.001, infinity), rec))
        {
            return background;
        }
        // Red sphere
        // return vec3(1, 0.0, 0.0);
        // Normals sphere
        // return 0.5 * (rec.normal + color(1, 1, 1));
        // -- Actual raycasting --
        // Cast randomly along hemisphere - random scatter direction
        // vec3 direction = random_on_hemisphere(rec.normal);
        // Lambertian diffuse
        // vec3 direction = rec.normal + random_unit_vector();

        // Use hit objects' material
        ray scattered_ray;
        color attenuation;
        double pdf_value;
        color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

        bool scatters = rec.mat->scatter(r, rec, attenuation, scattered_ray, pdf_value);
        if (!scatters)
        {
            return color_from_emission;
        }

        auto p0 = make_shared<hittable_pdf>(lights, rec.p);
        auto p1 = make_shared<cosine_pdf>(rec.normal);
        mixture_pdf mixed_pdf(p0, p1);

        scattered_ray = ray(rec.p, mixed_pdf.generate(), r.time());
        pdf_value = mixed_pdf.value(scattered_ray.direction());

        double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered_ray);
        // hacky way to send rays towards cornell box light
        // point3 point_on_light = point3(random_double(213, 343), 554, random_double(227, 332));
        // vec3 to_light = point_on_light - rec.p;
        // double l_squared = to_light.length_squared();
        // to_light = unit_vector(to_light);

        // // light is facing this point
        // if (dot(to_light, rec.normal) < 0)
        // {
        //     return color_from_emission;
        // }

        // double light_area = (343 - 213) * (332 - 227);
        // double light_cosine = std::fabs(to_light.y());
        // // super close to light
        // if (light_cosine < 0.0000001)
        // {
        //     return color_from_emission;
        // }

        // pdf_value = l_squared / (light_cosine * light_area);
        // scattered_ray = ray(rec.p, to_light, r.time());

        // double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered_ray);
        color sample_color = ray_color(scattered_ray, world, lights, bounces_remaining - 1, use_background);
        color color_from_scatter = (attenuation * scattering_pdf * sample_color) / pdf_value;
        return color_from_scatter + color_from_emission;
    }

    ray get_ray(int i, int j, int s_i, int s_j) const
    {
        // stratified sampling ensures that each pixel is sampled throughout its area
        vec3 pixel_offset = sample_stratified_square(s_i, s_j);
        // vec3 pixel_offset = sample_square();
        vec3 sample_location = pixel00_loc + ((j + pixel_offset.y()) * pixel_delta_v) + ((i + pixel_offset.x()) * pixel_delta_u);
        vec3 ray_origin = (defocus_angle <= 0) ? lookfrom : defocus_disk_sample();
        vec3 ray_direction = sample_location - ray_origin;
        // double time = random_double();
        double time = 0;
        ray r(ray_origin, ray_direction, time);
        return r;
    }

    vec3 defocus_disk_sample() const
    {
        vec3 p = random_in_unit_disk();
        return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    vec3 sample_square() const
    {
        return vec3(random_double(-0.5, 0.5), random_double(-0.5, 0.5), 0);
    }

    vec3 sample_stratified_square(int s_i, int s_j) const
    {
        return vec3((s_i + random_double()) * reciprocal_sqrt_samples_per_pixel - 0.5,
                    (s_j + random_double()) * reciprocal_sqrt_samples_per_pixel - 0.5,
                    0);
    }
};

#endif