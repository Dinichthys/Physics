#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "plane.hpp"

class Triangle : public Plane {
    private:
        Coordinates point_a_;
        Coordinates point_b_;
        Coordinates point_c_;

    public:
        explicit Triangle(const Plane& plane,
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

            if (IsInhere(start_pos + vec * tmp)) {
                coeff = tmp;
                return true;
            }

            return false;
        };

        bool IsInhere(const Coordinates& point) const {
            Coordinates vec_1 = !((point - point_a_) || (point_b_ - point_a_));
            Coordinates vec_2 = !((point - point_b_) || (point_c_ - point_b_));
            Coordinates vec_3 = !((point - point_c_) || (point_a_ - point_c_));

            return ((abs((vec_1 && vec_2) - kNormModule) < kEpsilon)
                    && (abs((vec_1 && vec_3) - kNormModule) < kEpsilon));
        };
};

#endif // TRIANGLE_HPP
