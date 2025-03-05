#include "pch.h"
#include "MathHelper.h"
#include <random>

namespace MathHelper
{
    float RandomValue()
    {
        return RandomFromRange(0.0f, 1.0f);
    }

    float RandomBinomial()
    {
        return RandomValue() - RandomValue();
    }

    Vector3 RandomDirection()
    {
        Vector3 random_vector = Vector3(RandomBinomial(), RandomBinomial(), RandomBinomial());
        random_vector.Normalize();
        return random_vector;
    }

    bool IsWithinRange(float value, float rangeMin, float rangeMax)
    {
        return value > rangeMin && value < rangeMax;
    }

    float DegreesToRadians(float degrees)
    {
        return degrees * static_cast<float>(M_PI) / 180.0f;
    }

    Vector3 GetNormalized(const Vector3& vector)
    {
        Vector3 normalized_vector;
        vector.Normalize(normalized_vector);
        return normalized_vector;
    }

    float GetProportional(float minOld, float maxOld, float value, float minNew, float maxNew)
    {
        assert(minOld != maxOld);
        const float factor = (value - minOld) / (maxOld - minOld);
        return minNew + (maxNew - minNew) * factor;
    }

    float GetBiggest(const Vector3& vector)
    {
        return std::max(std::max(vector.x, vector.y), vector.z);
    }
};


Vector3Int::Vector3Int()
    : Vector3Int(0,0,0)
{
}

Vector3Int::Vector3Int(int x, int y, int z)
    : x(x), y(y), z(z)
{

}