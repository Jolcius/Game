#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 摄像机可能的移动选项
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// 摄像机拍摄模式
enum Camera_Mode {
    STATIC,
    FREE
};

// 鼠标按钮
enum Mouse_Button {
    M_LEFT,
    M_RIGHT
};

// 默认值
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
    // 欧拉角
    float Yaw;
    float Pitch;
    // 属性
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    // 模式
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

    // 键盘输入处理
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

    // 鼠标移动处理
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        if(mode == FREE)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw += xoffset;
            Pitch += yoffset;

            // y轴上下界限制
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // 更新摄像机前/右/上向量
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

    // 鼠标滚轮处理
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

    // 鼠标输入处理
    void ProcessMouseInput(Mouse_Button btn, glm::vec3 target, bool down, float deltaTime)
    {
        if (mode == STATIC)
        {
            if (btn == M_RIGHT)     // 右键瞄准
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
            if (btn == M_LEFT)      // 左键射击
            {
                // 后座力
            }
        }
    }

    // 模式切换
    void SwitchMode(Camera_Mode mode)
    {
        this->mode = mode;
    }

private:

    bool _isAiming;

    void updateCameraVectors()
    {
        // 计算前向向量
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // 计算右/上向量
        Right = glm::normalize(glm::cross(Front, WorldUp));  
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif

