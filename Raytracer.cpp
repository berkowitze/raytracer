#include "util.h"
#include "hittable.h"
#include "hittables/hittable_list.h"
#include "material.h"
#include "bvh.h"
#include "hittables/sphere.h"
#include "hittables/quad.h"
#include "hittables/translate.h"
#include "hittables/rotate.h"
#include "hittables/constant_medium.h"

double lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}

void lots_of_balls(int chunk)
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
                // end_position = position + vec3(0, random_double(0, 0.5), 0);
                end_position = position;
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

    world = hittable_list(make_shared<bvh_node>(world));
    camera main_camera;
    main_camera.background = color(0.70, 0.80, 1.00);
    main_camera.aspect_ratio = 16.0 / 9.0;
    // For perf testing, keep these as 600 300 40
    main_camera.image_width = 600;
    main_camera.samples_per_pixel = 300;
    main_camera.max_depth = 40;
    // main_camera.image_width = 1920;
    // main_camera.samples_per_pixel = 1000;
    // main_camera.max_depth = 40;
    // main_camera.image_width = 600;
    // main_camera.samples_per_pixel = 100;
    // main_camera.max_depth = 30;

    main_camera.vfov = 20;
    main_camera.defocus_angle = 0.6;
    main_camera.focus_distance = 10;
    main_camera.lookfrom = vec3(13, 2, 3);
    main_camera.lookat = vec3(0, 0, 0);
    main_camera.render(world, *make_shared<hittable_list>(), chunk);
}

void checkered_spheres(int chunk)
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

    cam.background = color(0.70, 0.80, 1.00);

    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, *make_shared<hittable_list>(), chunk);
}

void fantasy_planet(int chunk)
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

    cam.background = color(0.70, 0.80, 1.00);

    cam.defocus_angle = 0;

    cam.render(world, *make_shared<hittable_list>(), chunk);
}

void perlin_spheres(int chunk)
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
    cam.background = color(0.70, 0.80, 1.00);

    cam.defocus_angle = 0;

    cam.render(world, *make_shared<hittable_list>(), chunk);
}

void quads(int chunk)
{
    hittable_list world;

    auto left_red = make_shared<lambertian>(color(1.0, .2, .2));
    auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, *make_shared<hittable_list>(), chunk);
}

void simple_light(int chunk)
{
    hittable_list world;

    auto perlin_texture = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(perlin_texture)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));

    auto diffuse_light_mat = make_shared<diffuse_light>(color(.1, .3, 7));
    world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), diffuse_light_mat));
    world.add(make_shared<sphere>(point3(0, 7, 0), 2, make_shared<diffuse_light>(color(3.8))));

    auto red_light_mat = make_shared<diffuse_light>(color(8, .2, .1));
    world.add(make_shared<sphere>(point3(-4, 1.5, 4), 1.5, red_light_mat));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 10000;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 20;
    cam.lookfrom = point3(26, 3, 6);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, *make_shared<hittable_list>(), chunk, true);
}

