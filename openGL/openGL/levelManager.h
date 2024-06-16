#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "texture.h"
#include "cube.h"
#include "trap.h"
#include "machine.h"

#include <vector>
#include <iostream>
#include <random>

using namespace std;

class LevelManager
{
public:
	enum Difficulty
	{
		Human,
		DevilHunter,
		DanteMustDie,
		HeavenandHell,
	};

	LevelManager(vector<Cube*>& vec)
	{
		objects = &vec;

		cube_tex = new Texture_cube("textures/container.jpg");
		ceiling_tex = new Texture_cube("textures/ceiling.jpg");
		enemy_tex = new Texture_cube("textures/enemy.png");
		wall_tex = new Texture_cube("textures/wall.jpg");
		bucket_tex = new Texture_cube("textures/bucket.png");
		machine_tex = new Texture_cube("textures/machine.png");
		bullet_tex = new Texture_cube("textures/bullet.png");

		timer = 0;
		mode = 0;

		diff = Human;
		max_enemy_size = 10;
	}

	Shelter* shelter;
	int shelter_x_pos;	// 掩体的x轴位置

	void InitScene()
	{
		// 地面和天花板
		glm::vec3 ground_pos = glm::vec3(0, -1.5, -10);
		Cube* ground = new Cube(ground_pos);
		ground->setTexture(*cube_tex);
		ground->setScale(glm::vec3(3, 0, 20));
		objects->push_back(ground);
		Cube* ceiling = new Cube(glm::vec3(0, 1.5, -10));
		ceiling->setTexture(*ceiling_tex);
		ceiling->setScale(glm::vec3(3, 0, 20));
		objects->push_back(ceiling);

		// 墙壁
		Cube* wall_1 = new Cube(glm::vec3(1.5, 0, -8));
		wall_1->setScale(glm::vec3(0, 3, 24));
		wall_1->setTexture(*wall_tex);
		objects->push_back(wall_1);
		Cube* wall_2 = new Cube(glm::vec3(-1.5, 0, -8));
		wall_2->setScale(glm::vec3(0, 3, 24));
		wall_2->setTexture(*wall_tex);
		objects->push_back(wall_2);
		Cube* wall_3 = new Cube(glm::vec3(0, 0, -20));
		wall_3->setScale(glm::vec3(3, 3, 0));
		wall_3->setTexture(*wall_tex);
		objects->push_back(wall_3);

		// 掩体
		shelter_x_pos = rand() % 3 - 1;
		Cube* shelt = new Cube(glm::vec3(shelter_x_pos, -1, -2));
		shelt->setTexture(*cube_tex);
		shelter = new Shelter(*shelt, 10);
		objects->push_back(shelter);
	}

	void GenerateEnemy()
	{
		if (objects->size() >= 15) return;
		if (timer == 100)
		{
			mode = rand() % 4;
			switch (mode)
			{
			case 0:
			{
				// 生成一个滚筒
				int x_pos = rand() % 3 - 1;

				Cube* trap = new Cube(glm::vec3(x_pos, -1.2, -20));
				trap->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap, 10));
				break;
			}
			case 1:
			{
				// 生成一个炮台
				int x_pos = rand() % 3 - 1;
				int y_pos = rand() % 2 * -1.2f;

				Cube* machine = new Cube(glm::vec3(x_pos, y_pos, -20));
				machine->setTexture(*machine_tex);
				machine->setScale(glm::vec3(0.75f, 0.75f, 0.75f));
				objects->push_back(new Machine(*machine, 10, bullet_tex));
				break;
			}
			case 2:
			{
				// 生成一排滚筒
				int y_pos = rand() % 2 * -1.2f;
				Cube* trap_1 = new Cube(glm::vec3(-1, y_pos, -20));
				trap_1->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap_1, 10));
				Cube* trap_2 = new Cube(glm::vec3(0, y_pos, -20));
				trap_2->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap_2, 10));
				Cube* trap_3 = new Cube(glm::vec3(1, y_pos, -20));
				trap_3->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap_3, 10));
				break;
			}
			case 3:
			{
				// 生成一个敌人
				int z_pos = -20 + rand() % 10;
				int x_pos = rand() % 3 - 1;

				Cube* enemy = new Cube(glm::vec3(x_pos, -0.8f, z_pos));
				enemy->setScale(glm::vec3(1, 1.5f, 0));
				enemy->setTexture(*enemy_tex);
				objects->push_back(new Machine(*enemy, 10, bullet_tex));
				break;
			}
			}
		}
		else if(timer == 200)
		{
			switch (mode)
			{
			case 0:
			{
				// 生成一个滚筒
				int x_pos = rand() % 3 - 1;

				Cube* trap = new Cube(glm::vec3(x_pos, -1.2, -20));
				trap->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap, 10));
			}
			case 3:
			{
				// 生成一个敌人
				int z_pos = -20 + rand() % 10;
				int x_pos = rand() % 3 - 1;

				Cube* enemy = new Cube(glm::vec3(x_pos, -0.8f, z_pos));
				enemy->setScale(glm::vec3(1, 1.5f, 0));
				enemy->setTexture(*enemy_tex);
				objects->push_back(new Machine(*enemy, 10, bullet_tex));
				break;
			}
			}
		}
		else if (timer == 300)
		{
			switch (mode)
			{
			case 0:
			{
				// 生成一个滚筒
				int x_pos = rand() % 3 - 1;

				Cube* trap = new Cube(glm::vec3(x_pos, -1.2, -20));
				trap->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap, 10));
			}
			case 2:
			{
				// 生成一排滚筒
				int y_pos = rand() % 2 * -1.2f;
				Cube* trap_1 = new Cube(glm::vec3(-1, y_pos, -20));
				trap_1->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap_1, 10));
				Cube* trap_2 = new Cube(glm::vec3(0, y_pos, -20));
				trap_2->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap_2, 10));
				Cube* trap_3 = new Cube(glm::vec3(1, y_pos, -20));
				trap_3->setTexture(*bucket_tex);
				objects->push_back(new Trap(*trap_3, 10));
				break;
			}
			case 3:
			{
				// 生成一个敌人
				int z_pos = -20 + rand() % 10;
				int x_pos = rand() % 3 - 1;

				Cube* enemy = new Cube(glm::vec3(x_pos, -0.8f, z_pos));
				enemy->setScale(glm::vec3(1, 1.5f, 0));
				enemy->setTexture(*enemy_tex);
				objects->push_back(new Machine(*enemy, 10, bullet_tex));
				break;
			}
			}
			timer = 0;
		}
		timer++;
	}

	// 设置游戏难度
	void SetDifficulty(Difficulty diff)
	{
		this->diff = diff;
		if (diff == Human) max_enemy_size = 10;
		else if (diff == DevilHunter) max_enemy_size = 20;
		else if (diff == DanteMustDie) max_enemy_size = 30;
		else if (diff == HeavenandHell) max_enemy_size = 2147483647;
	}

private:
	Difficulty diff;
	int max_enemy_size;	//场景中最大敌人数量

	Texture_cube* cube_tex;
	Texture_cube* ceiling_tex;
	Texture_cube* enemy_tex;
	Texture_cube* wall_tex;
	Texture_cube* machine_tex;
	Texture_cube* bucket_tex;
	Texture_cube* bullet_tex;

	vector<Cube*>* objects;

	int timer;
	int mode;		// 机关陷阱敌人生成模式
};

#endif