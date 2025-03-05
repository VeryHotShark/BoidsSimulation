#include "pch.h"
#include "Bounds.h"

#include "MathHelper.h"

Bounds::Bounds(Vector3 center, Vector3 size)
    :center(center)
    , size(size)
{
    extents = size * 0.5f;
    min = center - extents;
    max = center + extents;
    biggestExtent = MathHelper::GetBiggest(extents);
}

bool Bounds::Contains(Vector3 point) const
{
    return (point.x >= min.x && point.x <= max.x) &&
        (point.y >= min.y && point.y <= max.y) &&
        (point.z >= min.z && point.z <= max.z);
}

bool Bounds::Intersects(const Bounds& other) const
{
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y) &&
        (min.z <= other.max.z && max.z >= other.min.z);
}

bool Bounds::RadiusIntersects(const Bounds& other) const
{
    return Vector3::DistanceSquared(center, other.center) <= GetBiggestExtentSquared() + other.GetBiggestExtentSquared();
}

bool Bounds::IntersectsSphere(const Bounds& sphere) const
{
    const Vector3 closestPoint = ClosestPoint(sphere.center);
    const float distanceSquared = Vector3::DistanceSquared(sphere.center, closestPoint);
    return distanceSquared < sphere.biggestExtent * sphere.biggestExtent;
}

Vector3 Bounds::ClosestPoint(Vector3 point) const
{
    Vector3 closestPoint;
    point.Clamp(min, max, closestPoint);
    return closestPoint;
}

Vector3 Bounds::ClosestPointOnBounds(Vector3 point) const
{
    if(!Contains(point))
    {
        return ClosestPoint(point);
    }

    const float distanceXMax = max.x - point.x;
    const float distanceXMin = point.x - min.x;

    const float distanceYMax = max.y - point.y;
    const float distanceYMin = point.y - min.y;

    const float distanceZMax = max.z - point.z;
    const float distanceZMin = point.z - min.z;

    float minDistance = std::numeric_limits<float>::max();
    Vector3 surfacePoint = point;

    if (distanceXMin < minDistance)
    {
        minDistance = distanceXMin;
        surfacePoint = point;
        surfacePoint.x = min.x;
    }
    if (distanceXMax < minDistance)
    {
        minDistance = distanceXMax;
        surfacePoint = point;
        surfacePoint.x = max.x;
    }
    if (distanceYMin < minDistance)
    {
        minDistance = distanceYMin;
        surfacePoint = point;
        surfacePoint.y = min.y;
    }
    if (distanceYMax < minDistance)
    {
        minDistance = distanceYMax;
        surfacePoint = point;
        surfacePoint.y = max.y;
    }
    if (distanceZMin < minDistance)
    {
        minDistance = distanceZMin;
        surfacePoint = point;
        surfacePoint.z = min.z;
    }
    if (distanceZMax < minDistance)
    {
        surfacePoint = point;
        surfacePoint.z = max.z;
    }

    return surfacePoint;
}

Vector3 Bounds::ClosestSurfaceNormal(Vector3 point) const
{
    const Vector3 closestPoint = ClosestPointOnBounds(point);
    Vector3 normal = Vector3::Zero;

    if (closestPoint.x == min.x)
    {
        normal = Vector3::Left;
    }
    else if (closestPoint.x == max.x)
    {
        normal = Vector3::Right;
    }

    if (closestPoint.y == min.y)
    {
        normal += Vector3::Down;
    }
    else if (closestPoint.y == max.y)
    {
        normal += Vector3::Up;
    }

    if (closestPoint.z == min.z)
    {
        normal += Vector3::Forward;
    }
    else if (closestPoint.z == max.z)
    {
        normal += Vector3::Backward;
    }

    return MathHelper::GetNormalized(normal);
}

void Bounds::UpdateBounds(Vector3 newCenter)
{
    center = newCenter;
    min = center - extents;
    max = center + extents;
}