#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H

#include <glm/glm.hpp>
#include <model.h>
#include <animator.h>
#include <animation.h>
#include <string>

class Character {
public:
    enum State {
        STANDING,    // 站立状态
        AIMING,      // 瞄准状态
        CROUCHING,   // 蹲伏状态
        RELOADING,   // 装填状态
        MOVING_LEFT, // 向左移动
        MOVING_RIGHT,// 向右移动
        JUMPING,     // 跳跃状态
        FALLING,     // 下落状态
        DEAD         // 死亡状态
    };

    enum Direction {
        LEFT,
        RIGHT
    };


    Direction facingDirection = RIGHT; // 默认朝向向右

    Character(const std::string& modelPath);
    ~Character();

    void processInput(int key, bool isPressed);
    void update(float deltaTime);
    void render(Shader& shader);

    void onHit();

    void setPosition(glm::vec3 newPosition);
    void setTexture(unsigned int texID);
    void setScale(glm::vec3 newScale);
    void setColor(glm::vec4 newColor);
    void setInjured(bool injured);

    Model* model;
    Animator* animator;
    Animation* m_PreviousAnimation = nullptr;

    State currentState;
    State m_PreviousState = STANDING;
private:




    float velocityY;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 scale;
    glm::vec4 color;
    glm::mat4 initialRotation;

    unsigned int textureID;
    float invincibleTimer;
    bool isHurt;
    bool isInvincible;
    unsigned int texture_char;
    unsigned int VBO_char, VAO_char;
    float moveDuration;
    float moveTimer;

    float groundLevel;
    float gravity;

    void switchAnimation(const std::string& animationName);

    void jump();
    void shoot();
    void aim();
    void crouch();
    void stopCrouch();
    void reload();
    void moveLeft();
    void moveRight();

    void checkBounds();
    void updateInvincibility(float deltaTime);

    const glm::vec3 leftPosition;
    const glm::vec3 middlePosition;
    const glm::vec3 rightPosition;
    int currentPosition;
    glm::vec3 targetPosition;
    glm::vec3 calculateTargetPosition();
};

#endif // CHARACTER_H
