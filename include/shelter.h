#ifndef SHELTER_H
#define SHELTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cube.h"

#pragma once
class Shelter : public Cube {
  public:
    Shelter(Cube &cube, int health) : Cube(cube) {
        this->health = health;
    }

    void SetHealth(int h) {
        health = h;
    }
    int GetHealth() {
        return health;
    }

    void TakeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            this->_delete = true;
        }
    }

  private:
    int health;
};

#endif