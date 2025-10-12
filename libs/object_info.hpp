#ifndef OBJECT_INFO_HPP
#define OBJECT_INFO_HPP

#include "vector.hpp"

enum ObjectType {
    kSphere = 0,
    kLight,

    kAllTypes
};

class ObjectInfo {
    private:
        ObjectType type_;
        float coeff_reffraction_;
        float coeff_reflection_;
        float coeff_absorption_;

    public:
        explicit ObjectInfo(ObjectType type, float coeff_reffraction, float coeff_reflection, float coeff_absorption) {
            type_ = type;
            coeff_reffraction_ = coeff_reffraction;
            coeff_reflection_ = coeff_reflection;
            coeff_absorption_ = coeff_absorption;
        };

        virtual ObjectType GetType() const {return type_;};
        virtual float GetCoeffReffraction() const {return coeff_reffraction_;};
        virtual float GetCoeffReflection() const {return coeff_reflection_;};
        virtual float GetCoeffAbsorption() const {return coeff_absorption_;};

        virtual void SetType(ObjectType type) {type_ = type;};
        virtual void SetCoeffReffraction(float coeff_reffraction) {coeff_reffraction_ = coeff_reffraction;};
        virtual void SetCoeffReflection(float coeff_reflection) {coeff_reflection_ = coeff_reflection;};
        virtual void SetCoeffAbsorption(float coeff_absorption) {coeff_absorption_ = coeff_absorption;};
};

#endif // OBJECT_INFO_HPP
