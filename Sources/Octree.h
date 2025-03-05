#pragma once
#include <array>
#include "Bounds.h"
#include "IRenderContext.h"
#include "MathHelper.h"

template <typename T>
class Octree
{
public:
    Octree() = default;
    Octree(Bounds bounds, int maxDepth, int maxCapacity);
    void AddEntity(const T* entity);
    void OnRender(framework::RenderContextPtr& renderContext);
    std::vector<const T*> QueryEntitiesInBounds(const Bounds& inBounds) const;

private:
    struct Node
    {
        Bounds bounds;
        bool subdivided;

        std::unique_ptr< DirectX::GeometricPrimitive > shape;
        std::array<std::unique_ptr<Node>, 8> children;
        std::vector<const T*> entities;

        Node(const Bounds& bounds, int depth, int maxDepth, int maxCapacity);

        void AddEntity(const T* entity);
        void AddEntityToChildren(const T* entity) const;
        void Subdivide();

        void QueryEntitiesInBounds(const Bounds& inBounds, std::vector<const T*>& results) const;

        bool IsLeaf() const { return !subdivided; }
        bool HasEntities() const { return !entities.empty(); }
        const std::vector<T>& GetEntities() const { return entities; }

        int depth;
        int maxDepth;
        int maxCapacity;

        void OnRender(framework::RenderContextPtr& renderContext);
    };

    int m_maxNodeDepth;
    int m_maxNodeCapacity;
    std::unique_ptr<Node> m_root;
};

template <typename T>
Octree<T>::Octree(Bounds bounds, int maxDepth, int maxCapacity)
    : m_maxNodeDepth(maxDepth)
    , m_maxNodeCapacity(maxCapacity)
{
    m_root = std::make_unique<Node>(bounds, 0, maxDepth, maxCapacity);
}

template <typename T>
void Octree<T>::AddEntity(const T* entity)
{
    m_root->AddEntity(entity);
}

template <typename T>
void Octree<T>::OnRender(framework::RenderContextPtr& renderContext)
{
    m_root->OnRender(renderContext);
}

template <typename T>
std::vector<const T*> Octree<T>::QueryEntitiesInBounds(const Bounds& inBounds) const
{
    std::vector<const T*> results;
    m_root->QueryEntitiesInBounds(inBounds, results);
    return results;
}

template <typename T>
void Octree<T>::Node::QueryEntitiesInBounds(const Bounds& inBounds, std::vector<const T*>& results) const
{
    if (!bounds.Intersects(inBounds))
    {
        return;
    }

    if(IsLeaf())
    {
        for (const T* entity : entities)
        {
            results.push_back(entity);
        }
    }
    else
    {
        for (const std::unique_ptr<Node>& node : children)
        {
            node->QueryEntitiesInBounds(inBounds, results);
        }
    }
}

template <typename T>
Octree<T>::Node::Node(const Bounds& bounds, int depth, int maxDepth, int maxCapacity)
    : bounds(bounds)
    , subdivided(false)
    , depth(depth)
    , maxDepth(maxDepth)
    , maxCapacity(maxCapacity)
{
    shape.reset();
    shape = GetEngine().CreateBoxPrimitive(bounds.size);
}

template <typename T>
void Octree<T>::Node::AddEntity(const T* entity)
{
    if(entities.size() > maxCapacity && depth < maxDepth && !subdivided)
    {
        Subdivide();
    }

    if (IsLeaf())
    {
        entities.push_back(entity);
    }
    else
    {
        AddEntityToChildren(entity);
    }
}

template <typename T>
void Octree<T>::Node::AddEntityToChildren(const T* entity) const
{
    for (const std::unique_ptr<Node>& node : children)
    {
        if (node->bounds.Intersects(entity->bounds))
        {
            node->AddEntity(entity);
        }
    }
}

template <typename T>
void Octree<T>::Node::Subdivide()
{
    subdivided = true;
    const int childDepth = depth + 1;

    const Vector3 centerOffset = bounds.size * 0.25f; // quarter offset

    for (int i = 0; i < 8; i++) {
        Vector3 childCenter = bounds.center;
        childCenter.x += centerOffset.x * ((i & 1) == 0 ? -1.0f : 1.0f);
        childCenter.y += centerOffset.y * ((i & 2) == 0 ? -1.0f : 1.0f);
        childCenter.z += centerOffset.z * ((i & 4) == 0 ? -1.0f : 1.0f);
        children[i] = std::make_unique<Node>(Bounds(childCenter, bounds.extents), childDepth, maxDepth, maxCapacity);
    }

    for (const T* entity : entities)
    {
        AddEntityToChildren(entity);
    }

    entities.clear();
}

template <typename T>
void Octree<T>::Node::OnRender(framework::RenderContextPtr& renderContext)
{
    const float intensity = static_cast<float>(depth) / static_cast<float>(maxDepth);
    const float intensityClamped = MathHelper::GetProportional(0.0f, 1.0f, intensity, 0.2f, 0.7f);
    XMVECTORF32 bounds_color = { HasEntities() , 0.0f, !HasEntities(), intensityClamped };
    renderContext->RenderPrimitive(shape, Vector3::One, bounds.center, Vector3::Zero, bounds_color);

    if(!IsLeaf())
    {
        for (const std::unique_ptr<Node>& node : children)
        {
            node->OnRender(renderContext);
        }
    }
}