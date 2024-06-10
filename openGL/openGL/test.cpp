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
#include "audio_manager.h"
#include "stb_image.h"
#include "character.h"

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

// ȫ������
vector<Cube*> objects;


int main()
{
	// ��ʼ��glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// ����ȫ������
	//const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//SCR_WIDTH = mode->width;
	//SCR_HEIGHT = mode->height;
	//GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Preview", glfwGetPrimaryMonitor(), NULL);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Preview", NULL, NULL);


	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// ��ʼ��glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);



	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// �������
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ��ɫ��
	Shader shader("path/shaders/shader.vs", "path/shaders/shader.fs");
	Shader modelShader("path/shaders/anim_model.vs", "path/shaders/anim_model.fs");

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

	// ����cube����
	Texture_cube* cube_tex = new Texture_cube("textures/container.jpg");
	Texture_cube* senpai_tex = new Texture_cube("textures/senpai.jpg");
	Texture_cube* wall_tex = new Texture_cube("textures/wall.jpg");
	Texture_cube* ceiling_tex = new Texture_cube("textures/ceiling.jpg");
	Texture_cube* aim_tex = new Texture_cube("textures/aim.png");


	glEnable(GL_DEPTH_TEST);

	//glDisable(GL_CULL_FACE);

	// ȷ���� OpenGL ��ʼ���󴴽� Character ����
	// ���� Character ���󣬴���ģ��·��
	Character character("path/JoyfulJump.dae");


	// ������컨��
	glm::vec3 ground_pos = glm::vec3(0, -1.5, -10);
	Cube* ground = new Cube(ground_pos);
	ground->setTexture(*cube_tex);
	ground->setScale(glm::vec3(3, 0, 20));
	objects.push_back(ground);
	Cube* ceiling = new Cube(glm::vec3(0, 1.5, -10));
	ceiling->setTexture(*ceiling_tex);
	ceiling->setScale(glm::vec3(3, 0, 20));
	objects.push_back(ceiling);

	// ǽ��
	Cube* wall_1 = new Cube(glm::vec3(1.5, 0, -8));
	wall_1->setScale(glm::vec3(0, 3, 24));
	wall_1->setTexture(*wall_tex);
	objects.push_back(wall_1);
	Cube* wall_2 = new Cube(glm::vec3(-1.5, 0, -8));
	wall_2->setScale(glm::vec3(0, 3, 24));
	wall_2->setTexture(*wall_tex);
	objects.push_back(wall_2);
	Cube* wall_3 = new Cube(glm::vec3(0, 0, -20));
	wall_3->setScale(glm::vec3(3, 3, 0));
	wall_3->setTexture(*wall_tex);
	objects.push_back(wall_3);

	//// ׼��
	//Cube* aim = new Cube(glm::vec3(0, 0, 0));
	//aim->setTexture(*aim_tex);
	//aim->setScale(glm::vec3(1, 1, 0));
	//objects.push_back(aim);

	// ����
	Cube* shelter_1 = new Cube(glm::vec3(0, -1, -2));
	shelter_1->setTexture(*cube_tex);
	objects.push_back(new Shelter(*shelter_1, 10));
	Cube* shelter_2 = new Cube(glm::vec3(-1, -1, -2));
	shelter_2->setTexture(*cube_tex);
	objects.push_back(new Shelter(*shelter_2, 10));
	Cube* shelter_3 = new Cube(glm::vec3(1, -1, -2));
	shelter_3->setTexture(*cube_tex);
	objects.push_back(new Shelter(*shelter_3, 10));

	// ����
	Cube* trap_1 = new Cube(glm::vec3(0, -1.2, -20));
	trap_1->setTexture(*cube_tex);
	objects.push_back(new Trap(*trap_1, 10));
	Cube* machine_1 = new Cube(glm::vec3(0, 1, -20));
	machine_1->setTexture(*cube_tex);
	objects.push_back(new Machine(*machine_1, 10, ceiling_tex));

	camera.MovementSpeed = 5.f;

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		// ֡������
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ��������
		processInput(window, character);

		// ���½�ɫ״̬��λ��
		character.update(deltaTime);

		// ���¶���
		character.animator->UpdateAnimation(deltaTime);

		// ��Ⱦ����
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// ģ����ɫ�����ò���
		modelShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("view", view);

		auto transforms = character.animator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			modelShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

		// ��Ⱦ�Ѽ���ģ��
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, -0.5f));	// it's a bit too big for our scene, so scale it down
		modelShader.setMat4("model", model);
		character.model->Draw(modelShader);



		// ������ɫ������Ⱦ��ɫ
		shader.use();


		int viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		//camera.SwitchMode(FREE);



		// ����Objects
		for (auto& obj : objects)
		{

			// ������
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, obj->getTexture());

			glm::mat4 model;
			model = glm::translate(model, obj->getPos());
			model = glm::rotate(model, glm::radians(obj->getRot().x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(obj->getRot().y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(obj->getRot().z), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, obj->getScale());


			// �򶥵���ɫ������model����
			int modelLoc = glGetUniformLocation(shader.ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			// ��Ƭ����ɫ��������ɫ
			glm::vec4 color = obj->getColor();
			int colorLoc = glGetUniformLocation(shader.ID, "color");
			glUniform4f(colorLoc, color.x, color.y, color.z, color.w);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			// ���������ƶ�
			obj->setPos(obj->getPos() + obj->getVelocity() * deltaTime);

			// �������λ��
			if (dynamic_cast<Trap*>(obj) || dynamic_cast<Bullet*>(obj))
			{
				//cout << "trap" << endl;
				glm::vec3 pos = obj->getPos();
				if (pos.z > 0)
				{
					obj->_delete = true;
					/*if (abs(pos.x - character.getPosition().x) < 0.01f)
					{
						character.onHit();
					}*/
				}
			}
			else if (dynamic_cast<Machine*>(obj))
			{
				//cout << "machine" << endl;
				((Machine*)obj)->Shoot(objects);
			}
		}

		// ����Objectsɾ��
		for (auto& obj : objects)
		{
			if (obj->_delete)
			{
				remove_object(obj);
				break;
			}
		}



		character.render(shader);  // ���� Shader ���������


		// �������壬�����¼�
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

	// ���ǲ�����ֻ�ڰ���ʱ����һ��
	W_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	S_pressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	A_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	D_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	R_pressed = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
	shoot_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;

	if (W_pressed && !prev_W_pressed)
	{
		character.processInput('W', true); // ����W���������ֵ'W'��true
	}
	if (!W_pressed && prev_W_pressed)
	{
		character.processInput('W', false); // �ɿ�W���������ֵ'W'��false
	}

	if (S_pressed && !prev_S_pressed)
	{
		character.processInput('S', true); // ����S���������ֵ'S'��true
	}
	if (!S_pressed && prev_S_pressed)
	{
		character.processInput('S', false); // �ɿ�S���������ֵ'S'��false
	}

	if (A_pressed && !prev_A_pressed)
	{
		character.processInput('A', true); // ����A���������ֵ'A'��true
	}
	if (!A_pressed && prev_A_pressed)
	{
		character.processInput('A', false); // �ɿ�A���������ֵ'A'��false
	}

	if (D_pressed && !prev_D_pressed)
	{
		character.processInput('D', true); // ����D���������ֵ'D'��true
	}
	if (!D_pressed && prev_D_pressed)
	{
		character.processInput('D', false); // �ɿ�D���������ֵ'D'��false
	}

	if (R_pressed && !prev_R_pressed)
	{
		character.processInput('R', true); // ����R���������ֵ'R'��true
	}
	if (!R_pressed && prev_R_pressed)
	{
		character.processInput('R', false); // �ɿ�R���������ֵ'R'��false
	}

	if (shoot_pressed && !prev_shoot_pressed)
	{

	}
	if (!shoot_pressed && prev_shoot_pressed)
	{

	}

	// ���浱ǰ֡�İ���״̬�Թ���һ֡ʹ��
	prev_W_pressed = W_pressed;
	prev_S_pressed = S_pressed;
	prev_A_pressed = A_pressed;
	prev_D_pressed = D_pressed;
	prev_R_pressed = R_pressed;

	prev_shoot_pressed = shoot_pressed;

	// ������ƶ�
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	// �������
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		camera.ProcessMouseInput(M_LEFT, glm::vec3(0, 0, 0), true, deltaTime);
	else
		camera.ProcessMouseInput(M_LEFT, glm::vec3(0, 0, 0), false, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		camera.ProcessMouseInput(M_RIGHT, glm::vec3(0, 0, 0), true, deltaTime);
	else
		camera.ProcessMouseInput(M_RIGHT, glm::vec3(0, 0, 0), false, deltaTime);



	// ���Լ�
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		character.processInput('T', true);
}
