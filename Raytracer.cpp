#include "util.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "bvh.h"
#include "sphere.h"

double lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}

void lots_of_balls(int argc, char **argv)
{
    hittable_list world;
    // Sphere scene
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material)); // ground is just a big sphere

    for (int i = -11; i < 11; i++)
    {
        for (int j = -11; j < 11; j++)
        {
            double mat_rng = random_double();

            point3 position = vec3(i + 0.9 * random_double(), 0.2 + random_double() * 0.1, j + 0.9 * random_double());
            point3 end_position = position;
            double radius = random_double(0.1, 0.3);

            shared_ptr<material> sphere_material;

            if (mat_rng < 0.7)
            {
                sphere_material = make_shared<lambertian>(color::random() * color::random());
                end_position = position + vec3(0, random_double(0, 0.5), 0);
            }
            else if (mat_rng < 0.9)
            {
                sphere_material = make_shared<metal>(color::random(0.5, 1), random_double(0, 0.5));
            }
            else
            {
                sphere_material = make_shared<dielectric>(1.5);
            }
            world.add(make_shared<sphere>(position, end_position, radius, sphere_material));
        }
    }

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    // Big spheres in the middle (copy-pasted code from https://raytracing.github.io/books/RayTracingInOneWeekend.html#positionablecamera)
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    if (argc > 1 && std::string(argv[1]) == "--bvh")
    {
        std::clog << "Using BVH" << std::endl;
        world = hittable_list(make_shared<bvh_node>(world));
    }
    else
    {
        std::clog << "Not using BVH" << std::endl;
    }

    camera main_camera;
    main_camera.aspect_ratio = 16.0 / 9.0;
    main_camera.image_width = 400;
    main_camera.samples_per_pixel = 75;
    main_camera.max_depth = 30;
    main_camera.vfov = 20;
    main_camera.defocus_angle = 0.6;
    main_camera.focus_distance = 10;
    main_camera.lookfrom = vec3(13, 2, 3);
    main_camera.lookat = vec3(0, 0, 0);

    main_camera.render(world);
}

void checkered_spheres(int argc, char **argv)
{
    hittable_list world;

    auto checker_tex = make_shared<checker_texture>(0.32, color(.1, .3, .2), color(.9));

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker_tex)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker_tex)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void fantasy_planet(int argc, char **argv)
{
    hittable_list world;

    auto world_tex = make_shared<image_texture>("textures/Map-111.png");

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(world_tex)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(world_tex)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(60, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void perlin_spheres(int argc, char **argv)
{
    hittable_list world;

    auto pertext = make_shared<noise_texture>(10.0);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main(int argc, char **argv)
{
    int scene = 4;
    switch (scene)
    {
    case 1:
        lots_of_balls(argc, argv);
        break;
    case 2:
        checkered_spheres(argc, argv);
        break;
    case 3:
        fantasy_planet(argc, argv);
        break;
    case 4:
        perlin_spheres(argc, argv);
        break;
    }
}

// auto grayish = make_shared<lambertian>(color(0.4, 0.3, 0.4));
// auto reddish = make_shared<lambertian>(color(0.8, 0.3, 0.2));

// // auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
// auto material_left = make_shared<dielectric>(1.5);
// auto material_bubble = make_shared<dielectric>(1.0 / 1.3);
// auto metal_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.8);

// world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, grayish));
// world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, reddish));

// world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5, material_left));
// world.add(make_shared<sphere>(point3(-1, 0, -1), 0.4, material_bubble));

// world.add(make_shared<sphere>(point3(1, 0, -1), 0.5, metal_right));

// double sphere_radius = std::cos(pi / 4);
// // blue
// auto material_left = make_shared<lambertian>(color(0, 0, 1));
// // red
// auto material_right = make_shared<lambertian>(color(1, 0, 0));

// world.add(make_shared<sphere>(point3(-sphere_radius, 0, -1), sphere_radius, material_left));
// world.add(make_shared<sphere>(point3(sphere_radius, 0, -1), sphere_radius, material_right));
