#ifndef MACHINE_H
#define MACHINE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "bullet.h"
#include "cube.h"
#include "texture.h"

class Machine : public Cube {
  public:
    Machine(Cube &cube, int health, Texture_cube *bullet_tex) : Cube(cube) {
        this->health = health;
        this->localScale = glm::vec3(0.5f, 0.5f, 0.5f);
        this->bullet_tex = bullet_tex;
        timer = 0;
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

    void Shoot(std::vector<Cube *> &vec) {
        timer++;
        if (timer >= 600) {
            timer = 0;
        } else if (timer == 100) {
            cout << "machine shoot" << endl;
            Cube *bullet = new Cube(position);
            bullet->setTexture(*bullet_tex);
            vec.push_back(new Bullet(*bullet, 10));
        } else if (timer == 200) {
            cout << "machine shoot" << endl;
            Cube *bullet = new Cube(position);
            bullet->setTexture(*bullet_tex);
            vec.push_back(new Bullet(*bullet, 10));
        } else if (timer == 300) {
            cout << "machine shoot" << endl;
            Cube *bullet = new Cube(position);
            bullet->setTexture(*bullet_tex);
            vec.push_back(new Bullet(*bullet, 10));
        }
    }

  private:
    int health;
    int timer;
    Texture_cube *bullet_tex;
};

#endif