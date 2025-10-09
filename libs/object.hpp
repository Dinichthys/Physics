#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <map>
#include <limits.h>

#include "widget.hpp"

#include "object_info.hpp"

static const std::map<ObjectType, ObjectInfo> kObjectsInfo = {
    {kSphere, ObjectInfo{kSphere, __FLT_MAX__, 1}},
    {kLight, ObjectInfo{kLight, __FLT_MAX__, 1}},
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
};

class Circle : public Object {
    private:
        float radius_;

    public:
        explicit Circle(const Coordinates& center, float radius, ObjectType type = kSphere,
                        const Coordinates& brightness = Coordinates(3, 0, 0, 0))
            :Object(center, kObjectsInfo.at(type), brightness) {
            radius_ = radius;
        };

        virtual ~Circle() {};

        float GetRadius() const {return radius_;};
};

#endif // OBJECT_HPP
