#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "utility.h"
#include "hittable.h"

class triangle : public hittable {

    public:
        triangle() {}

        triangle(const point3& p0, const point3& p1, const point3& p2, shared_ptr<material> mat):
            p0(p0), p1(p1), p2(p2), mp(mat) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

            vec3 base_vec = p1 - p0;
            vec3 slant_vec = p2 - p0;
            vec3 ray_vec = r.origin() - p0;

            vec3 n = cross(base_vec , slant_vec);
            vec3 q = cross(ray_vec , r.direction());

            double d = 1.0/(dot(r.direction() , n));
            double u = d * dot(-q , slant_vec);
            double v = d * dot(q , base_vec);

            auto t = d * dot(-n , ray_vec);
            if( u<0.0 || v<0.0 || (u+v)>1.0 ) return false;

            if (t < t_min || t > t_max)
                return false; 

            point3 intersection = r.at(t);

            rec.u = u;
            rec.v = v;
            rec.t = t;

            auto outward_normal = unit_vector(n);
            rec.set_face_normal(r , outward_normal);
            rec.mat_ptr = mp;
            rec.p = intersection;
            return true;
        }


        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {

            double min_x = fmin(p0.x(), fmin(p1.x(), p2.x()));
            double min_y = fmin(p0.y(), fmin(p1.y(), p2.y()));
            double min_z = fmin(p0.z(), fmin(p1.z(), p2.z()));
        
            double max_x = fmax(p0.x(), fmax(p1.x(), p2.x()));
            double max_y = fmax(p0.y(), fmax(p1.y(), p2.y()));
            double max_z = fmax(p0.z(), fmax(p1.z(), p2.z()));

            output_box = aabb(point3(min_x, min_y, min_z), point3(max_x, max_y, max_z));
            return true;            
        }

    public:
        shared_ptr<material> mp;
        point3 p0; 
        point3 p1; 
        point3 p2; 
};

#endif
