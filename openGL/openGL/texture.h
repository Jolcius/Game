#ifndef TEXTURE_H
#define TEXTURE_H

class Texture_cube
{
public:
	Texture_cube();
	Texture_cube(const char* path);

	unsigned int getTexture();
	void setTexture(const char* path);

private:
	unsigned int id;
	const char* path;
};

#endif
