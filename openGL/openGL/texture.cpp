#include "texture.h"

#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include "stb_image.h"

#include <iostream>

Texture::Texture() {};

Texture::Texture(const char* path)
{
	this->path = path;
	setTexture(path);
}

unsigned int Texture::getTexture()
{
	return this->id;
}
void Texture::setTexture(const char* path)
{
    // ��������
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // Ϊ��ǰ�󶨵������������û��ơ����˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ���ز���������
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