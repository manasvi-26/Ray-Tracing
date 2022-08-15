#ifndef PYRAMID_H
#define PYRAMID_H

#include "utility.h"

#include "triangle.h"
#include "aarect.h"
#include "hittable_list.h"

class pyramid : public hittable  {
    public:
        pyramid() {}
        pyramid(const point3& p0, const point3& p1, const point3& p2, shared_ptr<material> ptr);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_pyramid) const override {
            point3 pyramid_box_min = pyramid_base_min;
            point3 pyramid_box_max = point3(pyramid_base_max.x(),pyramid_top.y(),pyramid_base_max.z());
            output_pyramid = aabb(pyramid_box_min, pyramid_box_max);
            return true;
        }

    public:
        point3 pyramid_base_min;
        point3 pyramid_base_max;
        point3 pyramid_top;
        hittable_list sides;
};

pyramid::pyramid(const point3& p0, const point3& p1, const point3& p2, shared_ptr<material> ptr) {
    pyramid_base_min = p0;
    pyramid_base_max = p1;
    pyramid_top = p2;

    point3 p0_rect = p0;
    point3 p1_rect = point3(p1.x(),p0.y(), p0.z());
    point3 p2_rect = p1;
    point3 p3_rect = point3(p0.x(), p0.y(), p1.z());

    sides.add(make_shared<triangle>(p0_rect,p1_rect,pyramid_top,ptr));
    sides.add(make_shared<triangle>(p1_rect,p2_rect,pyramid_top,ptr));
    sides.add(make_shared<triangle>(p2_rect,p3_rect,pyramid_top,ptr));
    sides.add(make_shared<triangle>(p3_rect,p0_rect,pyramid_top,ptr));
    sides.add(make_shared<xz_rect>(p0.x(),p1.x(),p0.z(),p1.z(),p0.y(),ptr));

}

bool pyramid::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return sides.hit(r, t_min, t_max, rec);
}

#endif