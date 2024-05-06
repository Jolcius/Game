#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ��������ܵ��ƶ�ѡ��
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// ���������ģʽ
enum Camera_Mode {
    STATIC,
    FREE
};

// ��갴ť
enum Mouse_Button {
    M_LEFT,
    M_RIGHT
};

// Ĭ��ֵ
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
public:
    
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // ŷ����
    float Yaw;
    float Pitch;
    // ����
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    // ģʽ
    Camera_Mode mode;
    glm::vec3 Original_Pos;


    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
        mode = STATIC;
        _isAiming = false;
        Original_Pos = position;
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
        mode = STATIC;
        _isAiming = false;
        Original_Pos = Position;
    }


    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // �������봦��
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        if (mode == FREE)
        {
            float velocity = MovementSpeed * deltaTime;
            if (direction == FORWARD)
                Position += Front * velocity;
            if (direction == BACKWARD)
                Position -= Front * velocity;
            if (direction == LEFT)
                Position -= Right * velocity;
            if (direction == RIGHT)
                Position += Right * velocity;
        }
    }

    // ����ƶ�����
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        if(mode == FREE)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw += xoffset;
            Pitch += yoffset;

            // y�����½�����
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // ���������ǰ/��/������
            updateCameraVectors();
        }
        else if (mode == STATIC && _isAiming)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw += xoffset;
            Pitch += yoffset;

            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
            if (Yaw > 89.0f)
                Yaw = 89.0f;
            if (Yaw < -89.0f)
                Yaw = -89.0f;

            updateCameraVectors();
        }
    }

    // �����ִ���
    void ProcessMouseScroll(float yoffset)
    {
        if (mode == FREE)
        {
            Zoom -= (float)yoffset;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }
    }

    // ������봦��
    void ProcessMouseInput(Mouse_Button btn, glm::vec3 target, bool down, float deltaTime)
    {
        if (mode == STATIC)
        {
            if (btn == M_RIGHT)     // �Ҽ���׼
            {
                _isAiming = down;
                glm::vec3 dir = target - Original_Pos;
                if (down && glm::distance(target, Position) > 0.01f)
                {
                    Position += dir * MovementSpeed * deltaTime;
                }
                if (!down && glm::distance(Original_Pos, Position) > 0.01f)
                {
                    Position -= dir * MovementSpeed * deltaTime;
                }
            }
            if (btn == M_LEFT)      // ������
            {
                // ������
            }
        }
    }

    // ģʽ�л�
    void SwitchMode(Camera_Mode mode)
    {
        this->mode = mode;
    }

private:

    bool _isAiming;

    void updateCameraVectors()
    {
        // ����ǰ������
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // ������/������
        Right = glm::normalize(glm::cross(Front, WorldUp));  
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif

