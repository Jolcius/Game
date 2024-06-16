#ifndef MACHINE_H
#define MACHINE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

#include "texture.h"
#include "cube.h"
#include "bullet.h"

class Machine :
        public Cube
{
public:
    Machine(Cube& cube, int health, Texture_cube* bullet_tex) : Cube(cube)
    {
        this->health = health;
        this->bullet_tex = bullet_tex;
        timer = 0;
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

    Bullet* Shoot()
    {
        timer++;
        if (timer >= 600)
        {
            timer = 0;
        }
        else if (timer == 100)
        {
            std::cout << "machine shoot 1" << std::endl;
            Cube* bullet = new Cube(position);
            bullet->setTexture(*bullet_tex);
            return new Bullet(*bullet, 10);
        }
        else if (timer == 200)
        {
            std::cout << "machine shoot 2" << std::endl;
            Cube* bullet = new Cube(position);
            bullet->setTexture(*bullet_tex);
            return new Bullet(*bullet, 10);
        }
        else if (timer == 300)
        {
            std::cout << "machine shoot 3" << std::endl;
            Cube* bullet = new Cube(position);
            bullet->setTexture(*bullet_tex);
            return new Bullet(*bullet, 10);
        }
        return nullptr;
    }

private:
    int health;
    int timer;
    Texture_cube* bullet_tex;
};

#endif