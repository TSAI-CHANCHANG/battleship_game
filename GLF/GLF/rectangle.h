#pragma once
#ifndef SHINA_LIB_RECTANGLE_H
#define SHINA_LIB_RECTANGLE_H
#include "main.h"
#include "shader.h"
#include <atlimage.h>
extern Shader *texParticleShader;

class rectangle {

	class Bitmap {
	public:
		int sizeX, sizeY;
		unsigned char *data;
	}src;

	GLuint vao, texName;
	GLuint vboHandles[2],
		positionBufferHandle, coordBufferHandle;

	GLfloat *getPos() {
		return toArray<GLfloat>(&pos);
	}
	void pushPos(float pos1 = 0, float pos2 = 0, float pos3 = 0) {
		pos.push_back(pos1);
		pos.push_back(pos2);
		pos.push_back(pos3);
	}
	void pushCoord(float coord1 = 0.f, float coord2 = 0.f) {
		coord.push_back(coord1);
		coord.push_back(coord2);
	}
	void pic(const char *fileName) {
		CImage *img = new CImage;
		if (!fileName) {
			return;
		}
		HRESULT hr = img->Load(fileName);
		if (!SUCCEEDED(hr)) {
			return;
		}
		src.sizeX = img->GetWidth();
		src.sizeY = img->GetHeight();
		if (img->GetPitch()<0)src.data = (unsigned char *)img->GetBits() + (img->GetPitch()*(img->GetHeight() - 1));
		else src.data = (unsigned char *)img->GetBits();
	}
	float *getCoord() {
		return toArray<float>(&coord);
	}

public:
	bool enable;
	glm::vec4 color;
	vector<GLfloat> pos;
	vector<GLfloat> coord;
	glm::vec3 position=glm::vec3(0,0,0);
	float a, b;
	glm::mat4 model;
	rectangle(){}

	void setShape(float a, float b)
	{
		this->a = a;
		this->b = b;
	}

	void fill(string filename)
	{
		pic(filename.c_str());
		glGenVertexArrays(1, &vao);
		glGenBuffers(2, vboHandles);
		positionBufferHandle = vboHandles[0];
		coordBufferHandle = vboHandles[1];
		glGenTextures(1, &texName);


		pos.clear();
		coord.clear();
		pushPos(position.x - a / 2, position.y - b / 2, position.z);
		pushPos(position.x + a / 2, position.y - b / 2, position.z);
		pushPos(position.x + a / 2, position.y + b / 2, position.z);
		pushPos(position.x + a / 2, position.y + b / 2, position.z);
		pushPos(position.x - a / 2, position.y + b / 2, position.z);
		pushPos(position.x - a / 2, position.y - b / 2, position.z);

		pushCoord(0, 0);
		pushCoord(1, 0);
		pushCoord(1, 1);
		pushCoord(1, 1);
		pushCoord(0, 1);
		pushCoord(0, 0);
	}
	void show()
	{
		if (enable == false)return;
		texParticleShader->use();
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
		glBufferData(GL_ARRAY_BUFFER, pos.size() * 4, getPos(), GL_STREAM_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
		glBindBuffer(GL_ARRAY_BUFFER, coordBufferHandle);
		glBufferData(GL_ARRAY_BUFFER, coord.size() * 4, getCoord(), GL_STREAM_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);

		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, src.sizeX, src.sizeY, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, src.data);


		texParticleShader->setInt("u_textureMap", 1);
		texParticleShader->setMat4("u_modelMatrix", model);
		texParticleShader->setVec4("v_color", color);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, pos.size() / 3);
		
	}

	inline void loadIdentity()
	{
		this->model = glm::mat4();
	}

	inline void translate(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::translate(this->model, glm::vec3(x, y, z));
	}
	inline void scale(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::scale(this->model, glm::vec3(x, y, z));
	}
	inline void rotate(GLfloat angel, glm::vec3 axis)
	{
		this->model = glm::rotate(this->model, angel, axis);
	}
};



#endif