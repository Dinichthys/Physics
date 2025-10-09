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

    public:
        explicit ObjectInfo(ObjectType type, float coeff_reffraction, float coeff_reflection) {
            type_ = type;
            coeff_reffraction_ = coeff_reffraction;
            coeff_reflection_ = coeff_reflection;
        };

        ObjectType GetType() const {return type_;};
        float GetCoeffReffraction() const {return coeff_reffraction_;};
        float GetCoeffReflection() const {return coeff_reflection_;};

        void SetType(ObjectType type) {type_ = type;};
        void SetCoeffReffraction(float coeff_reffraction) {coeff_reffraction_ = coeff_reffraction;};
        void SetCoeffReflection(float coeff_reflection) {coeff_reflection_ = coeff_reflection;};
};

#endif // OBJECT_INFO_HPP
