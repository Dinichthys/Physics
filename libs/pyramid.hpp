#ifndef PYRAMID_HPP
#define PYRAMID_HPP

#
#include "square.hpp"
#include "triangle.hpp"

class Pyramid : public Square {
    private:
        Triangle triangle_1_;
        Triangle triangle_2_;
        Triangle triangle_3_;
        Triangle triangle_4_;

    public:
        explicit Pyramid(const Plane& platform,
                 const Coordinates& point_a,
                 const Coordinates& point_b,
                 const Coordinates& point_c,
                 const Coordinates& point_d,
                 const Coordinates& point_e)
            :Square(platform, point_a, point_b, point_c, point_d),
             triangle_1_(Plane(point_e,
                               ((point_e - point_a) || (point_e - point_b)),
                               graphics::Color(platform.GetColor()),
                               platform.GetCoeffReflection(),
                               platform.GetCoeffAbsorption(),
                               platform.GetCoeffRefraction()), point_e, point_a, point_b),
             triangle_2_(Plane(point_e,
                               ((point_e - point_b) || (point_e - point_c)),
                               graphics::Color(platform.GetColor()),
                               platform.GetCoeffReflection(),
                               platform.GetCoeffAbsorption(),
                               platform.GetCoeffRefraction()), point_e, point_b, point_c),
             triangle_3_(Plane(point_e,
                               ((point_e - point_c) || (point_e - point_d)),
                               graphics::Color(platform.GetColor()),
                               platform.GetCoeffReflection(),
                               platform.GetCoeffAbsorption(),
                               platform.GetCoeffRefraction()), point_e, point_c, point_d),
             triangle_4_(Plane(point_e,
                               ((point_e - point_d) || (point_e - point_a)),
                               graphics::Color(platform.GetColor()),
                               platform.GetCoeffReflection(),
                               platform.GetCoeffAbsorption(),
                               platform.GetCoeffRefraction()), point_e, point_d, point_a) {};

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const override {
            bool result_base = Square::GetIntersection(start_pos, vec, coeff);
            bool result_triangle_1 = triangle_1_.GetIntersection(start_pos, vec, coeff);
            bool result_triangle_2 = triangle_2_.GetIntersection(start_pos, vec, coeff);
            bool result_triangle_3 = triangle_3_.GetIntersection(start_pos, vec, coeff);
            bool result_triangle_4 = triangle_4_.GetIntersection(start_pos, vec, coeff);

            return result_base
                   || result_triangle_1
                   || result_triangle_2
                   || result_triangle_3
                   || result_triangle_4;
        };
};

#endif // PYRAMID_HPP
