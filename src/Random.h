#pragma once


#include "uuid.h"


struct Random {
private:
    Random();

public:
    Random(const Random& Copy) = delete;

    void operator=(const Random& Copy) = delete;


    static Random& Get() {
        static Random Instance;

        return Instance;
    }

    static uuids::uuid Uuid() {
        return Get().UuidRng();
    }

    template<typename T>
    static T Int(T Min, T Max) {
        auto IntDist = std::uniform_int_distribution<T>(Min, Max);

        return IntDist(Get().Rng);
    }

    template<typename T>
    static T Float(T Min, T Max) {
        auto FloatDist = std::uniform_real_distribution<T>(Min, Max);

        return FloatDist(Get().Rng);
    }

    static float Perc() {
        return Float(0.0F, 1.0F);
    }

private:
    std::mt19937 Rng;
    uuids::uuid_random_generator UuidRng;
};
