#ifndef TRAP_H
#define TRAP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cube.h"

#pragma once
class Trap :
    public Cube
{
public:
    Trap(Cube& cube, int health) : Cube(cube)
    {
        this->health = health;
        this->velocity = glm::vec3(0, 0, 5);
        this->localScale = glm::vec3(1, 0.2f, 0.2f);
        this->color = glm::vec4(0.8, 0, 0, 1);
    }

    void SetHealth(int h) { health = h; }
    int GetHealth() { return health; }

    void TakeDamage(int damage)
    {
        health -= damage;
        if (health <= 0)
        {
            this->_delete = true;
        }
    }

private:
    int health;
};

#endif
