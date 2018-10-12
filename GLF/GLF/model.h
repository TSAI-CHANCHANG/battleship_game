#ifndef GLF_MODEL
#define GLF_MODEL
#include "main.h"
#include <atlimage.h>

class Bitmap {
public:
	int sizeX, sizeY;
	unsigned char *data;
};

class Element {
protected:
	vector<float> color;
	vector<float> normal;

	GLuint vao, svao;
	GLuint vboHandles[3],
		positionBufferHandle, colorBufferHandle, normalBufferHandle;
	GLuint spositionBufferHandle;

	struct Material {
		string name;
		glm::vec3 kd, ka, ks;
	};
	vector<Material> surface;
	int active = -1;

	glm::vec3 kd, ka;
	float ks;
	bool diy = false;
	void pushPos(float pos1 = 0, float pos2 = 0, float pos3 = 0) {
		pos.push_back(pos1);
		pos.push_back(pos2);
		pos.push_back(pos3);
	}
	void pushColor(float color1 = 1, float color2 = 1, float color3 = 1) {
		color.push_back(color1);
		color.push_back(color2);
		color.push_back(color3);
	}
	void pushNormal(float normal1, float normal2, float normal3) {
		normal.push_back(normal1);
		normal.push_back(normal2);
		normal.push_back(normal3);
	}
	float *getPos() {
		return toArray<float>(&pos);
	}
	float *getColor() {
		return toArray<float>(&color);
	}
	float *getNormal() {
		return toArray<float>(&normal);
	}
	int pointNum() {
		return pos.size() / 3;
	}

	friend class Mix;

public:
	bool pure = false;
	vector<float> pos;
	glm::mat4 model;
	bool enable = true;
	Element(bool diy = false,
		glm::vec3 d = glm::vec3(0.0, 0.0, 0.0), glm::vec3 a = glm::vec3(0.0, 0.0, 0.0),
		float s = 0) {
		pos.clear();
		color.clear();
		normal.clear();
		glGenVertexArrays(1, &vao);
		glGenVertexArrays(1, &svao);
		glGenBuffers(3, vboHandles);
		positionBufferHandle = vboHandles[0];
		colorBufferHandle = vboHandles[1];
		normalBufferHandle = vboHandles[2];
		glGenBuffers(1, &spositionBufferHandle);

		this->diy = diy;
		kd = d;
		ka = a;
		ks = s;
	}
	~Element() {
		glDeleteVertexArrays(1, &vao);
		glDeleteVertexArrays(1, &svao);
		glDeleteBuffers(3, vboHandles);
		glDeleteBuffers(1, &spositionBufferHandle);
	}
	Element operator= (Element e) {
		if (vao!=e.vao)glDeleteVertexArrays(1, &vao);
		if (svao != e.svao)glDeleteVertexArrays(1, &svao);
		if (positionBufferHandle != e.positionBufferHandle ||
			colorBufferHandle != e.colorBufferHandle ||
			normalBufferHandle != e.normalBufferHandle)glDeleteBuffers(3, vboHandles);
		if (spositionBufferHandle != e.spositionBufferHandle)glDeleteBuffers(1, &spositionBufferHandle);
		
		pos = e.pos;
		color = e.color;
		normal = e.normal;
		surface = e.surface;
		active = e.active;
		kd = e.kd;
		ka = e.ka;
		ks = e.ks;
		model = e.model;
		diy = e.diy;

		vao = e.vao;
		svao = e.svao;
		vboHandles[0] = positionBufferHandle = e.positionBufferHandle;
		vboHandles[1] = colorBufferHandle = e.colorBufferHandle;
		vboHandles[2] = normalBufferHandle = e.normalBufferHandle;
		spositionBufferHandle = e.spositionBufferHandle;

		return e;
	}

	Element *load(const char *filename);
	void shadow();
	void show();
	virtual void timerLoop(){}
	void addBall(float radius, float slice, float R, float G, float B);
};
class Texture {
protected:
	vector<float> pos;
	vector<float> coord;
	vector<float> normal;

	GLuint vao, svao, texName;
	GLuint vboHandles[3],
		positionBufferHandle, coordBufferHandle, normalBufferHandle;
	GLuint spositionBufferHandle;

	glm::vec3 kd, ka;
	float ks;

	bool diy = false;

	void pushPos(float pos1 = 0, float pos2 = 0, float pos3 = 0) {
		pos.push_back(pos1);
		pos.push_back(pos2);
		pos.push_back(pos3);
	}
	void pushCoord(float coord1 = 0.f, float coord2 = 0.f) {
		coord.push_back(coord1);
		coord.push_back(coord2);
	}
	void pushNormal(float normal1, float normal2, float normal3) {
		normal.push_back(normal1);
		normal.push_back(normal2);
		normal.push_back(normal3);
	}
	float *getPos() {
		return toArray<float>(&pos);
	}
	float *getCoord() {
		return toArray<float>(&coord);
	}
	float *getNormal() {
		return toArray<float>(&normal);
	}

	friend class Mix;

	void pic(const char *fileName);
public:
	vector<Bitmap> src;
	bool pure = false;
	glm::mat4 model;
	bool enable = true;
	Texture(bool diy = false,
		glm::vec3 d = glm::vec3(0.0, 0.0, 0.0), glm::vec3 a = glm::vec3(0.0, 0.0, 0.0),
		float s = 0) {
		pos.clear();
		coord.clear();
		normal.clear();
		glGenVertexArrays(1, &vao);
		glGenVertexArrays(1, &svao);
		glGenBuffers(3, vboHandles);
		positionBufferHandle = vboHandles[0];
		coordBufferHandle = vboHandles[1];
		normalBufferHandle = vboHandles[2];
		glGenBuffers(1, &spositionBufferHandle);
		glGenTextures(1, &texName);

		this->diy = diy;
		kd = d;
		ka = a;
		ks = s;
	}
	~Texture() {
		glDeleteVertexArrays(1, &vao);
		glDeleteVertexArrays(1, &svao);
		glDeleteBuffers(3, vboHandles);
		glDeleteBuffers(1, &spositionBufferHandle);
	}

	Texture *load(const char *filename);
	void shadow();
	void show();
	virtual void timerLoop(){}
};
class Mix {
private:
	struct Material {
		string name;
		glm::vec3 kd, ka, ks;
	};
	vector<Material> surface;
	int active = -1;

	glm::vec3 kd, ka;
	float ks;

	bool diy = false;

	void pic(const char *fileName);
public:
	Element element;
	Texture texture;

	vector<Bitmap> src;

	glm::mat4 model;

	Mix(bool diy = false,
		glm::vec3 d = glm::vec3(0.0, 0.0, 0.0), glm::vec3 a = glm::vec3(0.0, 0.0, 0.0),
		float s = 0) {
		this->diy = diy;
		kd = d;
		ka = a;
		ks = s;
	}
	~Mix() {}

	Mix *load(const char *filename);
	void shadow();
	void show();
};

class Scene {
public:
	string name;
	bool active;

	vector<Element*> elements;
	vector<Texture*> textures;
	vector<Mix*> mixes;

	Scene() { active = true; };
	~Scene() {};
	void shadow();
	void show();
};

#endif