#pragma once

#include "animation.h"
#include "bone.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>

class Animator {
  public:
    Animator(Animation *animation) {
        m_CurrentTime = 0.0;
        m_CurrentAnimation = animation;

        m_FinalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void UpdateAnimation(float dt) {
        m_DeltaTime = dt;
        if (!m_CurrentAnimation)
            return;

        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;

        if (m_CurrentAnimation->loop) {
            // 重复动画，使用模运算循环时间
            m_CurrentTime =
                fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        } else {
            // 如果动画不循环，防止时间超过动画持续时间
            if (m_CurrentTime >= m_CurrentAnimation->GetDuration()) {
                m_CurrentTime =
                    m_CurrentAnimation->GetDuration(); // 设置时间为动画持续时间
                // 可以在这里调用一个函数来处理动画结束事件
                return;
            }
        }

        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(),
                               glm::mat4(1.0f));
    }

    Animation *GetCurrentAnimation() const {
        return m_CurrentAnimation;
    }

    bool IsAnimationFinished() const {
        return m_CurrentTime >= m_CurrentAnimation->GetDuration();
    }

    void PlayAnimation(Animation *pAnimation) {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
    }

    void CalculateBoneTransform(const AssimpNodeData *node,
                                glm::mat4 parentTransform) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone *Bone = m_CurrentAnimation->FindBone(nodeName);

        if (Bone) {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    std::vector<glm::mat4> GetFinalBoneMatrices() {
        return m_FinalBoneMatrices;
    }

  private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation *m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};
