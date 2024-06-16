#include "Character.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

Character::Character(const std::string& modelPath): 
    groundLevel(-0.5f),
    position(0.0f, -0.5f, -1.0f), 
    scale(1.8f), 
    currentState(STANDING), 
    velocityY(0.0f), 
    gravity(50.0f),
    isInvincible(false), 
    invincibleTimer(0.0f)
{
    model = new Model(modelPath);
    Animation* idleAnimation = new Animation("path/Rifle Idle.dae", model, true);
    animator = new Animator(idleAnimation);

    // Update the model's initial transformation right after initialization
    updateModelTransform();
}

const std::string Character::getStateName(Character::State state) {
    switch (state) {
    case Character::STANDING: return "վ��";
    case Character::AIMING: return "��׼";
    case Character::CROUCHING: return "�׷�";
    case Character::RELOADING: return "װ��";
    case Character::SHOOTING: return "���";
    case Character::MOVING_LEFT: return "�����ƶ�";
    case Character::MOVING_RIGHT: return "�����ƶ�";
    case Character::JUMPING: return "��Ծ";
    case Character::FALLING: return "����";
    case Character::DEAD: return "����";
    default: return "δ֪״̬";
    }
}


Character::~Character() {
    delete model;
    delete animator;
}


void Character::updateModelTransform() {
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Ӧ�ó�ʼλ�ñ任
    modelMatrix = glm::translate(modelMatrix, position);

    // Ӧ�ö���ĳ�ʼƽ�ƺ�����
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));  // Translate it down so it's at the center of the scene
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));  // Scale it down

    // Ӧ��ģ����������ű任
    modelMatrix = glm::scale(modelMatrix, scale);  // Apply the scale of the character

    // Ӧ�ó�ʼ��ת
    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));  // Rotate to face the camera

    model->setTransform(modelMatrix);  // ����ģ�͵ı任����
}



void Character::render(Shader& shader)
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    // Ӧ��λ�ñ任
    modelMatrix = glm::translate(modelMatrix, position);
    // Ӧ�����ű任
    modelMatrix = glm::scale(modelMatrix, scale);
    // Ӧ�ó�ʼ��ת����������ɫ��ǰ����Ҫ����۲��ߣ�����Ҫ��ת180��
    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

    // ȷ����ɫ�����򼤻�
    shader.use();
    // ��ģ�;��󴫵ݵ���ɫ����
    shader.setMat4("model", modelMatrix);
    shader.setVec4("overrideColor", color);
    shader.setFloat("transparency", color.a);
    // ����ģ��
    model->Draw(shader);
}

void Character::processInput(int key, bool isPressed) {
    if (isPressed) {
        // �ڲ����¶���ǰ���浱ǰ������״̬
        if (currentState == STANDING || currentState == CROUCHING) {
            m_PreviousAnimation = animator->GetCurrentAnimation();
            m_PreviousState = currentState;
        }
        switch (key) {
        case 'A':
            if ((currentState == STANDING || currentState == CROUCHING) && position.x > -1) {
                facingDirection = LEFT;  // �����������
                move();  // ֱ�Ӵ����ƶ�
            }
            break;
        case 'D':
            if ((currentState == STANDING || currentState == CROUCHING) && position.x < 1) {
                facingDirection = RIGHT;  // ���������ұ�
                move();  // ֱ�Ӵ����ƶ�
            }
            break;
        case 'W':
            if (currentState == STANDING&& currentState != JUMPING && currentState != FALLING) {
                jump();  // ������Ծ������
                std::cout << "��Ծ" << std::endl;
            }
            break;
        case 'S':
            if (currentState == STANDING) {
                crouch();  // �������
                std::cout << "����" << std::endl;
            }
            break;
        case 'R':
            if (currentState == STANDING || currentState == CROUCHING) {
                reload();  // ��������װ��
                std::cout << "����װ��" << std::endl;
            }
            break;
        case 'T':
            onHit();
            std::cout << "������" << std::endl;
            break;
        }
    }
    else {
        if (key == 'S' && currentState == CROUCHING) {
            stand();  // ����ֹͣ����
            std::cout << "վ��" << std::endl;
        }
    }
}


