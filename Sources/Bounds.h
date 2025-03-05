#pragma once

struct Bounds
{
    Vector3 center;
    Vector3 extents;
    Vector3 min;
    Vector3 max;
    Vector3 size;

    float biggestExtent; // Radius of encapsulating ball
    float GetBiggestExtentSquared() const { return biggestExtent * biggestExtent; } // Considering getters for Min Max too instead of variable?

    Bounds() = default;
    Bounds(Vector3 center, Vector3 size);

    void UpdateBounds(Vector3 newCenter);
    bool Contains(Vector3 point) const;
    bool Intersects(const Bounds& other) const;
    bool RadiusIntersects(const Bounds& other) const;
    bool IntersectsSphere(const Bounds& sphere) const;
    Vector3 ClosestPoint(Vector3 point) const;
    Vector3 ClosestPointOnBounds(Vector3 point) const;
    Vector3 ClosestSurfaceNormal(Vector3 point) const;
};


