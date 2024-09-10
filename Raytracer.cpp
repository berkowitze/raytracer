#include "util.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

double lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}

int main()
{

    hittable_list world;

    auto grayish = make_shared<lambertian>(color(0.4, 0.3, 0.4));
    auto reddish = make_shared<lambertian>(color(0.8, 0.3, 0.2));

    // auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_left = make_shared<dielectric>(1.5);
    auto material_bubble = make_shared<dielectric>(1.0 / 1.3);
    auto metal_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.8);

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, grayish));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, reddish));

    world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1, 0, -1), 0.4, material_bubble));

    world.add(make_shared<sphere>(point3(1, 0, -1), 0.5, metal_right));

    camera main_camera;
    main_camera.aspect_ratio = 16.0 / 9.0;
    main_camera.image_width = 400;
    main_camera.samples_per_pixel = 50;
    main_camera.max_depth = 50;
    main_camera.render(world);
}
