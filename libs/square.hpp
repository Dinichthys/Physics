#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "plane.hpp"

class Square : public Plane {
    private:
        Coordinates point_a_;
        Coordinates point_b_;
        Coordinates point_c_;
        Coordinates point_d_;

    public:
        explicit Square(const Plane& plane,
                 const Coordinates& point_a,
                 const Coordinates& point_b,
                 const Coordinates& point_c,
                 const Coordinates& point_d)
            :Plane(plane), point_a_(point_a), point_b_(point_b), point_c_(point_c), point_d_(point_d) {};

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
            Coordinates vec_3 = !((point - point_c_) || (point_d_ - point_c_));
            Coordinates vec_4 = !((point - point_d_) || (point_a_ - point_d_));

            return ((abs((vec_1 && vec_2) - kNormModule) < kEpsilon)
                    && (abs((vec_1 && vec_3) - kNormModule) < kEpsilon)
                    && (abs((vec_1 && vec_4) - kNormModule) < kEpsilon));
        };
};

#endif // SQUARE_HPP
