#include "texture.h"

#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include "stb_image.h"

#include <iostream>

Texture_cube::Texture_cube() {};

Texture_cube::Texture_cube(const char* path)
{
	this->path = path;
	setTexture(path);
}

unsigned int Texture_cube::getTexture()
{
	return this->id;
}
void Texture_cube::setTexture(const char* path)
{
    // 生成纹理
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}