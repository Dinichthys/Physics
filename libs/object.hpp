#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <map>
#include <limits.h>

#include "widget.hpp"

#include "object_info.hpp"

static const colors::Color kIBase(19, 19, 19);
static const float kAirCoeffRefraction = 1;
static const float kNormModule = 1;

static const std::map<ObjectType, ObjectInfo> kObjectsInfo = {
    {kSphere,       ObjectInfo{kSphere,       INFINITY, 0, 1}},
    {kLight,        ObjectInfo{kLight,        INFINITY, 0, 1}},
    {kPlane,        ObjectInfo{kPlane,        INFINITY, 0, 1}},
    {kTrianglesSet, ObjectInfo{kTrianglesSet, INFINITY, 0, 1}},
    {kBorder,       ObjectInfo{kBorder,       INFINITY, 0, 1}},
};

class Border;
class InfoTable;

class Object : public ObjectInfo{
    private:
        Coordinates center_;
        colors::Color color_;

    public:
        explicit Object(const Coordinates& center, const ObjectInfo& info,
                        const colors::Color& color = kIBase)
            :ObjectInfo(info), center_(center), color_(color) {};

        virtual ~Object() {};

        virtual const Coordinates& GetCenterCoordinates() const {return center_;};
        virtual void SetCenterCoordinates(const Coordinates& center) {center_ = center;};

        virtual Coordinates GetColor() const {return Coordinates(3, color_.r, color_.g, color_.b);};
        virtual void SetColor(colors::Color color) {color_ = color;};

        virtual void Move(const Coordinates& move_direction) {
            center_ = center_ + move_direction;
        };

        virtual bool GetIntersection(const Coordinates& start_pos,
                                     const Coordinates& vec,
                                     float& coeff) const = 0;

        virtual Coordinates GetReflectionVec(const Coordinates& point,
                                             const Coordinates& vec) const = 0;

        virtual Coordinates GetRefractionVec(const Coordinates& point,
                                             const Coordinates& vec,
                                             float old_coeff_refraction,
                                             bool& enter,
                                             bool& stop) const = 0;

        virtual Coordinates GetNormal(const Coordinates& point, const Coordinates& vec) const = 0;

        virtual Object* GetCopy() const = 0;

        virtual Border* GetBorder() const = 0;

        virtual void OnSelect(InfoTable*) const {};
};

#endif // OBJECT_HPP
