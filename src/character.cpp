#include "Character.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

Character::Character(const std::string& modelPath):
        position(0.0f, -0.5f, -1.0f),
        scale(1.8f),
        currentState(STANDING),
        isInvincible(false),
        invincibleTimer(0.0f)
{
    model = new Model(modelPath);
    Animation* idleAnimation = new Animation(GAME_HOME "assets/path/Rifle Idle.dae", model, true);
    animator = new Animator(idleAnimation);

    // Update the model's initial transformation right after initialization
    updateModelTransform();
}

const std::string Character::getStateName(Character::State state) {
    switch (state) {
        case Character::STANDING: return "站立";
        case Character::AIMING: return "瞄准";
        case Character::CROUCHING: return "蹲伏";
        case Character::RELOADING: return "装填";
        case Character::SHOOTING: return "射击";
        case Character::MOVING_LEFT: return "向左移动";
        case Character::MOVING_RIGHT: return "向右移动";
        case Character::JUMPING: return "跳跃";
        case Character::FALLING: return "下落";
        case Character::DEAD: return "死亡";
        default: return "未知状态";
    }
}


Character::~Character() {
    delete model;
    delete animator;
}


void Character::updateModelTransform() {
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // 应用初始位置变换
    modelMatrix = glm::translate(modelMatrix, position);

    // 应用额外的初始平移和缩放
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));  // Translate it down so it's at the center of the scene
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));  // Scale it down

    // 应用模型自身的缩放变换
    modelMatrix = glm::scale(modelMatrix, scale);  // Apply the scale of the character

    // 应用初始旋转
    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));  // Rotate to face the camera

    model->setTransform(modelMatrix);  // 更新模型的变换矩阵
}



void Character::render(Shader& shader)
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    // 应用位置变换
    modelMatrix = glm::translate(modelMatrix, position);
    // 应用缩放变换
    modelMatrix = glm::scale(modelMatrix, scale);
    // 应用初始旋转，这里假设角色的前方需要面向观察者，即需要旋转180度
    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

    // 确保着色器程序激活
    shader.use();
    // 将模型矩阵传递到着色器中
    shader.setMat4("model", modelMatrix);
    shader.setVec4("overrideColor", color);
    shader.setFloat("transparency", color.a);
    // 绘制模型
    model->Draw(shader);
}

void Character::processInput(int key, bool isPressed) {
    if (isPressed) {
        // 在播放新动画前保存当前动画和状态
        if (currentState == STANDING || currentState == CROUCHING) {
            m_PreviousAnimation = animator->GetCurrentAnimation();
        }
        switch (key) {
            case 'A':
                if ((currentState == STANDING || currentState == CROUCHING) && position.x > -1.0f) {
                    facingDirection = LEFT;  // 设置面向左边
                    move();  // 直接处理移动
                }
                break;
            case 'D':
                if ((currentState == STANDING || currentState == CROUCHING) && position.x < 1.0f) {
                    facingDirection = RIGHT;  // 设置面向右边
                    move();  // 直接处理移动
                }
                break;
            case 'W':
                if (currentState == STANDING&& currentState != JUMPING && currentState != FALLING) {
                    jump();  // 处理跳跃和下落
                    std::cout << "跳跃" << std::endl;
                }
                break;
            case 'S':
                if (currentState == STANDING) {
                    crouch();  // 处理蹲下
                    std::cout << "蹲下" << std::endl;
                }
                break;
            case 'R':
                if (currentState == STANDING || currentState == CROUCHING) {
                    reload();  // 处理重新装填
                    std::cout << "重新装填" << std::endl;
                }
                break;
            case 'T':
                onHit();
                std::cout << "被击中" << std::endl;
                break;
        }
    }
    else {
        if (key == 'S' && currentState == CROUCHING) {
            stand();  // 处理停止蹲下
            std::cout << "站立" << std::endl;
        }
    }
}


