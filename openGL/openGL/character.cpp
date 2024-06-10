#include "Character.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

Character::Character(const std::string& modelPath): 

    groundLevel(-0.7f),
    position(0.0f, -0.7f, -1.0f), 
    scale(1.8f), 
    currentState(STANDING), 
    velocityY(0.0f), 
    gravity(50.0f),
    isInvincible(false), 
    invincibleTimer(0.0f),

    initialRotation(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)))  // ��y����ת180��
{
    model = new Model(modelPath);
    Animation* idleAnimation = new Animation("path/Rifle Idle.dae", model, true);
    animator = new Animator(idleAnimation);

}



Character::~Character() {
    delete model;
    delete animator;
}


void Character::processInput(int key, bool isPressed) {
    if (isPressed) {
        // �ڲ����¶���ǰ���浱ǰ������״̬
        m_PreviousAnimation = animator->GetCurrentAnimation();
        m_PreviousState = currentState;
        switch (key) {
        case 'A':
            if (currentState == STANDING) {
                Animation* RunningLeft = new Animation("path/Strafe Right.dae", model, false);
                animator->PlayAnimation(RunningLeft);
                currentState = MOVING_LEFT;
                std::cout << "�����ƶ�" << std::endl;
            }
            break;
        case 'D':
            if (currentState == STANDING) {
                Animation* RunningRight = new Animation("path/Strafe Left.dae", model, false);
                animator->PlayAnimation(RunningRight);
                currentState = MOVING_RIGHT;
                std::cout << "�����ƶ�" << std::endl;
            }
            break;
        case 'W':
            if (currentState == STANDING) {
                Animation* Jumping = new Animation("path/Jumping.dae", model, false);
                animator->PlayAnimation(Jumping);
                currentState = JUMPING;
                velocityY = 5.0f;
                std::cout << "��Ծ" << std::endl;
            }
            break;
        case 'S':
            if (currentState == STANDING) {
                Animation* Crouching = new Animation("path/Idle Crouching.dae", model, true);
                animator->PlayAnimation(Crouching);
                currentState = CROUCHING;
                std::cout << "����" << std::endl;
            }
            break;
        case 'R':
            if (currentState != JUMPING && currentState != FALLING) {
                if (currentState == CROUCHING) {
                    Animation* Reloading = new Animation("path/Reload.dae", model, false);
                    animator->PlayAnimation(Reloading);
                }
                else {
                    Animation* Reloading = new Animation("path/Reloading.dae", model, false);
                    animator->PlayAnimation(Reloading);
                }
                currentState = RELOADING;
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
            Animation* Idle = new Animation("path/Rifle Idle.dae", model, true);
            animator->PlayAnimation(Idle);
            currentState = STANDING;
            std::cout << "վ��" << std::endl;
        }
    }
}


void Character::update(float deltaTime) {
    // �����޵�ʱ��
    updateInvincibility(deltaTime);


    // ���¶���
    animator->UpdateAnimation(deltaTime);

    // ��鵱ǰ�����Ƿ�����Ҳ���ѭ������
    if (!animator->GetCurrentAnimation()->loop && animator->IsAnimationFinished() && currentState != MOVING_LEFT && currentState != MOVING_RIGHT) {
        // �ָ���ǰһ��״̬�Ͷ���
        currentState = m_PreviousState;
        animator->PlayAnimation(m_PreviousAnimation);
    }

    std::cout << position.x << "," << position.y << "," << position.z << std::endl;
    std::cout << currentState << std::endl;

    // ���½�ɫλ��
    switch (currentState) {
    case STANDING:
        // ��վ��״̬�¼���ɫ��λ�ò���������
        if (currentPosition == 0) {
            facingDirection = RIGHT;
        }
        else if (currentPosition == 2) {
            facingDirection = LEFT;
        }
        break;
    case JUMPING:
        position.y += velocityY * deltaTime; // �����µ�Yλ��
        velocityY -= gravity * deltaTime; // Ӧ�����������ٴ�ֱ�ٶ�
        std::cout << "��Ծ��: λ�� = " << position.y << ", �ٶ� = " << velocityY << std::endl;
        if (velocityY <= 0.0) {
            currentState = FALLING; // �����ֱ�ٶ�С�ڵ���0����ʼ����
            std::cout << "��ʼ����" << std::endl;
        }
        break;
    case FALLING:
        position.y += velocityY * deltaTime; // ����Ӧ�ô�ֱ�ٶ�
        velocityY -= gravity * deltaTime; // ����Ӧ������
        std::cout << "������: λ�� = " << position.y << ", �ٶ� = " << velocityY << std::endl;
        if (position.y <= groundLevel) {
            position.y = groundLevel; // ȷ�����ᴩ������
            currentState = STANDING; // �䵽��������վ��״̬
            velocityY = 0; // ���ô�ֱ�ٶ�
            std::cout << "��أ��ص�վ��״̬" << std::endl;
        }
        break;
    case MOVING_LEFT:
        moveTimer += deltaTime; // �����ƶ���ʱ��
        if (moveTimer < moveDuration) {
            // ���ƶ�����ʱ���ڣ����ƶ���Ŀ��λ��
            position = glm::mix(position, targetPosition, moveTimer / moveDuration);
        }
        else {
            // �ƶ���ɺ󣬽�λ������ΪĿ��λ��
            position = targetPosition;
            currentState = STANDING; // �ƶ���ɺ�ָ���վ��״̬
            moveTimer = 0.0f; // �����ƶ���ʱ��
        }
        break;
    case MOVING_RIGHT:
        moveTimer += deltaTime; // �����ƶ���ʱ��
        if (moveTimer < moveDuration) {
            // ���ƶ�����ʱ���ڣ����ƶ���Ŀ��λ��
            position = glm::mix(position, targetPosition, moveTimer / moveDuration);
        }
        else {
            // �ƶ���ɺ󣬽�λ������ΪĿ��λ��
            position = targetPosition;
            currentState = STANDING; // �ƶ���ɺ�ָ���վ��״̬
            moveTimer = 0.0f; // �����ƶ���ʱ��
        }
        break;
    default:
        break;
    }

    // ȷ����λ�ø��º���¶���
    animator->UpdateAnimation(deltaTime);
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

    // ����ģ��
    model->Draw(shader);
}


void Character::onHit() {
    if (!isInvincible) {
        std::cout << "Character hit!" << std::endl;
        isInvincible = true;
        invincibleTimer = 1.0f;
    }
}




void Character::jump() {
    // ʵ����Ծ�߼�
    currentState = JUMPING;
    // ������Ծ���ٶȣ�����
    velocity.y = 10.0f;
}

void Character::shoot() {
    // ʵ������߼�
    currentState = STANDING; // ����������ɫ�ָ���վ��״̬
}

void Character::aim() {
    // ʵ����׼�߼�
    currentState = AIMING;
}

void Character::crouch() {
    // ʵ�ֶ׷��߼�
    currentState = CROUCHING;

    // ���ض�������
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // ����ͼƬʱ��תͼƬ
    unsigned char* data = stbi_load("textures/char/crouch.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB; // ȷ��ͼ���ʽ
        glBindTexture(GL_TEXTURE_2D, texture_char); // ʹ�� Character �������ɵ����� ID
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load crouch texture" << std::endl;
    }
    stbi_image_free(data);
}

void Character::stopCrouch() {
    // ֹͣ�׷��߼�
    currentState = STANDING;

    // ����վ������
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // ����ͼƬʱ��תͼƬ
    unsigned char* data = stbi_load("textures/char/idle.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB; // ȷ��ͼ���ʽ
        glBindTexture(GL_TEXTURE_2D, texture_char); // ʹ�� Character �������ɵ����� ID
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load idle texture" << std::endl;
    }
    stbi_image_free(data);
}



void Character::reload() {
    // ʵ��װ���߼�
    currentState = RELOADING;
}


glm::vec3 Character::calculateTargetPosition() {
    glm::vec3 targetPos = position; // ��Ŀ��λ�ó�ʼ��Ϊ��ǰλ��

    // �����ƶ����򣬼�������Ǹ������ұ���������
    float movementDistance = 1.0f; // �����ƶ��ľ�����1����λ
    if (facingDirection == LEFT) {
        targetPos.x -= movementDistance; // �����ƶ�
    }
    else {
        targetPos.x += movementDistance; // �����ƶ�
    }

    return targetPos;
}



void Character::moveLeft() {
    // ����Ƿ������ƶ�������ǣ���ִ���ƶ��߼�
    if (currentState == MOVING_LEFT || currentState == MOVING_RIGHT) {
        return;
    }

    // ���³���Ϊ��
    facingDirection = LEFT;

    if (currentPosition > 0) {
        currentPosition--;
        targetPosition = calculateTargetPosition(); // ����Ŀ��λ��
        currentState = MOVING_LEFT;
    }
}

void Character::moveRight() {
    // ����Ƿ������ƶ�������ǣ���ִ���ƶ��߼�
    if (currentState == MOVING_LEFT || currentState == MOVING_RIGHT) {
        return;
    }

    // ���³���Ϊ��
    facingDirection = RIGHT;

    if (currentPosition < 2) {
        currentPosition++;
        targetPosition = calculateTargetPosition(); // ����Ŀ��λ��
        currentState = MOVING_RIGHT;
    }
}


void Character::checkBounds() {
    if (position.x < leftPosition.x) position.x = leftPosition.x;
    if (position.x > rightPosition.x) position.x = rightPosition.x;
}

void Character::updateInvincibility(float deltaTime) {
    if (isInvincible) {
        invincibleTimer -= deltaTime;

        // ������ɫ
        if (invincibleTimer > 0.8f) {
            // ��һ�뱣�ֺ�ɫ
            color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else {
            // ��һ��֮�󱣳�������ɫ
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        // ��˸Ч����������0.1��Ϊ��λ�л�͸���ȣ�
        float blinkRate = 0.2f;
        if (fmod(invincibleTimer, blinkRate) > blinkRate / 2.0f) {
            color.a = 0.3f;  // ����͸����Ϊ0.3
        }
        else {
            color.a = 1.0f;  // ����͸����Ϊ1.0
        }

        if (invincibleTimer <= 0.0f) {
            isInvincible = false; // �޵�ʱ�����
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // ��ɫ�ָ�����
        }
    }
}