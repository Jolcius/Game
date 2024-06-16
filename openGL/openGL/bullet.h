#ifndef BULLET_H
#define BULLET_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cube.h"

class Bullet :
    public Cube
{
public:
    Bullet(Cube& cube, int health) : Cube(cube)
    {
        this->health = health;
        this->velocity = glm::vec3(0, 0, 5);
        this->localScale = glm::vec3(0.3f, 0.3f, 0);
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