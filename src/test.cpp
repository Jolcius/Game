#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <filesystem>
#include <vector>

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "animator.h"
#include "model.h"
#include "cube.h"
#include "shelter.h"
#include "trap.h"
#include "machine.h"
#include "levelManager.h"
#include "audio_manager.h"
#include "stb_image.h"
#include "character.h"
#include "ray.h"

using namespace std;

int SCR_WIDTH = 1920, SCR_HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Character& character);
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

int fire_counter = 100; // 枪口火焰渲染计数器

int main()
{
    // 初始化glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    else
    {
        std::cout << "GLAD initialized successfully" << std::endl;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 隐藏鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 着色器
    Shader shader(GAME_HOME "assets/path/shaders/shader.vs", GAME_HOME "assets/path/shaders/shader.fs");
    Shader modelShader(GAME_HOME "assets/path/shaders/anim_model.vs", GAME_HOME "assets/path/shaders/anim_model.fs");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    std::cout << "Generated VAO: " << VAO << " VBO: " << VBO << std::endl;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 确保在 OpenGL 初始化后创建 Character 对象
    // 创建 Character 对象，传递模型路径
    Character character(GAME_HOME "assets/path/JoyfulJump.dae");

    // 关卡生成器
    LevelManager _lm(objects);
    _lm.SetDifficulty(_lm.DevilHunter);
    _lm.InitScene();

    // 枪口火焰
    Cube* fire = new Cube(glm::vec3(0, 0, 0));
    Texture_cube* fire_tex = new Texture_cube(GAME_HOME "assets/textures/fire.png");
    std::cout << "Loading fire texture from: " << GAME_HOME "assets/textures/fire.png" << std::endl;
    fire->setTexture(*fire_tex);
    fire->setScale(glm::vec3(1, 1, 0));
    // 准星
    Cube* aim = new Cube(glm::vec3(0, 0, -1));
    Texture_cube* aim_tex = new Texture_cube(GAME_HOME "assets/textures/aim.png");
    std::cout << "Loading aim texture from: " << GAME_HOME "assets/textures/aim.png" << std::endl;
    aim->setTexture(*aim_tex);
    aim->setScale(glm::vec3(1, 1, 0));

    camera.MovementSpeed = 10.f;

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 帧数计算
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入
        processInput(window, character);

        // 更新角色状态和位置
        character.update(deltaTime);

        // 更新动画
        character.animator->UpdateAnimation(deltaTime);

        // 渲染背景
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::cout << objects.size() << std::endl;

        // 模型着色器配置参数
        modelShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        auto transforms = character.animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
            modelShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);


        // 设置着色器参数
        shader.use();

        int viewLoc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Debug: 检查 uniform 变量是否正确设置
        glm::mat4 model = glm::mat4(1.0f);
        int modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        character.model->Draw(modelShader);

        // LevelManager生成敌人
        _lm.GenerateEnemy();

        // 炮台射击
        vector<Cube*> new_objects;
        for (auto& obj : objects)
        {
            if (dynamic_cast<Machine*>(obj))
            {
                Cube* new_obj = ((Machine*)obj)->Shoot();
                if (dynamic_cast<Bullet*>(new_obj))
                {
                    new_objects.push_back(new_obj);
                }
            }
        }
        for (auto& obj : new_objects)
        {
            objects.push_back(obj);
        }

        // 画出Objects
        for (auto& obj : objects)
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

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            // Debug: 检查顶点数据
            float* vertexData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
            if (vertexData != nullptr) {
                for (int i = 0; i < 36 * 5; i++) { // 假设每个顶点有 5 个属性（3 个位置 + 2 个纹理坐标），36 个顶点
                    std::cout << vertexData[i] << " ";
                    if ((i + 1) % 5 == 0) std::cout << std::endl;
                }
                glUnmapBuffer(GL_ARRAY_BUFFER);
            } else {
                std::cout << "Failed to map buffer for debugging." << std::endl;
            }

            glDrawArrays(GL_TRIANGLES, 0, 36);


            // 处理物体移动
            obj->setPos(obj->getPos() + obj->getVelocity() * deltaTime);

            // 检查陷阱位置
            if (dynamic_cast<Trap*>(obj) || dynamic_cast<Bullet*>(obj))
            {
                glm::vec3 pos = obj->getPos();
                if (!_lm.shelter->_delete && pos.z >= -2 && pos.z < -1)
                {
                    if (abs(_lm.shelter_x_pos - pos.x) < 0.01f && pos.y < -0.5f)
                    {
                        obj->_delete = true;
                        _lm.shelter->TakeDamage(1);
                    }
                }
                if (pos.z >= -1)
                {
                    obj->_delete = true;
                    if (abs(pos.x - character.getPosition().x) < 0.01f)
                    {
                        if(pos.y >= -0.5f && character.currentState != character.CROUCHING)
                            character.onHit();
                        if (pos.y < -0.5f && character.currentState != character.JUMPING)
                            character.onHit();
                    }
                }
            }
        }

        // 处理Objects删除
        for (auto& obj : objects)
        {
            if (obj->_delete)
            {
                remove_object(obj);
                break;
            }
        }

        // 渲染UI
        glDisable(GL_DEPTH_TEST); //关闭深度测试
        if (camera.Position == glm::vec3(character.getPosition().x, 0, -1.2f))
        {
            if (fire_counter <= 2)
            {
                // 绑定纹理
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, fire->getTexture());

                glm::mat4 model;
                model = glm::translate(model, camera.Position + camera.Front);
                model = glm::rotate(model, glm::radians(camera.Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(90 - camera.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, fire->getScale());

                // 向顶点着色器输入model矩阵
                int modelLoc = glGetUniformLocation(shader.ID, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // 向片段着色器输入颜色
                glm::vec4 color = fire->getColor();
                int colorLoc = glGetUniformLocation(shader.ID, "color");
                glUniform4f(colorLoc, color.x, color.y, color.z, color.w);

                glDrawArrays(GL_TRIANGLES, 0, 36);
                fire_counter++;
            }
            else if (fire_counter >= 1e6) fire_counter = 1e6; // 防止越界
            // 绑定纹理
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, aim->getTexture());

            glm::mat4 model;
            model = glm::translate(model, camera.Position + camera.Front);
            model = glm::rotate(model, glm::radians(camera.Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90 - camera.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, aim->getScale());

            // 向顶点着色器输入model矩阵
            int modelLoc = glGetUniformLocation(shader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // 向片段着色器输入颜色
            glm::vec4 color = aim->getColor();
            int colorLoc = glGetUniformLocation(shader.ID, "color");
            glUniform4f(colorLoc, color.x, color.y, color.z, color.w);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glEnable(GL_DEPTH_TEST);

        character.render(shader); // 传递 Shader 对象的引用

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

void processInput(GLFWwindow* window, Character& character)
{
    static bool W_pressed = false;
    static bool S_pressed = false;
    static bool A_pressed = false;
    static bool D_pressed = false;
    static bool R_pressed = false;
    static bool prev_W_pressed = false;
    static bool prev_S_pressed = false;
    static bool prev_A_pressed = false;
    static bool prev_D_pressed = false;
    static bool prev_R_pressed = false;

    static bool shoot_pressed = false;
    static bool prev_shoot_pressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 主角操作，只在按下时触发一次
    W_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    S_pressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    A_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    D_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    R_pressed = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    shoot_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;

    if (W_pressed && !prev_W_pressed)
    {
        character.processInput('W', true); // 按下W键，传入键值'W'和true
    }
    if (!W_pressed && prev_W_pressed)
    {
        character.processInput('W', false); // 松开W键，传入键值'W'和false
    }

    if (S_pressed && !prev_S_pressed)
    {
        character.processInput('S', true); // 按下S键，传入键值'S'和true
    }
    if (!S_pressed && prev_S_pressed)
    {
        character.processInput('S', false); // 松开S键，传入键值'S'和false
    }

    if (A_pressed && !prev_A_pressed)
    {
        character.processInput('A', true); // 按下A键，传入键值'A'和true
    }
    if (!A_pressed && prev_A_pressed)
    {
        character.processInput('A', false); // 松开A键，传入键值'A'和false
    }

    if (D_pressed && !prev_D_pressed)
    {
        character.processInput('D', true); // 按下D键，传入键值'D'和true
    }
    if (!D_pressed && prev_D_pressed)
    {
        character.processInput('D', false); // 松开D键，传入键值'D'和false
    }

    if (R_pressed && !prev_R_pressed)
    {
        character.processInput('R', true); // 按下R键，传入键值'R'和true
    }
    if (!R_pressed && prev_R_pressed)
    {
        character.processInput('R', false); // 松开R键，传入键值'R'和false
    }

    if (shoot_pressed && !prev_shoot_pressed)
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
        {
            Ray* ray = new Ray(camera.Position, glm::normalize(camera.Front));
            Cube* hit = ray->RayCastCubes(objects);
            if (dynamic_cast<Trap*>(hit)) ((Trap*)hit)->TakeDamage(4);
            if (dynamic_cast<Machine*>(hit)) ((Machine*)hit)->TakeDamage(2);
            if (dynamic_cast<Bullet*>(hit)) ((Bullet*)hit)->TakeDamage(10);

            fire_counter = 0;
        }
    }
    if (!shoot_pressed && prev_shoot_pressed)
    {
        //松开左键
    }

    // 保存当前帧的按键状态以供下一帧使用
    prev_W_pressed = W_pressed;
    prev_S_pressed = S_pressed;
    prev_A_pressed = A_pressed;
    prev_D_pressed = D_pressed;
    prev_R_pressed = R_pressed;

    prev_shoot_pressed = shoot_pressed;

    // 摄像机移动
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // 鼠标输入
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
        camera.ProcessMouseInput(M_LEFT, glm::vec3(0, 0, -1.2f), true, deltaTime);
    else
        camera.ProcessMouseInput(M_LEFT, glm::vec3(0, 0, -1.2f), false, deltaTime);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && character.currentState == character.STANDING)
        camera.ProcessMouseInput(M_RIGHT, glm::vec3(character.getPosition().x, 0, -1.2f), true, deltaTime);
    else
        camera.ProcessMouseInput(M_RIGHT, glm::vec3(character.getPosition().x, 0, -1.2f), false, deltaTime);

    // 测试键
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        character.processInput('T', true);
}
