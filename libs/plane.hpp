#ifndef PLANE_HPP
#define PLANE_HPP

#include "object.hpp"

class Plane : public Object {
    private:
        Coordinates normal_;

    public:
        explicit Plane(const Coordinates& center, const Coordinates& normal,
                        const colors::Color& color = kIBase,
                        float coeff_reflection = NAN, float coeff_absorption = NAN,
                        float coeff_refraction = NAN)
            :Object(center, kObjectsInfo.at(kPlane), color), normal_(!normal) {
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

        virtual ~Plane() {};

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const override {
            float n_vec_scalar = normal_ && vec;
            if ((-kEpsilon < n_vec_scalar) && (n_vec_scalar < kEpsilon)) {
                return false;
            }
            float d = -(normal_ && Object::GetCenterCoordinates());
            float tmp = -((normal_ && start_pos) + d) / n_vec_scalar;
            if ((tmp > kEpsilon) && ((tmp < coeff) || (coeff < 0))) {
                coeff = tmp;
                return true;
            }
            return false;
        };

        virtual Coordinates GetReflectionVec(__attribute_maybe_unused__ const Coordinates& point,
                                             const Coordinates& vec) const override {
            Coordinates before_ref = !vec;
            return before_ref - (!normal_) * (before_ref && (!normal_)) * 2;
        };

        virtual Coordinates GetRefractionVec(__attribute_maybe_unused__ const Coordinates& point,
                                             const Coordinates& vec,
                                             float old_coeff_refraction,
                                             bool& enter,
                                             bool& stop) const override {
            stop = false;

            float new_coeff_refraction = Object::GetCoeffRefraction();

            Coordinates before_ref = !vec;
            Coordinates normal_radius = !((normal_ || before_ref) || normal_);

            float old_cos = -(before_ref && normal_);
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

            return (old_cos < 0) ? ((!normal_) * new_cos) + (normal_radius * new_sin)
                                 : ((!normal_) * (-new_cos)) + (normal_radius * new_sin);
        };

        virtual Coordinates GetNormal(__attribute_maybe_unused__ const Coordinates& point,
                                      __attribute_maybe_unused__ const Coordinates& vec) const override {
            return !(normal_ * ((!normal_) && (!vec)) * (-1));
        };

        virtual Object* GetCopy() const override {
            return new Plane(*this);
        };

        virtual Border* GetBorder() const override;

        virtual void OnSelect(InfoTable*) const override;
};

#endif // PLANE_HPP
