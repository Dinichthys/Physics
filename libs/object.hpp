#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <map>
#include <limits.h>

#include "widget.hpp"

#include "object_info.hpp"

static const std::map<ObjectType, ObjectInfo> kObjectsInfo = {
    {kSphere, ObjectInfo{kSphere, 0, 0, 1}},
    {kLight,  ObjectInfo{kLight,  0, 0, 1}},
};

class Object : public ObjectInfo{
    private:
        Coordinates center_;
        Coordinates brightness_;

    public:
        explicit Object(const Coordinates& center, const ObjectInfo& info,
                        const Coordinates& brightness = Coordinates(3, 0, 0, 0))
            :ObjectInfo(info), center_(center), brightness_(brightness) {};

        virtual const Coordinates& GetCenterCoordinates() const {return center_;};
        virtual Coordinates GetBrightness() const {return Coordinates(brightness_);};
};

class Circle : public Object {
    private:
        float radius_;

    public:
        explicit Circle(const Coordinates& center, float radius, ObjectType type = kSphere,
                        const Coordinates& brightness = Coordinates(3, 0, 0, 0),
                        float coeff_reflection = NAN, float coeff_absorption = NAN,
                        float coeff_reffraction = NAN)
            :Object(center, kObjectsInfo.at(type), brightness) {
            radius_ = radius;
            if (!isnan(coeff_reffraction)) {
                Object::SetCoeffReffraction(coeff_reffraction);
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
};

#endif // OBJECT_HPP
