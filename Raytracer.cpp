// Raytracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "util.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

double lerp(double a, double b, double t) {
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
