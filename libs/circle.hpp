#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "object.hpp"

#include "border.hpp"

class Circle : public Object {
    private:
        float radius_;

    public:
        explicit Circle(const Coordinates& center, float radius, ObjectType type = kSphere,
                        const graphics::Color& color = kIBase,
                        float coeff_reflection = NAN, float coeff_absorption = NAN,
                        float coeff_refraction = NAN)
            :Object(center, kObjectsInfo.at(type), color) {
            radius_ = radius;
            if (!isnan(coeff_refraction)) {
                Object::SetCoeffRefraction(coeff_refraction);
            }
            if (!isnan(coeff_absorption)) {
                Object::SetCoeffAbsorption(coeff_absorption);
            }
            if (!isnan(coeff_reflection)) {
                Object::SetCoeffReflection(coeff_reflection);
            }
        };

        virtual ~Circle() {};

        float GetRadius() const {return radius_;};

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const override {
            float res_plus = 0;
            float res_minus = 0;
            if (!GetIntersectionResultQuadraticEquation(start_pos, vec, res_plus, res_minus)) {
                return false;
            }

            if (((res_plus < coeff) || (coeff < 0)) && (res_plus > kEpsilon)) {
                coeff = res_plus;
                if (((res_minus < coeff) || (coeff < 0)) && (res_minus > kEpsilon)) {
                    coeff = res_minus;
                }
                return true;
            }
            if (((res_minus < coeff) || (coeff < 0)) && (res_minus > kEpsilon)) {
                coeff = res_minus;
                return true;
            }

            return false;
        };

        virtual Coordinates GetReflectionVec(const Coordinates& point,
                                             const Coordinates& vec) const override {
            Coordinates radius_vec = !(point - Object::GetCenterCoordinates());
            Coordinates before_ref = !vec;
            return before_ref - radius_vec * (before_ref && radius_vec) * 2;
        };

        virtual Coordinates GetRefractionVec(const Coordinates& point,
                                             const Coordinates& vec,
                                             float old_coeff_refraction,
                                             bool& enter,
                                             bool& stop) const override {
            stop = false;

            float new_coeff_refraction = Object::GetCoeffRefraction();

            Coordinates radius_vec = !(point - Object::GetCenterCoordinates());
            Coordinates before_ref = !vec;
            Coordinates normal_radius = !((radius_vec || before_ref) || radius_vec);

            float old_cos = -(before_ref && radius_vec);
            if (old_cos < 0) {
                float tmp = new_coeff_refraction;
                new_coeff_refraction = old_coeff_refraction;
                old_coeff_refraction = tmp;
                enter = false;
            } else {
                enter = true;
            }

            float new_sin = sqrt(1 - old_cos * old_cos) * old_coeff_refraction / new_coeff_refraction;
            if ((new_sin > 1) || (new_sin < -1)) {
                stop = true;
                return Coordinates(3);
            }

            float new_cos = sqrt(1 - new_sin * new_sin);

            return (old_cos < 0) ? radius_vec * new_cos + normal_radius * new_sin
                                 : radius_vec * (-new_cos) + normal_radius * new_sin;
        };

        virtual Coordinates GetNormal(const Coordinates& point, const Coordinates& vec) const override {
            Coordinates normal = point - Object::GetCenterCoordinates();
            return !(normal * ((!normal) && (!vec)) * (-1));
        };

        virtual Object* GetCopy() const override {
            return new Circle(*this);
        };

        virtual Border* GetBorder() const override {
            return new Border(Object::GetCenterCoordinates(), 2 * radius_, 2 * radius_, 2 * radius_);
        };

    private:
        bool GetIntersectionResultQuadraticEquation(const Coordinates& point,
                                                    const Coordinates& vec,
                                                    float& res_plus, float& res_minus) const {
            Coordinates center(Object::GetCenterCoordinates());
            float distance = ((center - point) || vec).GetModule()
                                / vec.GetModule();
            if (distance > radius_) {
                return false;
            }
            float a = vec.SqLength();
            float b = 2 * (vec && (point - center));
            float c = (point - center).SqLength() - radius_ * radius_;
            float discrim = b * b - 4 * a * c;
            if (discrim < 0) {
                return false;
            }
            discrim = sqrt(discrim);
            res_minus = (-b - discrim) / (2 * a);
            res_plus = (-b + discrim) / (2 * a);

            return true;
        };
};

#endif // CIRCLE_HPP
