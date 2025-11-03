#ifndef BORDER_HPP
#define BORDER_HPP

#include "graphics.hpp"

#include "object.hpp"
#include "triangle.hpp"

static const graphics::Color kBorderColor = graphics::kColorRed;
static const float kBorderWidth = 10;

class Square : public TrianglesSet {
    public:
        explicit Square(const Coordinates& lt_corner, const Coordinates& rb_corner, const Coordinates& side_vec)
            :TrianglesSet(lt_corner + (rb_corner - lt_corner) / 2, std::vector<Triangle>(), kBorderColor) {
            Coordinates side_vec_ = (!side_vec) * ((rb_corner - lt_corner) && (!side_vec));
            Coordinates addition = rb_corner - lt_corner - side_vec_;
            Coordinates normal = addition || side_vec_;
            Triangle first(Plane(lt_corner, normal, kBorderColor), side_vec_, rb_corner - lt_corner);
            Triangle second(Plane(lt_corner, normal, kBorderColor), addition, rb_corner - lt_corner);
            TrianglesSet::AddTriangle(first);
            TrianglesSet::AddTriangle(second);
        };

        virtual Object* GetCopy() const override {
            return new Square(*this);
        };
};

class Parallelepiped : public Object {
    private:
        std::vector<Square> sides_;

    public:
        explicit Parallelepiped(const Coordinates& lt_corner, const Coordinates& rb_corner)
            :Object((lt_corner + rb_corner) / 2, kObjectsInfo.at(kBorder), kBorderColor){
            Coordinates diagonal = rb_corner - lt_corner;
            Coordinates z_vec = Coordinates(3, 0, 0, diagonal[2]);
            Coordinates x_vec = Coordinates(3, diagonal[0], 0, 0);
            Coordinates y_vec = Coordinates(3, 0, diagonal[1], 0);
            sides_.push_back(Square(lt_corner, lt_corner + x_vec + y_vec, x_vec));
            sides_.push_back(Square(lt_corner, lt_corner + z_vec + y_vec, y_vec));
            sides_.push_back(Square(lt_corner, lt_corner + x_vec + z_vec, x_vec));
            sides_.push_back(Square(lt_corner + x_vec + y_vec, rb_corner, y_vec));
            sides_.push_back(Square(lt_corner + z_vec + y_vec, rb_corner, y_vec));
            sides_.push_back(Square(lt_corner + x_vec + z_vec, rb_corner, x_vec));
        };

        virtual void Move(const Coordinates& move_direction) {
            size_t sides_num = sides_.size();
            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                sides_[side_idx].Move(move_direction);
            }
        };

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const override {
            bool result = false;

            size_t sides_num = sides_.size();
            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                result = (sides_[side_idx].GetIntersection(start_pos, vec, coeff)) || result;
            }

            return result;
        };

        virtual Coordinates GetNormal(const Coordinates& point, const Coordinates& vec) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetNormal(point, vec);
                }
            }

            return Coordinates(3);
        };

        virtual Coordinates GetReflectionVec(const Coordinates& point,
                                             const Coordinates& vec) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetReflectionVec(point, vec);
                }
            }

            return Coordinates(3);
        };

        virtual Coordinates GetRefractionVec(const Coordinates& point,
                                             const Coordinates& vec,
                                             float old_coeff_refraction,
                                             bool& enter,
                                             bool& stop) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetRefractionVec(point, vec, old_coeff_refraction, enter, stop);
                }
            }

            return Coordinates(3);
        };

        bool IsInhere(const Coordinates& point) const {
            for (size_t side_idx = 0; side_idx < sides_.size(); side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return true;
                }
            }
            return false;
        };

        virtual Object* GetCopy() const override {
            return new Parallelepiped(*this);
        };

        virtual Border* GetBorder() const override {
            return NULL;
        };
};

class Edge : public Object {
    private:
        std::vector<Square> sides_;

    public:
        explicit Edge(const Coordinates& l_corner, const Coordinates& r_corner,
                      const Coordinates& first_side_vec, const Coordinates& second_side_vec)
            :Object((l_corner + r_corner) / 2, kObjectsInfo.at(kBorder), kBorderColor){
            Coordinates diagonal = r_corner - l_corner;
            sides_.push_back(Square(l_corner, r_corner + first_side_vec, diagonal));
            sides_.push_back(Square(l_corner, r_corner + second_side_vec, diagonal));
        };

        virtual void Move(const Coordinates& move_direction) {
            size_t sides_num = sides_.size();
            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                sides_[side_idx].Move(move_direction);
            }
        };

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const override {
            bool result = false;

            size_t sides_num = sides_.size();
            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                result = (sides_[side_idx].GetIntersection(start_pos, vec, coeff)) || result;
            }

            return result;
        };

        virtual Coordinates GetNormal(const Coordinates& point, const Coordinates& vec) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetNormal(point, vec);
                }
            }

            return Coordinates(3);
        };

        virtual Coordinates GetReflectionVec(const Coordinates& point,
                                             const Coordinates& vec) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetReflectionVec(point, vec);
                }
            }

            return Coordinates(3);
        };

        virtual Coordinates GetRefractionVec(const Coordinates& point,
                                             const Coordinates& vec,
                                             float old_coeff_refraction,
                                             bool& enter,
                                             bool& stop) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetRefractionVec(point, vec, old_coeff_refraction, enter, stop);
                }
            }

            return Coordinates(3);
        };

        bool IsInhere(const Coordinates& point) const {
            for (size_t side_idx = 0; side_idx < sides_.size(); side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return true;
                }
            }
            return false;
        };

        virtual Object* GetCopy() const override {
            return new Edge(*this);
        };

        virtual Border* GetBorder() const override {
            return NULL;
        };
};

