#include<bits/stdc++.h>

#include "utility.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "aarect.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "box.h"
#include "pyramid.h"
#include "rotate_y.h"
#include "bvh.h"

using namespace std;

hittable_list simple_light() {
    hittable_list objects;

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

color ray_color(const ray& r, const color& background, const bvh_node& root_node, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!root_node.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, root_node, depth-1);
}

color ray_color_normal(const ray& r, const color& background, const hittable& world){
    hit_record rec;

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;
    
    return 0.5 * (rec.normal + color(1,1,1));
}

int main() {

    // IMAGE
    const auto aspect_ratio = 1.0;
    const int image_width = 200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 30;
    const int max_depth = 50;

    // World
    vector<shared_ptr<hittable>> world;

    // auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.5, .5, .5));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));
    auto checker = make_shared<checker_texture>(color(0, 0, 0), color(1,1,1));
    auto transparent = make_shared<dielectric>(1.5);
    auto metalic_pyramid = make_shared<metal>(color(0.1,0,0.2),0);

    auto box_color = make_shared<lambertian>(color(0.7,0.1,0.5 ));
    auto pyramid_color = make_shared<lambertian>(color(0.1,0,0.2)); 
    auto sphere_color = make_shared<lambertian>(color(0.3,0.2,0.6));

    // Objects
    world.push_back(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    world.push_back(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    world.push_back(make_shared<xz_rect>(100, 400, 50, 150, 554, light));
    world.push_back(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    world.push_back(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    world.push_back(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    world.push_back(make_shared<rotate_y>(make_shared<box>(point3(200, 120, 300), point3(350, 240, 450), box_color),-15));
    world.push_back(make_shared<rotate_y>(make_shared<pyramid>(point3(400,0,100), point3(600,0,400), point3(500,250,250),pyramid_color),-15));
    world.push_back(make_shared<sphere>(point3(350,350,350),100,sphere_color));


    // Camera
    point3 lookfrom(278, 278, -800);
    point3 lookat(278, 278, 0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    color background(0,0,0);
    double vfov = 40.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);
    bvh_node root_node(world , 0,  9 , 0.0 , 0.0);

    // RENDER
    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    long long int start = std::chrono::duration_cast< std::chrono::milliseconds >(
    std::chrono::system_clock::now().time_since_epoch()
    ).count();

    for (int i = image_height-1; i >= 0; --i) {
        for (int j = 0; j < image_width; ++j) {
            cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
            color pixel_color = color(0,0,0);
            for(int s=0;s<samples_per_pixel;s++)
            {
                auto u = (j + random_double()) / (image_width-1);
                auto v = (i + random_double()) / (image_height-1);
                ray r = cam.get_ray(u,v);
                pixel_color += ray_color(r,background,root_node,max_depth);
                // pixel_color += ray_color_normal(r,background,world);

            }
            write_color(cout,pixel_color, samples_per_pixel);
        }
    }

    long long int end = std::chrono::duration_cast< std::chrono::milliseconds >(
    std::chrono::system_clock::now().time_since_epoch()
    ).count();
    double time_taken = ((double)end - (double)start)/1000.0;
    cerr << "TIME TAKEN WAS " << fixed << time_taken << setprecision(3) << endl;

    cerr << "\nDone.\n";
}