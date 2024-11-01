#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H

#include <glm/glm.hpp>
#include "Model.h"
#include "Animator.h"
#include "Animation.h"
#include <string>

class Character {
public:
    enum State {
        STANDING,    // վ��״̬
        AIMING,      // ��׼״̬
        CROUCHING,   // �׷�״̬
        RELOADING,   // װ��״̬
        SHOOTING,    // ���״̬
        MOVING_LEFT, // �����ƶ�
        MOVING_RIGHT,// �����ƶ�
        JUMPING,     // ��Ծ״̬
        FALLING,     // ����״̬
        DEAD         // ����״̬
    };

    enum Direction {
        LEFT,
        RIGHT
    };


    Direction facingDirection = RIGHT; // Ĭ�ϳ�������

    Character(const std::string& modelPath);
    ~Character();

    void processInput(int key, bool isPressed);
    void update(float deltaTime);
    void updateModelTransform();
    void render(Shader& shader);
    const std::string getStateName(State state);
    void onHit();

    void setPosition(glm::vec3 newPosition);
    glm::vec3 getPosition() { return position; }
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
    float moveDuration = 0.5f;
    float moveTimer;

    float groundLevel;
    float gravity;

    void switchAnimation(const std::string& animationName);

    void jump();
    void shoot();
    void aim();
    void crouch();
    void stand();
    void reload();
    void move();

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