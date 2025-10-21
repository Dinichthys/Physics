#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "plane.hpp"

class Triangle : public Plane {
    private:
        Coordinates point_a_;
        Coordinates point_b_;
        Coordinates point_c_;

    public:
        Triangle(const Plane& plane,
                 const Coordinates& point_a,
                 const Coordinates& point_b,
                 const Coordinates& point_c)
            :Plane(plane), point_a_(point_a), point_b_(point_b), point_c_(point_c) {};

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const override {
            float tmp = coeff;
            if(!Plane::GetIntersection(start_pos, vec, tmp)) {
                return false;
            }

            Coordinates inter_point = start_pos + vec * tmp;
            Coordinates vec_1 = !((inter_point - point_a_) || (point_b_ - point_a_));
            Coordinates vec_2 = !((inter_point - point_b_) || (point_c_ - point_b_));
            Coordinates vec_3 = !((inter_point - point_c_) || (point_a_ - point_c_));

            if (((vec_1 && vec_2) > 0) && ((vec_1 && vec_3) > 0) && ((vec_2 && vec_3) > 0)) {
                coeff = tmp;
                return true;
            }

            return false;
        };

};

#endif // TRIANGLE_HPP
