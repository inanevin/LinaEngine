#include "Animation/Skeleton.hpp"

#include "Log/Log.hpp"

namespace Lina::Graphics
{
    Skeleton::~Skeleton()
    {
    }

    bool Skeleton::LoadSkeleton(const String& skelPath)
    {
        return false;
    }
    void Skeleton::SetVertexBoneDataToDefault(VertexBoneData& vertex)
    {
    }

    void Skeleton::SetVertexBoneData(VertexBoneData& vertex, int boneID, float weight)
    {
    }
} // namespace Lina::Graphics