void cornell_box(int chunk)
{
    hittable_list world;

    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(.73);
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light_material = make_shared<diffuse_light>(color(15));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light_material));
    world.add(make_shared<quad>(point3(0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    // shared_ptr<hittable> box1 = box(point3(0), point3(165, 330, 165), white);
    // box1 = make_shared<rotate>(box1, 0, degrees_to_radians(15));
    // box1 = make_shared<translate>(box1, vec3(265, 1, 295));
    // world.add(box1);

    // shared_ptr<hittable> box2 = box(point3(0), point3(165), white);
    // box2 = make_shared<rotate>(box2, 0, degrees_to_radians(-18));
    // box2 = make_shared<translate>(box2, vec3(130, 1, 65));
    // world.add(box2);

    shared_ptr<hittable> box1 = box(point3(0), point3(165, 330, 165), white);
    box1 = make_shared<rotate>(box1, 1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    auto empty_material = shared_ptr<material>();
    quad lights(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), empty_material);

    shared_ptr<hittable> box2 = box(point3(0), point3(165), white);
    box2 = make_shared<rotate>(box2, 1, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(box2);

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 10;
    cam.max_depth = 50;

    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    // world = hittable_list(make_shared<bvh_node>(world));
    cam.render(world, lights, chunk, true);
}

// code copied from tutorial since it's just setup
void cornell_smoke(int chunk)
{
    hittable_list world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
    world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate>(box1, 1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate>(box2, 1, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    world.add(make_shared<constant_medium>(box1, 0.01, color(0, .1, .2)));
    world.add(make_shared<constant_medium>(box2, 0.01, color(1, .9, .8)));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 5000;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, *make_shared<hittable_list>(), chunk, true);
}

void rotate_test(int chunk)
{
    hittable_list world;
    // auto b = box(vec3(-1), vec3(1), make_shared<lambertian>(vec3(0.6, 0.1, 0.2)));
    // auto c = make_shared<translate>(b, vec3(-2, 0, 1));
    // auto d = make_shared<rotate>(c, 1, -45);
    auto white = make_shared<lambertian>(.73);
    // shared_ptr<hittable> box1 = box(point3(0), point3(165, 330, 165), white);
    // box1 = make_shared<rotate>(box1, 1, 15);
    // box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    // world.add(box1);

    shared_ptr<hittable> box2 = box(point3(0), point3(165), white);
    box2 = make_shared<rotate>(box2, 1, -35);
    // box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(box2);
    camera cam;

    // cam.aspect_ratio = 1.0;
    // cam.image_width = 400;
    // cam.samples_per_pixel = 150;
    // cam.max_depth = 50;
    // cam.background = color(.5, .5, .5);

    // cam.vfov = 40;
    // cam.lookfrom = point3(10, 5, 0);
    // cam.lookat = point3(0);
    // cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 300;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(82.5, 82.5, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    world = hittable_list(make_shared<bvh_node>(world));
    cam.render(world, *make_shared<hittable_list>(), chunk);

    // cam.render(world, *make_shared<hittable_list>(), chunk);
}

void book_2_final_scene(int chunk)
{
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.83, 0.48, 0.52));

    int boxes_per_side = 20;
    double w = 100;
    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            double x0 = -1000 + i * w;
            double z0 = -1000 + j * w;
            double y0 = 0;

            double x1 = x0 + w;
            double y1 = random_double(1, 101);
            double z1 = z0 + w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7));
    // area light
    world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    // Moving sphere
    point3 center1 = point3(400, 400, 200);
    point3 center2 = center1 + vec3(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    // Add glass and metal spheres
    world.add(make_shared<sphere>(
        point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), .1)));

    // Subsurface sphere is a dielectric sphere right around a volumetric sphere
    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.01, color(0.9, 0.2, 0.4)));
    // Also add fog to ~the entire scene
    boundary = make_shared<sphere>(point3(0), 5000, make_shared<dielectric>(1.5)); // don't think dielectric matters here
    world.add(make_shared<constant_medium>(boundary, 0.0001, color(1)));

    // Add fantasy globe
    auto worldmap = make_shared<lambertian>(make_shared<image_texture>("textures/Map-111.png"));
    world.add(make_shared<sphere>(point3(400, 200, 400), 100, worldmap));

    auto perlin_texture = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(perlin_texture)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(0.73));
    int num_boxes = 1000;
    for (int j = 0; j < num_boxes; j++)
    {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    world.add(make_shared<translate>(
        make_shared<rotate>(
            make_shared<bvh_node>(boxes2), 1, 15),
        vec3(-100, 270, 395)));

    camera cam;

    cam.aspect_ratio = 1.0;
    // cam.image_width = 1000;
    cam.image_width = 600;
    // cam.samples_per_pixel = 10000;
    cam.samples_per_pixel = 1000;
    cam.max_depth = 30;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, *make_shared<hittable_list>(), chunk, true);
}

int main(int argc, char **argv)
{
    // parse --chunk=int from argv
    int chunk = -1;
    if (argc > 1)
    {
        std::string arg = argv[1];
        if (arg.find("--chunk=") == 0)
        {
            chunk = std::stoi(arg.substr(8));
        }
    }

    switch (7)
    {
    case 1:
        lots_of_balls(chunk);
        break;
    case 2:
        checkered_spheres(chunk);
        break;
    case 3:
        fantasy_planet(chunk);
        break;
    case 4:
        perlin_spheres(chunk);
        break;
    case 5:
        quads(chunk);
        break;
    case 6:
        simple_light(chunk);
        break;
    case 7:
        cornell_box(chunk);
        break;
    case 8:
        cornell_smoke(chunk);
        break;
    case 9:
        rotate_test(chunk);
        break;
    case 10:
        book_2_final_scene(chunk);
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
