#pragma once
#include <random>

namespace MathHelper
{
    float RandomValue();
    float RandomBinomial();

    template<typename T>
    T RandomFromRange(T min, T max);
    Vector3 RandomDirection();

    bool IsWithinRange(float value, float rangeMin, float rangeMax);
    float DegreesToRadians(float degrees);
    Vector3 GetNormalized(const Vector3& vector);

    float GetProportional(float minOld, float maxOld, float value, float minNew, float maxNew);
    float GetBiggest(const Vector3& vector);


    template <typename T>
    T RandomFromRange(T min, T max)
    {
        static_assert(std::is_arithmetic_v<T>, "Template parameter must be an arithmetic type.");

        std::random_device rd;
        std::mt19937 gen(rd());

        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<T> dis(min, max);
            return dis(gen);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            std::uniform_real_distribution<T> dis(min, max);
            return dis(gen);
        }
        else
        {
            static_assert("RandomFromRange Unsupported type used");
        }
}
};

struct Vector3Int
{
    Vector3Int();
    Vector3Int(int x, int y, int z);
    int x, y, z;

    bool operator==(const Vector3Int& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
};


