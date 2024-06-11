#include "stb_image.h"
#include <character.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Character::Character(const std::string &modelPath)
    :

      groundLevel(-0.7f), position(0.0f, -0.7f, -1.0f), scale(1.8f),
      currentState(STANDING), velocityY(0.0f), gravity(50.0f),
      isInvincible(false), invincibleTimer(0.0f), leftPosition(),
      rightPosition(), middlePosition(),

      initialRotation(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
                                  glm::vec3(0.0f, 1.0f, 0.0f))) // ��y����ת180��
{
    model = new Model(modelPath);
    Animation *idleAnimation =
        new Animation("path/Rifle Idle.dae", model, true);
    animator = new Animator(idleAnimation);
}

Character::~Character() {
    delete model;
    delete animator;
}

void Character::processInput(int key, bool isPressed) {
    if (isPressed) {
        // 在播放新动画前保存当前动画和状态
        m_PreviousAnimation = animator->GetCurrentAnimation();
        m_PreviousState = currentState;
        switch (key) {
        case 'A':
            if (currentState == STANDING) {
                Animation *RunningLeft =
                    new Animation("path/Strafe Right.dae", model, false);
                animator->PlayAnimation(RunningLeft);
                currentState = MOVING_LEFT;
                std::cout << "向左移动" << std::endl;
            }
            break;
        case 'D':
            if (currentState == STANDING) {
                Animation *RunningRight =
                    new Animation("path/Strafe Left.dae", model, false);
                animator->PlayAnimation(RunningRight);
                currentState = MOVING_RIGHT;
                std::cout << "向右移动" << std::endl;
            }
            break;
        case 'W':
            if (currentState == STANDING) {
                Animation *Jumping =
                    new Animation("path/Jumping.dae", model, false);
                animator->PlayAnimation(Jumping);
                currentState = JUMPING;
                velocityY = 5.0f;
                std::cout << "跳跃" << std::endl;
            }
            break;
        case 'S':
            if (currentState == STANDING) {
                Animation *Crouching =
                    new Animation("path/Idle Crouching.dae", model, true);
                animator->PlayAnimation(Crouching);
                currentState = CROUCHING;
                std::cout << "蹲下" << std::endl;
            }
            break;
        case 'R':
            if (currentState != JUMPING && currentState != FALLING) {
                if (currentState == CROUCHING) {
                    Animation *Reloading =
                        new Animation("path/Reload.dae", model, false);
                    animator->PlayAnimation(Reloading);
                } else {
                    Animation *Reloading =
                        new Animation("path/Reloading.dae", model, false);
                    animator->PlayAnimation(Reloading);
                }
                currentState = RELOADING;
                std::cout << "重新装填" << std::endl;
            }
            break;
        case 'T':
            onHit();
            std::cout << "被击中" << std::endl;
            break;
        }
    } else {
        if (key == 'S' && currentState == CROUCHING) {
            Animation *Idle = new Animation("path/Rifle Idle.dae", model, true);
            animator->PlayAnimation(Idle);
            currentState = STANDING;
            std::cout << "站立" << std::endl;
        }
    }
}

