#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <map>
#include <limits.h>

#include "widget.hpp"

#include "object_info.hpp"

static const std::map<ObjectType, ObjectInfo> kObjectsInfo = {
    {kSphere, ObjectInfo{kSphere, __FLT_MAX__, 0}},
    {kLight, ObjectInfo{kLight, __FLT_MAX__, 0}},
};

class Object {
    private:
        ObjectInfo info_;
        Coordinates center_;
        Coordinates brightness_;

    public:
        explicit Object(const Coordinates& center, const ObjectInfo& info,
                        const Coordinates& brightness = Coordinates(3, 0, 0, 0))
            :info_(info), center_(center), brightness_(brightness) {};

        virtual const Coordinates& GetCenterCoordinates() const {return center_;};
        virtual const ObjectInfo& GetObjectInfo() const {return info_;};
        virtual ObjectType GetObjectType() const {return info_.GetType();};
        virtual float GetReffraction() const {return info_.GetCoeffReffraction();};
        virtual float GetReflection() const {return info_.GetCoeffReflection();};
        virtual Coordinates GetBrightness() const {return Coordinates(brightness_);};

        virtual void SetType(ObjectType type) {info_.SetType(type);};
        virtual void SetCoeffReffraction(float coeff_reffraction) {info_.SetCoeffReffraction(coeff_reffraction);};
        virtual void SetCoeffReflection(float coeff_reflection) {info_.SetCoeffReflection(coeff_reflection);};
};

class Circle : public Object {
    private:
        float radius_;

    public:
        explicit Circle(const Coordinates& center, float radius, ObjectType type = kSphere,
                        const Coordinates& brightness = Coordinates(3, 0, 0, 0),
                        float coeff_reflection = NAN, float coeff_reffraction = NAN)
            :Object(center, kObjectsInfo.at(type), brightness) {
            radius_ = radius;
            if (!isnan(coeff_reffraction)) {
                Object::SetCoeffReffraction(coeff_reffraction);
            }
            if (!isnan(coeff_reflection)) {
                Object::SetCoeffReflection(coeff_reflection);
            }
        };

        virtual ~Circle() {};

        float GetRadius() const {return radius_;};
};

#endif // OBJECT_HPP
