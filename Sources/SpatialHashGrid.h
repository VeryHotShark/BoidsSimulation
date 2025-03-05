#pragma once
#include <unordered_map>
#include "MathHelper.h"

template<typename T>
class SpatialHashGrid
{
public:
    SpatialHashGrid(float cellSize);

    void AddEntity(T* entity);
    void RemoveEntity(T* entity);
    void UpdateEntity(T* entity);
    void Clear();

    std::vector<T*> QueryInRadius(Vector3 position, float radius) const;

private:
    struct CellKeyHasher
    {
        size_t operator()(const Vector3Int& key) const
        {
            return std::hash<int>()(key.x) * 73856093 ^ std::hash<int>()(key.y) * 19349663 ^ std::hash<int>()(key.z) * 83492791;
        }
    };

    float m_cellSize;
    std::unordered_map<Vector3Int, std::vector<T*>, CellKeyHasher> m_cells;

    Vector3Int GetCellIndex(Vector3 position) const;
};

template <typename T>
SpatialHashGrid<T>::SpatialHashGrid(float cellSize)
    : m_cellSize(cellSize)
{
}

template <typename T>
void SpatialHashGrid<T>::AddEntity(T* entity)
{
    const Vector3Int cellIndex = GetCellIndex(entity->GetPosition());
    entity->SetCellIndex(cellIndex);
    m_cells[cellIndex].push_back(entity);
}

template <typename T>
void SpatialHashGrid<T>::RemoveEntity(T* entity)
{
    auto it = m_cells.find(entity->GetCellIndex());
    if (it != m_cells.end())
    {
        std::vector<T*>& cellEntities = it->second;
        cellEntities.erase(std::remove(cellEntities.begin(), cellEntities.end(), entity), cellEntities.end());
    }
}

template <typename T>
void SpatialHashGrid<T>::UpdateEntity(T* entity)
{
    const Vector3Int currentIndex = GetCellIndex(entity->GetPosition());

    if (currentIndex == entity->GetCellIndex())
    {
        return;
    }

    RemoveEntity(entity);
    AddEntity(entity);
}

template <typename T>
void SpatialHashGrid<T>::Clear()
{
    m_cells.clear();
}

template <typename T>
std::vector<T*> SpatialHashGrid<T>::QueryInRadius(Vector3 position, float radius) const
{
    std::vector<T*> result;

    const Vector3Int minCellIndex = GetCellIndex({ position - Vector3::One * radius });
    const Vector3Int maxCellIndex = GetCellIndex({ position + Vector3::One * radius });

    const int rangeX = maxCellIndex.x - minCellIndex.x + 1;
    const int rangeY = maxCellIndex.y - minCellIndex.y + 1;
    const int rangeZ = maxCellIndex.z - minCellIndex.z + 1;

    /*
    for (int x = minCellIndex.x; x <= maxCellIndex.x; ++x)
        for (int y = minCellIndex.y; y <= maxCellIndex.y; ++y)
            for (int z = minCellIndex.z; z <= maxCellIndex.z; ++z)

    */
    // Flattened loop
    for (int i = 0; i < rangeX * rangeY * rangeZ; ++i) {
        int x = minCellIndex.x + (i % rangeX);
        int y = minCellIndex.y + ((i / rangeX) % rangeY);
        int z = minCellIndex.z + (i / (rangeX * rangeY));

        auto it = m_cells.find({ x, y, z });
        if (it != m_cells.end())
        {
            for (T* entity : it->second) {
                const float distanceSquared = (entity->GetPosition() - position).LengthSquared();

                if (distanceSquared < radius * radius)
                {
                    result.push_back(entity);
                }
            }
        }
    }

    return result;
}

template <typename T>
Vector3Int SpatialHashGrid<T>::GetCellIndex(Vector3 position) const
{
    return Vector3Int{
        static_cast<int>(std::floor(position.x / m_cellSize)),
        static_cast<int>(std::floor(position.y / m_cellSize)),
        static_cast<int>(std::floor(position.z / m_cellSize))
    };
}
