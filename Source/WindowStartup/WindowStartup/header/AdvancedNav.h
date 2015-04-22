#ifndef ADVANCEDNAV_H
#define ADVANCEDNAV_H

#include "Application.h"

class AdvancedNav
{
public:
	AdvancedNav();
	~AdvancedNav();

	int loadShader(unsigned int type, const char* path);
	void CreateShader();
	void CreateBuffers();
	void Draw(glm::mat4 &projectview);


private:
	FBXFile* m_sponza;
};



#endif