void Character::update(float deltaTime) {
    // 更新无敌时间
    updateInvincibility(deltaTime);

    // 检查当前动画是否完成且不是循环动画
    if (!animator->GetCurrentAnimation()->loop && animator->IsAnimationFinished() && currentState != MOVING_LEFT && currentState != MOVING_RIGHT) {
        // 恢复到前一个状态和动画
        animator->PlayAnimation(m_PreviousAnimation);
    }

    std::cout << "位置: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    std::cout << "当前状态: " << getStateName(currentState) << std::endl;

    // 更新模型变换矩阵
    updateModelTransform();

    animator->UpdateAnimation(deltaTime);
}



void Character::onHit() {
    if (!isInvincible) {
        std::cout << "Character hit!" << std::endl;
        isInvincible = true;
        invincibleTimer = 1.0f;
    }
}

void Character::jump() {
    Animation* Jumping = new Animation(GAME_HOME "assets/path/Jumping.dae", model, false);
    animator->PlayAnimation(Jumping);
    currentState = JUMPING;
    std::cout << "跳跃" << std::endl;
}

void Character::shoot() {
    // 射击逻辑实现
    std::cout << "射击" << std::endl;
    currentState = STANDING; // 假设射击后角色恢复到站立状态
}

void Character::aim() {
    // 瞄准逻辑实现
    std::cout << "瞄准" << std::endl;
    currentState = AIMING;
}

void Character::crouch() {
    // 蹲伏逻辑实现

    Animation* Crouching = new Animation(GAME_HOME "assets/path/Idle Crouching.dae", model, true);
    animator->PlayAnimation(Crouching);
    std::cout << "蹲下" << std::endl;
    currentState = CROUCHING;
}

void Character::stand() {

    // 在站立状态下检查角色的位置并调整朝向
    if (currentPosition == -1.0f) {
        facingDirection = RIGHT;
    }
    else if (currentPosition == 1.0f) {
        facingDirection = LEFT;
    }
    Animation* Idle = new Animation(GAME_HOME "assets/path/Rifle Idle.dae", model, true);
    animator->PlayAnimation(Idle);
    std::cout << "停止蹲伏，站立" << std::endl;
    currentState = STANDING;
}

void Character::reload() {
    // 装填逻辑
    if (currentState == CROUCHING) {
        Animation* Reloading = new Animation(GAME_HOME "assets/path/Reload.dae", model, false);
        animator->PlayAnimation(Reloading);
    }
    else {
        Animation* Reloading = new Animation(GAME_HOME "assets/path/Reloading.dae", model, false);
        animator->PlayAnimation(Reloading);
    }
    std::cout << "装填" << std::endl;
    currentState = RELOADING;
}



glm::vec3 Character::calculateTargetPosition() {
    glm::vec3 targetPos = position; // 将目标位置初始化为当前位置
    std::cout << "计算前存储的当前位置: " << position.x << ", " << position.y << ", " << position.z << ")\n";


    // 计算移动方向，假设左边是负方向，右边是正方向
    float movementDistance = 1.0f; // 假设移动的距离是1个单位
    if (facingDirection == LEFT && currentPosition > -1.0f) {
        targetPos.x -= movementDistance; // 向左移动
    }
    else if (facingDirection == RIGHT && currentPosition < 1.0f) {
        targetPos.x += movementDistance; // 向右移动

    }

    return targetPos;
}

void Character::move() {
    targetPosition = calculateTargetPosition();  // 计算新的目标位置
    currentState = (facingDirection == LEFT ? MOVING_LEFT : MOVING_RIGHT);
    Animation* animation = new Animation((facingDirection == LEFT ? GAME_HOME "assets/path/Strafe Left.dae" : GAME_HOME "assets/path/Strafe Right.dae"), model, false);
    animator->PlayAnimation(animation);
}

void Character::updateInvincibility(float deltaTime) {
    if (isInvincible) {
        invincibleTimer -= deltaTime;
        // 设置颜色为红色并改变透明度以产生闪烁效果
        if (invincibleTimer > 0.8f) {
            color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // 红色
        }
        else {
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // 正常颜色
        }
        color.a = (sin(glm::radians(360.0f * invincibleTimer * 5)) + 1.0f) / 2.0f; // 闪烁效果

        if (invincibleTimer <= 0.0f) {
            isInvincible = false;
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
}
