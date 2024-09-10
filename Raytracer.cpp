#include "util.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

double lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}

int main()
{

    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    camera main_camera;
    main_camera.aspect_ratio = 16.0 / 9.0;
    main_camera.image_width = 400;
    main_camera.render(world);
}
