#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
public:
	Texture();
	Texture(const char* path);

	unsigned int getTexture();
	void setTexture(const char* path);

private:
	unsigned int id;
	const char* path;
};

#endif
