#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "plane.hpp"

static const float kBorderLenTrianglesSet = 100;

class Triangle : public Plane {
    private:
        Coordinates point_b_;
        Coordinates point_c_;

    public:
        explicit Triangle(const Plane& plane,
                 const Coordinates& point_b,
                 const Coordinates& point_c)
            :Plane(plane), point_b_(point_b), point_c_(point_c) {};

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
            const Coordinates& point_a = Object::GetCenterCoordinates();
            Coordinates vec_1 = !((point - point_a) || point_b_);
            Coordinates vec_2 = !((point - point_a - point_b_) || (point_c_ - point_b_));
            Coordinates vec_3 = !((point - point_a - point_c_) || (point_c_ * (-1)));

            return ((abs((vec_1 && vec_2) - kNormModule) < kEpsilon)
                    && (abs((vec_1 && vec_3) - kNormModule) < kEpsilon));
        };

        virtual Object* GetCopy() const override {
            return new Triangle(*this);
        };
};

class TrianglesSet : public Object {
    private:
        std::vector<Triangle> triangles_;

    public:
        explicit TrianglesSet(const Coordinates& center, const std::vector<Triangle> triangles,
                              const graphics::Color& color = kIBase,
                              float coeff_reflection = NAN, float coeff_absorption = NAN,
                              float coeff_refraction = NAN)
            :Object(center, kObjectsInfo.at(kTrianglesSet), color), triangles_(triangles) {
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

        void AddTriangle(const Triangle& triangle) {
            triangles_.push_back(Triangle(triangle));
        };

        virtual void Move(const Coordinates& move_direction) {
            size_t triangles_num = triangles_.size();
            for (size_t triangle_idx = 0; triangle_idx < triangles_num; triangle_idx++) {
                triangles_[triangle_idx].Move(move_direction);
            }
        };

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const override {
            bool result = false;

            size_t triangles_num = triangles_.size();
            for (size_t triangle_idx = 0; triangle_idx < triangles_num; triangle_idx++) {
                result = (triangles_[triangle_idx].GetIntersection(start_pos, vec, coeff)) || result;
            }

            return result;
        };

        virtual Coordinates GetNormal(const Coordinates& point, const Coordinates& vec) const override {
            size_t triangles_num = triangles_.size();

            for (size_t triangle_idx = 0; triangle_idx < triangles_num; triangle_idx++) {
                if (triangles_[triangle_idx].IsInhere(point)) {
                    return triangles_[triangle_idx].GetNormal(point, vec);
                }
            }

            return Coordinates(3);
        };

        virtual Coordinates GetReflectionVec(const Coordinates& point,
                                             const Coordinates& vec) const override {
            size_t triangles_num = triangles_.size();

            for (size_t triangle_idx = 0; triangle_idx < triangles_num; triangle_idx++) {
                if (triangles_[triangle_idx].IsInhere(point)) {
                    return triangles_[triangle_idx].GetReflectionVec(point, vec);
                }
            }

            return Coordinates(3);
        };

        virtual Coordinates GetRefractionVec(const Coordinates& point,
                                             const Coordinates& vec,
                                             float old_coeff_refraction,
                                             bool& enter,
                                             bool& stop) const override {
            size_t triangles_num = triangles_.size();

            for (size_t triangle_idx = 0; triangle_idx < triangles_num; triangle_idx++) {
                if (triangles_[triangle_idx].IsInhere(point)) {
                    return triangles_[triangle_idx].GetRefractionVec(point, vec, old_coeff_refraction, enter, stop);
                }
            }

            return Coordinates(3);
        };

        virtual Object* GetCopy() const override {
            return new TrianglesSet(*this);
        };

        virtual Border* GetBorder() const override;

        bool IsInhere(const Coordinates& point) const {
            for (size_t triangle_idx = 0; triangle_idx < triangles_.size(); triangle_idx++) {
                if (triangles_[triangle_idx].IsInhere(point)) {
                    return true;
                }
            }
            return false;
        };
};

#endif // TRIANGLE_HPP
