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
        STANDING,    // Õ¾Á¢×´Ì¬
        AIMING,      // Ãé×¼×´Ì¬
        CROUCHING,   // ¶×·ü×´Ì¬
        RELOADING,   // ×°Ìî×´Ì¬
        MOVING_LEFT, // Ïò×óÒÆ¶¯
        MOVING_RIGHT,// ÏòÓÒÒÆ¶¯
        JUMPING,     // ÌøÔ¾×´Ì¬
        FALLING,     // ÏÂÂä×´Ì¬
        DEAD         // ËÀÍö×´Ì¬
    };

    enum Direction {
        LEFT,
        RIGHT
    };


    Direction facingDirection = RIGHT; // Ä¬ÈÏ³¯ÏòÏòÓÒ

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