void Character::update(float deltaTime) {
    // 更新无敌时间
    updateInvincibility(deltaTime);

    // 更新动画
    animator->UpdateAnimation(deltaTime);

    // 检查当前动画是否完成且不是循环动画
    if (!animator->GetCurrentAnimation()->loop &&
        animator->IsAnimationFinished() && currentState != MOVING_LEFT &&
        currentState != MOVING_RIGHT) {
        // 恢复到前一个状态和动画
        currentState = m_PreviousState;
        animator->PlayAnimation(m_PreviousAnimation);
    }

    std::cout << position.x << "," << position.y << "," << position.z
              << std::endl;
    std::cout << currentState << std::endl;

    // 更新角色位置
    switch (currentState) {
    case STANDING:
        // 在站立状态下检查角色的位置并调整朝向
        if (currentPosition == 0) {
            facingDirection = RIGHT;
        } else if (currentPosition == 2) {
            facingDirection = LEFT;
        }
        break;
    case JUMPING:
        position.y += velocityY * deltaTime; // 计算新的Y位置
        velocityY -= gravity * deltaTime; // 应用重力，减少垂直速度
        std::cout << "跳跃中: 位置 = " << position.y << ", 速度 = " << velocityY
                  << std::endl;
        if (velocityY <= 0.0) {
            currentState = FALLING; // 如果垂直速度小于等于0，开始下落
            std::cout << "开始下落" << std::endl;
        }
        break;
    case FALLING:
        position.y += velocityY * deltaTime; // 继续应用垂直速度
        velocityY -= gravity * deltaTime;    // 继续应用重力
        std::cout << "下落中: 位置 = " << position.y << ", 速度 = " << velocityY
                  << std::endl;
        if (position.y <= groundLevel) {
            position.y = groundLevel; // 确保不会穿过地面
            currentState = STANDING;  // 落到地面后进入站立状态
            velocityY = 0;            // 重置垂直速度
            std::cout << "落地，回到站立状态" << std::endl;
        }
        break;
    case MOVING_LEFT:
        moveTimer += deltaTime; // 更新移动计时器
        if (moveTimer < moveDuration) {
            // 在移动持续时间内，逐渐移动到目标位置
            position =
                glm::mix(position, targetPosition, moveTimer / moveDuration);
        } else {
            // 移动完成后，将位置设置为目标位置
            position = targetPosition;
            currentState = STANDING; // 移动完成后恢复到站立状态
            moveTimer = 0.0f;        // 重置移动计时器
        }

        break;
    case MOVING_RIGHT:
        moveTimer += deltaTime; // 更新移动计时器
        if (moveTimer < moveDuration) {
            // 在移动持续时间内，逐渐移动到目标位置
            position =
                glm::mix(position, targetPosition, moveTimer / moveDuration);
        } else {
            // 移动完成后，将位置设置为目标位置
            position = targetPosition;
            currentState = STANDING; // 移动完成后恢复到站立状态
            moveTimer = 0.0f;        // 重置移动计时器
        }
        break;
    default: break;
    }

    // 确保在位置更新后更新动画
    animator->UpdateAnimation(deltaTime);
}

void Character::render(Shader &shader) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    // 应用位置变换
    modelMatrix = glm::translate(modelMatrix, position);
    // 应用缩放变换
    modelMatrix = glm::scale(modelMatrix, scale);
    // 应用初始旋转，这里假设角色的前方需要面向观察者，即需要旋转180度
    modelMatrix =
        glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

    // 确保着色器程序激活
    shader.use();
    // 将模型矩阵传递到着色器中
    shader.setMat4("model", modelMatrix);

    // 绘制模型
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
    // 实现跳跃逻辑
    currentState = JUMPING;
    // 设置跳跃初速度，例如
    velocity.y = 10.0f;
}

void Character::shoot() {
    // 实现射击逻辑
    currentState = STANDING; // 假设射击后角色恢复到站立状态
}

void Character::aim() {
    // 实现瞄准逻辑
    currentState = AIMING;
}

void Character::crouch() {
    // 实现蹲伏逻辑
    currentState = CROUCHING;

    // 加载蹲下纹理
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // 加载图片时翻转图片
    unsigned char *data =
        stbi_load("textures/char/crouch.png", &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB; // 确定图像格式
        glBindTexture(GL_TEXTURE_2D,
                      texture_char); // 使用 Character 类中生成的纹理 ID
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load crouch texture" << std::endl;
    }
    stbi_image_free(data);
}

void Character::stopCrouch() {
    // 停止蹲伏逻辑
    currentState = STANDING;

    // 加载站立纹理
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // 加载图片时翻转图片
    unsigned char *data =
        stbi_load("textures/char/idle.png", &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB; // 确定图像格式
        glBindTexture(GL_TEXTURE_2D,
                      texture_char); // 使用 Character 类中生成的纹理 ID
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
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
    } else {
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
    if (position.x < leftPosition.x)
        position.x = leftPosition.x;
    if (position.x > rightPosition.x)
        position.x = rightPosition.x;
}

void Character::updateInvincibility(float deltaTime) {
    if (isInvincible) {
        invincibleTimer -= deltaTime;

        // ������ɫ
        if (invincibleTimer > 0.8f) {
            // ��һ�뱣�ֺ�ɫ
            color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        } else {
            // ��һ��֮�󱣳�������ɫ
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        // ��˸Ч����������0.1��Ϊ��λ�л�͸���ȣ�
        float blinkRate = 0.2f;
        if (fmod(invincibleTimer, blinkRate) > blinkRate / 2.0f) {
            color.a = 0.3f; // ����͸����Ϊ0.3
        } else {
            color.a = 1.0f; // ����͸����Ϊ1.0
        }

        if (invincibleTimer <= 0.0f) {
            isInvincible = false; // �޵�ʱ�����
            color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // ��ɫ�ָ�����
        }
    }
}
