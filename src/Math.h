#pragma once

#include <valarray>
#include "HashUtil.h"


#define PI                  3.141592F
#define DEG2RAD             (PI / 180.0F)
#define RAD2DEG             (180.0F / PI)


#define CLAMP(value, min, max)           (value >= min ? (value <= max ? value : max) : min)


template<typename T>
inline bool FracEqual(T A, T B) {
    return std::abs(A - B) < std::numeric_limits<T>::epsilon();
}

struct Point {
    Point(int32_t X_, int32_t Y_, int32_t Z_) : X{X_}, Y{Y_}, Z{Z_} {

    }

    Point() : Point(0, 0, 0) {

    }

    Point operator*(float Scalar) const {
        return Point{(int) (X * Scalar), (int) (Y * Scalar), (int) (Z * Scalar)};
    }

    Point operator/(float Scalar) const {
        return *this * (1 / Scalar);
    }

    Point operator-() const {
        return *this * -1.0F;
    }

    Point operator+(const Point& Other) const {
        Point Coord{X + Other.X, Y + Other.Y, Z + Other.Z};

        return Coord;
    }

    Point operator-(const Point& Other) const {
        return *this + (-Other);
    }

    Point& operator+=(const Point& Other) {
        *this = *this + Other;

        return *this;
    }

    bool operator==(const Point& Other) const {
        return this->X == Other.X && this->Y == Other.Y && this->Z == Other.Z;
    }

    int32_t X;
    int32_t Y;
    int32_t Z;
};

struct Vec3 {
    Vec3(float X_, float Y_, float Z_) : X{X_}, Y{Y_}, Z{Z_} {

    }

    Vec3() : Vec3(0.0F, 0.0F, 0.0F) {

    }

    Vec3 operator*(float Scalar) const {
        return Vec3{X * Scalar, Y * Scalar, Z * Scalar};
    }

    Vec3 operator/(float Scalar) const {
        return *this * (1 / Scalar);
    }

    Vec3 operator-() const {
        return *this * -1.0F;
    }

    Vec3 operator+(const Vec3& Other) const {
        Vec3 Coord{X + Other.X, Y + Other.Y, Z + Other.Z};

        return Coord;
    }

    Vec3 operator-(const Vec3& Other) const {
        return *this + (-Other);
    }

    Vec3& operator+=(const Vec3& Other) {
        *this = *this + Other;

        return *this;
    }

    bool operator==(const Vec3& Other) const {
        return FracEqual(X, Other.X) && FracEqual(Y, Other.Y) && FracEqual(Z, Other.Z);
    }

    float Dot(const Vec3& Other) const // The length of A and the length of B and the cosine between A and B
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }

    float LengthSq() const {
        return Dot(*this);
    }

    float Length() const {
        return sqrtf(LengthSq());
    }

    Vec3 Normalized() const {
        return *this / Length();
    }

    Vec3 Displacement(const Vec3& Other) const {
        return Other - *this;
    }

    float DistanceSq(const Vec3& Other) const {
        return Displacement(Other).LengthSq();
    }

    float Distance(const Vec3& Other) const {
        return sqrtf(DistanceSq(Other));
    }

    Vec3 Direction(const Vec3& Other) const {
        return Displacement(Other).Normalized();
    }

    float AngleBetween(const Vec3& Other) const {
        return acosf(Dot(Other)) * RAD2DEG;
    }

    float X;
    float Y;
    float Z;
};

namespace std {
    template<>
    struct hash<Vec3> {
        inline size_t operator()(const Vec3& Vec) const {
            size_t value = 0;
            hash_combine(value, Vec.X, Vec.Y);

            return value;
        }
    };

    template<>
    struct hash<Point> {
        inline size_t operator()(const Point& ThePoint) const {
            size_t value = 0;
            hash_combine(value, ThePoint.X, ThePoint.Y);

            return value;
        }
    };
}
