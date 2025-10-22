#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <map>
#include <limits.h>

#include "widget.hpp"

#include "object_info.hpp"

static const graphics::Color kIBase(19, 19, 19);
static const float kAirCoeffRefraction = 1;
static const float kNormModule = 1;

static const std::map<ObjectType, ObjectInfo> kObjectsInfo = {
    {kSphere, ObjectInfo{kSphere, INFINITY, 0, 1}},
    {kLight,  ObjectInfo{kLight,  INFINITY, 0, 1}},
    {kPlane,  ObjectInfo{kPlane,  INFINITY, 0, 1}},
};

class Object : public ObjectInfo{
    private:
        Coordinates center_;
        graphics::Color color_;

    public:
        explicit Object(const Coordinates& center, const ObjectInfo& info,
                        const graphics::Color& color = kIBase)
            :ObjectInfo(info), center_(center), color_(color) {};

        virtual ~Object() {};

        virtual const Coordinates& GetCenterCoordinates() const {return center_;};
        virtual Coordinates GetColor() const {return Coordinates(color_);};

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
};

#endif // OBJECT_HPP
