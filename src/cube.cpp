#include "cube.h"

Cube::Cube()
{
	this->position = glm::vec3(0, 0, 0);
	this->rotation = glm::vec3(0, 0, 0);
	this->localScale = glm::vec3(1, 1, 1);
	this->color = glm::vec4(1, 1, 1, 1);
}
Cube::Cube(glm::vec3 pos)
{
	this->position = pos;
	this->rotation = glm::vec3(0, 0, 0);
	this->localScale = glm::vec3(1, 1, 1);
	this->color = glm::vec4(1, 1, 1, 1);
}
Cube::Cube(glm::vec3 pos, glm::vec3 rot)
{
	this->position = pos;
	this->rotation = rot;
	this->localScale = glm::vec3(1, 1, 1);
	this->color = glm::vec4(1, 1, 1, 1);
}
Cube::Cube(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	this->position = pos;
	this->rotation = rot;
	this->localScale = scale;
	this->color = glm::vec4(1, 1, 1, 1);
}