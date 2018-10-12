#include "main.h"
enum particleType {
	RAIN,
	BLAST,
	CLOUD,
	FIRE,
	SNOW,
	SHIP
};

class particle {
public:
	GLfloat life = 0;
	GLfloat fullLife;
	GLfloat fade;
	glm::vec3 RGB;

	glm::vec3 pos, vel, acc, arc;
};
class generator {
public:
	GLenum particleType;
	GLuint particleCount;
	particle *particles;
	GLuint colorCount;
	GLuint(*colorList)[3];
	int flag;
	GLfloat baseX, baseY, baseZ;
	GLfloat particleLife;
	GLfloat particleSize;
public:
	virtual void genersInit(GLfloat baseX, GLfloat baseY, GLfloat baseZ) = 0;
	virtual void particleInit(int i) = 0;
	virtual void particleUpdate(int i) = 0;
	void particleDisplay(int i);
	void particleLoop();
};
class blast : public generator {
public:
	blast(GLfloat baseX, GLfloat baseY, GLfloat baseZ);
	~blast();
	virtual void genersInit(GLfloat baseX, GLfloat baseY, GLfloat baseZ);
	virtual void particleInit(int i);
	virtual void particleUpdate(int i);
};
class fire : public generator {
public:
	fire(GLfloat baseX, GLfloat baseY, GLfloat baseZ);
	~fire();
	virtual void genersInit(GLfloat baseX, GLfloat baseY, GLfloat baseZ);
	virtual void particleInit(int i);
	virtual void particleUpdate(int i);
};
class shipPart : public generator {
public:
	shipPart(GLfloat baseX, GLfloat baseY, GLfloat baseZ);
	~shipPart();
	virtual void genersInit(GLfloat baseX, GLfloat baseY, GLfloat baseZ);
	virtual void particleInit(int i);
	virtual void particleUpdate(int i);
};
class systemp {
private:
	int generCount;
	generator ** geners;
public:
	systemp() {}
	systemp(int *xpos, int *ypos, int *zpos, int SYSTYPE, int n);
	~systemp();
	void systemLoop();
	void systemInit(int xpos, int ypos, int zpos, int SYSTYPE,int n);
};
