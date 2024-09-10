#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

class camera
{
public:
    double aspect_ratio = 1.0; // width to height ratio
    int image_width = 100;
    int samples_per_pixel = 10;

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
                    pixel_color += ray_color(r, world);
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

    void initialize()
    {
        image_height = std::max(1, int(image_width / aspect_ratio));

        // Camera
        camera_center = vec3();
        auto focal_length = 1.0; // distance from camera_center to center of viewport
        auto viewport_height = 2.0;
        // Don't reuse aspect_ratio here, because the min() and int() used to calculate
        // the image_height makes the effective aspect ratio different. So recalculate
        // an aspect ratio.
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Vectors spanning axes of the viewport
        vec3 viewport_u = vec3(viewport_width, 0, 0);
        point3 viewport_v = point3(0, -viewport_height, 0);

        pixel_delta_u = viewport_u / image_width;  // vector for one pixel-step in the u direction
        pixel_delta_v = viewport_v / image_height; // vector for one pixel-step in the v direction

        // Go from camera center, to viewport (using focal_length), to left side, to top left side of viewport
        vec3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        // First pixel is half a (du, dv) from the top left
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    color ray_color(const ray &r, const hittable &world) const
    {
        hit_record rec;
        if (world.hit(r, interval(0, infinity), rec))
        {
            // Red sphere
            // return vec3(1, 0.0, 0.0);
            // Normals sphere
            // return 0.5 * (rec.normal + color(1, 1, 1));
            // Actual raycasting
            vec3 direction = random_on_hemisphere(rec.normal);
            return 0.5 * ray_color(ray(rec.p, direction), world);
        }

        vec3 unit_direction = unit_vector(r.direction());
        double a = 0.5 * (unit_direction.y() + 1.0); // scale from (-1, 1) to (0, 1)
        return (1.0 - a) * color(1.0, 1.0, 0.0) + a * color(0.5, 0.7, 1.0);
    }

    ray get_ray(int i, int j)
    {
        vec3 offset = sample_square();
        vec3 sample_location = pixel00_loc + ((j + offset.y()) * pixel_delta_v) + ((i + offset.x()) * pixel_delta_u);
        // No need to be a unit vector
        vec3 ray_direction = sample_location - camera_center;
        ray r(camera_center, ray_direction);
        return r;
    }

    vec3 sample_square() const
    {
        return vec3(random_double(-0.5, 0.5), random_double(-0.5, 0.5), 0);
    }
};

#endif