void Character::update(float deltaTime) {
    // �����޵�ʱ��
    updateInvincibility(deltaTime);

    // ��鵱ǰ�����Ƿ�����Ҳ���ѭ������
    if (!animator->GetCurrentAnimation()->loop && animator->IsAnimationFinished() && currentState != MOVING_LEFT && currentState != MOVING_RIGHT) {
        // �ָ���ǰһ��״̬�Ͷ���
        currentState = m_PreviousState;
        animator->PlayAnimation(m_PreviousAnimation);
    }

    if (currentState == MOVING_LEFT || currentState == MOVING_RIGHT) {
        moveTimer += deltaTime;
        if (moveTimer >= 0.55f) {
            position = targetPosition;
            currentState = STANDING;
            moveTimer = 0.0f;
            stand();
        }
    }

    //std::cout << "λ��: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    //std::cout << "��ǰ״̬: " << getStateName(currentState) << std::endl;

    // ����ģ�ͱ任����
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
    Animation* Jumping = new Animation("path/Jumping.dae", model, false);
    animator->PlayAnimation(Jumping);
    currentState = JUMPING;
    velocityY = 10.0f; // ������Ծ�ĳ�ʼ�ٶ�
    std::cout << "��Ծ" << std::endl;
}

void Character::shoot() {
    // ����߼�ʵ��
    std::cout << "���" << std::endl;
    currentState = STANDING; // ����������ɫ�ָ���վ��״̬
}

void Character::aim() {
    // ��׼�߼�ʵ��
    std::cout << "��׼" << std::endl;
    currentState = AIMING;
}

void Character::crouch() {
    // �׷��߼�ʵ��

    Animation* Crouching = new Animation("path/Idle Crouching.dae", model, true);
    animator->PlayAnimation(Crouching);
    std::cout << "����" << std::endl;
    currentState = CROUCHING;
}

void Character::stand() {

    // ��վ��״̬�¼���ɫ��λ�ò���������
    if (currentPosition == -1) {
        facingDirection = RIGHT;
    }
    else if (currentPosition == 1) {
        facingDirection = LEFT;
    }
    Animation* Idle = new Animation("path/Rifle Idle.dae", model, true);
    animator->PlayAnimation(Idle);
    std::cout << "ֹͣ�׷���վ��" << std::endl;
    currentState = STANDING;
}

void Character::reload() {
    // װ���߼�
    if (currentState == CROUCHING) {
        Animation* Reloading = new Animation("path/Reload.dae", model, false);
        animator->PlayAnimation(Reloading);
    }
    else {
        Animation* Reloading = new Animation("path/Reloading.dae", model, false);
        animator->PlayAnimation(Reloading);
    }
    std::cout << "װ��" << std::endl;
    currentState = RELOADING;
}



glm::vec3 Character::calculateTargetPosition() {
    glm::vec3 targetPos = position; // ��Ŀ��λ�ó�ʼ��Ϊ��ǰλ��
    std::cout << "����ǰ�洢�ĵ�ǰλ��: " << position.x << ", " << position.y << ", " << position.z << ")\n";


    // �����ƶ����򣬼�������Ǹ������ұ���������
    float movementDistance = 1.0f; // �����ƶ��ľ�����1����λ
    if (facingDirection == LEFT && currentPosition > -1) {
        targetPos.x -= movementDistance; // �����ƶ�
    }
    else if (facingDirection == RIGHT && currentPosition < 1) {
        targetPos.x += movementDistance; // �����ƶ�

    }

    return targetPos;
}

void Character::move() {
    targetPosition = calculateTargetPosition();  // �����µ�Ŀ��λ��
    moveTimer = 0.0f;  // �����ƶ���ʱ��
    currentState = (facingDirection == LEFT ? MOVING_LEFT : MOVING_RIGHT);
    Animation* animation = new Animation((facingDirection == LEFT ? "path/Strafe Left.dae" : "path/Strafe Right.dae"), model, false);
    animator->PlayAnimation(animation);
}


void Character::checkBounds() {
    if (position.x < leftPosition.x) position.x = leftPosition.x;
    if (position.x > rightPosition.x) position.x = rightPosition.x;
}

void Character::updateInvincibility(float deltaTime) {
    if (isInvincible) {
        invincibleTimer -= deltaTime;
        // ������ɫΪ��ɫ���ı�͸�����Բ�����˸Ч��
        if (invincibleTimer > 0.8f) {
            color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // ��ɫ
        }
        else {
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // ������ɫ
        }
        color.a = (sin(glm::radians(360.0f * invincibleTimer * 5)) + 1.0f) / 2.0f; // ��˸Ч��

        if (invincibleTimer <= 0.0f) {
            isInvincible = false;
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
}
