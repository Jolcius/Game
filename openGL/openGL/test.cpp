#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "cube.h"
#include "audio_manager.h"

using namespace std;

const int SCR_WIDTH = 600, SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void remove_object(Cube* cube);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;

unsigned int vertexShader;
unsigned int fragmentShader;

// 全局物体
vector<Cube*> objects;

int main()
{
    // 初始化glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Preview", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 着色器
    Shader shader("path/shaders/shader.vs", "path/shaders/shader.fs");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 创建纹理
    Texture* cube_tex = new Texture("textures/container.jpg");
    Texture* senpai_tex = new Texture("textures/senpai.jpg");
    Texture* wall_tex = new Texture("textures/wall.jpg");
    Texture* ceiling_tex = new Texture("textures/ceiling.jpg");

    glEnable(GL_DEPTH_TEST);

    // 地面和天花板
    glm::vec3 ground_pos = glm::vec3(0, -1.5, -10);
    Cube* ground = new Cube(ground_pos);
    ground->setTexture(*cube_tex);
    ground->setScale(glm::vec3(3, 0, 20));
    objects.push_back(ground);
    Cube* ceiling = new Cube(glm::vec3(0, 1.5, -10));
    ceiling->setTexture(*ceiling_tex);
    ceiling->setScale(glm::vec3(3, 0, 20));
    objects.push_back(ceiling);

    // 墙壁
    Cube* wall_1 = new Cube(glm::vec3(1.5, 0, -10));
    wall_1->setScale(glm::vec3(0, 3, 20));
    wall_1->setTexture(*wall_tex);
    objects.push_back(wall_1);
    Cube* wall_2 = new Cube(glm::vec3(-1.5, 0, -10));
    wall_2->setScale(glm::vec3(0, 3, 20));
    wall_2->setTexture(*wall_tex);
    objects.push_back(wall_2);
    Cube* wall_3 = new Cube(glm::vec3(0, 0, -20));
    wall_3->setScale(glm::vec3(3, 3, 0));
    wall_3->setTexture(*wall_tex);
    objects.push_back(wall_3);

    // 掩体
    Cube* shelter_1 = new Cube(glm::vec3(0, -1, -2));
    shelter_1->setTexture(*cube_tex);
    objects.push_back(shelter_1);
    Cube* shelter_2 = new Cube(glm::vec3(-1, -1, -2));
    shelter_2->setTexture(*cube_tex);
    objects.push_back(shelter_2);
    Cube* shelter_3 = new Cube(glm::vec3(1, -1, -2));
    shelter_3->setTexture(*cube_tex);
    objects.push_back(shelter_3);

    camera.MovementSpeed = 10.f;

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 帧数计算
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入
        processInput(window);

        // 渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();

        // 坐标变换矩阵
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), SCR_WIDTH * 1.0f / SCR_HEIGHT, 0.1f, 100.0f);
        
        int viewLoc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);
        
        // 画出Objects
        for (auto &obj : objects)
        {
            // 绑定纹理
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, obj->getTexture());

            glm::mat4 model;
            model = glm::translate(model, obj->getPos());
            model = glm::rotate(model, glm::radians(obj->getRot().x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(obj->getRot().y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(obj->getRot().z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, obj->getScale());

            // 向顶点着色器输入model矩阵
            int modelLoc = glGetUniformLocation(shader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // 向片段着色器输入颜色
            glm::vec4 color = obj->getColor();
            int colorLoc = glGetUniformLocation(shader.ID, "color");
            glUniform4f(colorLoc, color.x, color.y, color.z, color.w);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            // 处理物体移动
            obj->setPos(obj->getPos() + obj->getVelocity() * deltaTime);
        }

        // 处理Objects删除
        for (auto &obj : objects)
        {
            if (obj->_delete)
            {
                remove_object(obj);
                break;
            }
        }

        // 交换缓冲，调用事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void remove_object(Cube* cube)
{
    objects.erase(remove_if(objects.begin(), objects.end(), [cube](Cube* obj) {return *cube == *obj; }), objects.end());
    delete cube;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // 摄像机移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // 鼠标输入
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
        camera.ProcessMouseInput(M_LEFT, glm::vec3(0, 0, 0), true, deltaTime);
    else
        camera.ProcessMouseInput(M_LEFT, glm::vec3(0, 0, 0), false, deltaTime);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
        camera.ProcessMouseInput(M_RIGHT, glm::vec3(0, 0, 0), true, deltaTime);
    else
        camera.ProcessMouseInput(M_RIGHT, glm::vec3(0, 0, 0), false, deltaTime);
}