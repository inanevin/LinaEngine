/*
Class: Skeleton



Timestamp: 12/7/2021 2:13:56 PM
*/

#pragma once

#ifndef Skeleton_HPP
#define Skeleton_HPP

// Headers here.
#include "Rendering/RenderingCommon.hpp"

#include "Data/Map.hpp"
#include <Data/String.hpp>

namespace Lina::Graphics
{
    class Animation;

    class Skeleton
    {

    public:
        Skeleton() = default;
        ~Skeleton();

        bool LoadSkeleton(const String& path);

        bool IsLoaded() const
        {
            return m_loaded;
        }

        void SetVertexBoneDataToDefault(VertexBoneData& vertex);
        void SetVertexBoneData(VertexBoneData& vertex, int boneID, float weight);

        Map<String, Animation*>& GetAnimations()
        {
            return m_animationMap;
        }

    private:
        bool                              m_loaded = false;
        Vector<VertexBoneData>       m_vertexBoneData;
        Map<String, Animation*> m_animationMap;
        Map<String, BoneInfo>   m_boneInfoMap;

        int   m_boneIDs[MAX_BONE_INFLUENCE];
        float m_boneWeights[MAX_BONE_INFLUENCE];
    };
} // namespace Lina::Graphics

#endif