class Border : public Object {
    private:
        std::vector<Edge> sides_;

        float width_;
        float height_;
        float length_;

    public:
        explicit Border(const Coordinates& center, float width, float height, float length)
            :Object(center, kObjectsInfo.at(kBorder), kBorderColor) {
            width_ = width;
            height_ = height;
            length_ = length;

            Coordinates x_vec(3, width / 2, 0, 0);
            Coordinates x_shift(3, kBorderWidth / 2, 0, 0);
            Coordinates y_vec(3, 0, height / 2, 0);
            Coordinates y_shift(3, 0, kBorderWidth / 2, 0);
            Coordinates z_vec(3, 0, 0, length / 2);
            Coordinates z_shift(3, 0, 0, kBorderWidth / 2);

            sides_.push_back(Edge(center + x_vec + y_vec + z_vec + x_shift + y_shift + z_shift,
                                  center - x_vec + y_vec + z_vec - x_shift + y_shift + z_shift,
                                  y_shift * (-2), z_shift * (-2)));
            sides_.push_back(Edge(center + x_vec + y_vec + z_vec + x_shift + y_shift + z_shift,
                                  center + x_vec - y_vec + z_vec + x_shift - y_shift + z_shift,
                                  x_shift * (-2), z_shift * (-2)));
            sides_.push_back(Edge(center + x_vec + y_vec + z_vec + x_shift + y_shift + z_shift,
                                  center + x_vec + y_vec - z_vec + x_shift + y_shift - z_shift,
                                  x_shift * (-2), y_shift * (-2)));

            sides_.push_back(Edge(center - x_vec - y_vec + z_vec - x_shift - y_shift + z_shift,
                                  center + x_vec - y_vec + z_vec + x_shift - y_shift + z_shift,
                                  y_shift * 2, z_shift * (-2)));
            sides_.push_back(Edge(center - x_vec - y_vec + z_vec - x_shift - y_shift + z_shift,
                                  center - x_vec + y_vec + z_vec - x_shift + y_shift + z_shift,
                                  x_shift * 2, z_shift * (-2)));
            sides_.push_back(Edge(center - x_vec - y_vec + z_vec - x_shift - y_shift + z_shift,
                                  center - x_vec - y_vec - z_vec - x_shift - y_shift - z_shift,
                                  x_shift * 2, y_shift * 2));

            sides_.push_back(Edge(center + x_vec - y_vec - z_vec + x_shift - y_shift - z_shift,
                                  center - x_vec - y_vec - z_vec - x_shift - y_shift - z_shift,
                                  y_shift * 2, z_shift * 2));
            sides_.push_back(Edge(center + x_vec - y_vec - z_vec + x_shift - y_shift - z_shift,
                                  center + x_vec + y_vec - z_vec + x_shift + y_shift - z_shift,
                                  x_shift * (-2), z_shift * 2));
            sides_.push_back(Edge(center + x_vec - y_vec - z_vec + x_shift - y_shift - z_shift,
                                  center + x_vec - y_vec + z_vec + x_shift - y_shift + z_shift,
                                  x_shift * (-2), y_shift * 2));

            sides_.push_back(Edge(center - x_vec + y_vec - z_vec - x_shift + y_shift - z_shift,
                                  center + x_vec + y_vec - z_vec + x_shift + y_shift - z_shift,
                                  y_shift * (-2), z_shift * 2));
            sides_.push_back(Edge(center - x_vec + y_vec - z_vec - x_shift + y_shift - z_shift,
                                  center - x_vec - y_vec - z_vec - x_shift - y_shift - z_shift,
                                  x_shift * 2, z_shift * 2));
            sides_.push_back(Edge(center - x_vec + y_vec - z_vec - x_shift + y_shift - z_shift,
                                  center - x_vec + y_vec + z_vec - x_shift + y_shift + z_shift,
                                  x_shift * 2, y_shift * (-2)));
        };

        virtual void Move(const Coordinates& move_direction) {
            size_t sides_num = sides_.size();
            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                sides_[side_idx].Move(move_direction);
            }
        };

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const override {
            bool result = false;

            size_t sides_num = sides_.size();
            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                result = (sides_[side_idx].GetIntersection(start_pos, vec, coeff)) || result;
            }

            return result;
        };

        virtual Coordinates GetNormal(const Coordinates& point, const Coordinates& vec) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetNormal(point, vec);
                }
            }

            return Coordinates(3);
        };

        virtual Coordinates GetReflectionVec(const Coordinates& point,
                                             const Coordinates& vec) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetReflectionVec(point, vec);
                }
            }

            return Coordinates(3);
        };

        virtual Coordinates GetRefractionVec(const Coordinates& point,
                                             const Coordinates& vec,
                                             float old_coeff_refraction,
                                             bool& enter,
                                             bool& stop) const override {
            size_t sides_num = sides_.size();

            for (size_t side_idx = 0; side_idx < sides_num; side_idx++) {
                if (sides_[side_idx].IsInhere(point)) {
                    return sides_[side_idx].GetRefractionVec(point, vec, old_coeff_refraction, enter, stop);
                }
            }

            return Coordinates(3);
        };

        virtual Object* GetCopy() const override {
            return new Border(*this);
        };

        virtual Border* GetBorder() const override {
            return new Border(*this);
        };
};

#endif // BORDER_HPP
