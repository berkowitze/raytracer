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

    void render(const hittable &world)
    {
        initialize();

        std::cout << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                color pixel_color = color();
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, world, max_depth);
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

    void initialize()
    {
        image_height = std::max(1, int(image_width / aspect_ratio));
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

    color ray_color(const ray &r, const hittable &world, int bounces_remaining) const
    {
        if (bounces_remaining <= 0)
        {
            return color();
        }

        hit_record rec;
        // Ignore very close intersections since that could be "shadow acne" (close intersections due to rounding error)
        if (world.hit(r, interval(0.001, infinity), rec))
        {
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
            color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

            bool scatters = rec.mat->scatter(r, rec, attenuation, scattered_ray);
            if (scatters)
            {
                color color_from_scatter = attenuation * ray_color(scattered_ray, world, bounces_remaining - 1);
                return color_from_scatter + color_from_emission;
            }
            else
            {
                return color_from_emission;
            }
        }
        else
        {
            return background;
        }

        vec3 unit_direction = unit_vector(r.direction());
        double a = 0.5 * (unit_direction.y() + 1.0); // scale from (-1, 1) to (0, 1)
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    ray get_ray(int i, int j)
    {
        vec3 pixel_offset = sample_square();
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
};

#endif