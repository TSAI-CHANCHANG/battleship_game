#include "particle.h"
#include "eye.h"
#include "model.h"
#include "main.h"
#include "rectangle.h"
#define CALCDIST(x1, y1, z1, x2, y2, z2) (sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2)))

extern Eye *eye;
extern float randNumber[10000];
extern glm::vec3 initialColor;
extern glm::vec3 fadeColor;
extern rectangle *r[FIRENUM];
extern vector<Scene*> globe;
const int rainColor = 10, blastColor = 10;
GLuint colorArray1[blastColor][3] = {
	255,20,0,
	255,50,0,
	255,80,0,
	255,100,0,
	255,120,0,
	255,150,0,
	255,200,0,
	255,220,0,
	255,250,0,
	255,255,0
};

void generator::particleDisplay(int i) {
	if (particles[i].life <= 0) { 
		switch (particleType) {
		case BLAST:
			globe[0]->elements[i]->enable = false;
			return;
		case FIRE:
			r[i]->enable = false;
			particleInit(i);
			r[i]->color.a = 1;
			break;
		case SHIP:
			globe[0]->elements[401+i]->enable = false;
			break;
		}
	}
	//glColor4f(particles[i].RGB.x, particles[i].RGB.y, particles[i].RGB.z, particles[i].life / particleLife);
	//glPointSize(particleSize);
	switch (particleType) {
	case BLAST:
		globe[0]->elements[i]->enable = true;
		globe[0]->elements[i]->model = glm::translate(glm::mat4x4(), glm::vec3(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z));
		break;
	case FIRE:
		r[i]->enable = true;
		r[i]->model = glm::translate(glm::mat4x4(), glm::vec3(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z));
		r[i]->model = glm::rotate(r[i]->model, particles[i].arc.x, glm::vec3(1, 0, 0));
		r[i]->model = glm::rotate(r[i]->model, particles[i].arc.y, glm::vec3(0, 1, 0));
		r[i]->model = glm::rotate(r[i]->model, particles[i].arc.z, glm::vec3(0, 0, 1));
		r[i]->color.r = interpolate<float>(particles[i].life / particles[i].fullLife, initialColor.r, fadeColor.r);
		r[i]->color.g = interpolate<float>(particles[i].life / particles[i].fullLife, initialColor.g, fadeColor.g);
		r[i]->color.b = interpolate<float>(particles[i].life / particles[i].fullLife, initialColor.b, fadeColor.b);
		break;
	case SHIP:
		globe[0]->elements[401 + i]->enable = true;
		globe[0]->elements[401 + i]->model = glm::translate(glm::mat4x4(), glm::vec3(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z));
		globe[0]->elements[401 + i]->model = glm::rotate(globe[0]->elements[401 + i]->model, particles[i].arc.x, glm::vec3(1, 0, 0));
		globe[0]->elements[401 + i]->model = glm::rotate(globe[0]->elements[401 + i]->model, particles[i].arc.y, glm::vec3(0, 1, 0));
		globe[0]->elements[401 + i]->model = glm::rotate(globe[0]->elements[401 + i]->model, particles[i].arc.z, glm::vec3(0, 0, 1));
		break;
	}
}
void generator::particleLoop() {
	for (int i = 0; i < particleCount; i++) {
		particleDisplay(i);
		particleUpdate(i);
	}
}
blast::blast(GLfloat baseX, GLfloat baseY, GLfloat baseZ) {
	this->genersInit(baseX, baseY, baseZ);
}
blast::~blast() {
	delete[] particles;
}
void blast::genersInit(GLfloat baseX, GLfloat baseY, GLfloat baseZ)
{
	glm::vec3 tmpPos = eye->pos;
	this->baseX = baseX;
	this->baseY = baseY;
	this->baseZ = baseZ;
	particleType = BLAST;
	particleCount = 400;
	if (flag == 0) {
		particles = new particle[particleCount];
		flag = 1;
	}
	colorList = colorArray1;
	colorCount = rainColor;
	//GLfloat dist = CALCDIST(baseX, baseY, baseZ, tmpPos.x, tmpPos.y, tmpPos.z);
	particleSize = 15;
	particleLife = 10;
	for (int i = 0; i < particleCount; i++)particleInit(i);
}
void blast::particleInit(int i) {
	globe[0]->elements[i]->model = glm::translate(glm::mat4x4(), glm::vec3(0 - particles[i].pos.x, 0 - particles[i].pos.y, 0 - particles[i].pos.z));
	float xRange = 5;
	float yRange = 10;
	float zRange = 5;
	int random = rand() % 10000;
	particles[i].life = 0.5 + particleLife*(randNumber[random]* 50) * 0.01f;
	particles[i].fade = 0.2f;

	particles[i].pos.x = (float)(xRange / 2 - xRange* randNumber[random * 2 % 10000]) + baseX;
	particles[i].pos.y = (float)(yRange / 2 - xRange* randNumber[random * 3 % 10000]) + baseY;
	particles[i].pos.z = (float)(zRange / 2 - zRange* randNumber[random * 4 % 10000]) + baseZ;
	particles[i].vel.x = (particles[i].pos[0] - baseX);
	particles[i].vel.y = 2*(particles[i].pos[1] - baseY);
	particles[i].vel.z = (particles[i].pos[2] - baseZ);
	particles[i].acc.x = -(particles[i].pos[0] - baseX) / 40;
	particles[i].acc.y = -(particles[i].pos[1] - baseY) / 10;
	particles[i].acc.z = -(particles[i].pos[2] - baseZ) / 40;
}
void blast::particleUpdate(int i) {
	if (particles[i].life < 0)return;
	particles[i].pos.x += particles[i].vel.x;
	particles[i].pos.y += particles[i].vel.y;
	particles[i].pos.z += particles[i].vel.z;
	particles[i].vel.x += particles[i].acc.x;
	particles[i].vel.y += particles[i].acc.y;
	particles[i].vel.z += particles[i].acc.z;
	particles[i].life -= particles[i].fade;
}
fire::fire(GLfloat baseX, GLfloat baseY, GLfloat baseZ)
{
	this->genersInit(baseX, baseY, baseZ);
}

void fire::genersInit(GLfloat baseX, GLfloat baseY, GLfloat baseZ)
{
	this->baseX = baseX;
	this->baseY = baseY;
	this->baseZ = baseZ;
	particleType = FIRE;
	particleCount = FIRENUM;
	if (flag == 0) {
		particles = new particle[particleCount];
		flag = 1;
	}
	particleLife = 10;
	for (int i = 0; i < particleCount; i++)particleInit(i);
}

void fire::particleInit(int i)
{
	//r[i]->model = glm::translate(glm::mat4x4(), glm::vec3(0 - particles[i].pos.x, 0 - particles[i].pos.y, 0 - particles[i].pos.z));
	int random = rand() % 10000;
	particles[i].life = randNumber[random] * 15;
	particles[i].fade = 0.05f;
	particles[i].fullLife = particles[i].life;

	particles[i].pos.x = (randNumber[(random * 2) % 10000] - 0.15) * 160 + baseX;
	particles[i].pos.y = baseY;
	particles[i].pos.z = (randNumber[(random * 3) % 10000] - 0.15) * 6 + baseZ;
	particles[i].vel.x = 0;
	particles[i].vel.y = randNumber[(random * 4) % 10000] * 9;
	particles[i].vel.z = 0;
	particles[i].acc.x = 0;
	particles[i].acc.y = -0.06;
	particles[i].acc.z = 0;
	particles[i].arc.x = randNumber[(random * 5) % 10000];
	particles[i].arc.y = randNumber[(random * 6) % 10000];
	particles[i].arc.z = randNumber[(random * 7) % 10000];
}

void fire::particleUpdate(int i)
{
	if (particles[i].life < 0)return;
	particles[i].pos.x += particles[i].vel.x * particles[i].fade;
	particles[i].pos.y += particles[i].vel.y * particles[i].fade;
	particles[i].pos.z += particles[i].vel.z * particles[i].fade;
	particles[i].vel.x += particles[i].acc.x * particles[i].fade;
	particles[i].vel.y += particles[i].acc.y * particles[i].fade;
	particles[i].vel.z += particles[i].acc.z * particles[i].fade;
	particles[i].life -= particles[i].fade;
}
systemp::systemp(int *xpos, int *ypos, int *zpos, int SYSTYPE, int n) {
	generCount = n;
	switch (SYSTYPE) {
	case BLAST:
		geners = (generator**)new generator*[n];
		for (int i = 0; i < n; i++) {
			geners[i] = new blast((float)xpos[i], (float)ypos[i], (float)zpos[i]);
			geners[i]->flag = 0;
		}
		break;
	case FIRE:
		geners = (generator**)new generator*[n];
		for (int i = 0; i < n; i++) {
			geners[i] = new fire((float)xpos[i], (float)ypos[i], (float)zpos[i]);
			geners[i]->flag = 0;
		}
		break;
	case SHIP:
		geners = (generator**)new generator*[n];
		for (int i = 0; i < n; i++) {
			geners[i] = new shipPart((float)xpos[i], (float)ypos[i], (float)zpos[i]);
			geners[i]->flag = 0;
		}
		break;
	}
}
systemp::~systemp() {

}
void systemp::systemLoop() {
	for (int i = 0; i<generCount; i++)geners[i]->particleLoop();
}

void systemp::systemInit(int xpos, int ypos, int zpos, int SYSTYPE, int n)
{
	switch (SYSTYPE) {
	case BLAST:
		geners[n]->genersInit((float)xpos, (float)ypos, (float)zpos);
		break;
	case FIRE:
		geners[n]->genersInit((float)xpos, (float)ypos, (float)zpos);
		break;
	case SHIP:
		geners[n]->genersInit((float)xpos, (float)ypos, (float)zpos);
		break;
	}
}

shipPart::shipPart(GLfloat baseX, GLfloat baseY, GLfloat baseZ)
{
	this->genersInit(baseX, baseY, baseZ);
}

shipPart::~shipPart()
{
}

void shipPart::genersInit(GLfloat baseX, GLfloat baseY, GLfloat baseZ)
{
	glm::vec3 tmpPos = eye->pos;
	this->baseX = baseX;
	this->baseY = baseY;
	this->baseZ = baseZ;
	particleType = SHIP;
	particleCount = 2;
	if (flag == 0) {
		particles = new particle[particleCount];
		flag = 1;
	}
	for (int i = 0; i < particleCount; i++)particleInit(i);
}

void shipPart::particleInit(int i)
{
	globe[0]->elements[i]->model = glm::translate(glm::mat4x4(), glm::vec3(0 - particles[i].pos.x, 0 - particles[i].pos.y, 0 - particles[i].pos.z));
	globe[0]->elements[i]->enable = false;
	float xRange = 10;
	float yRange = 30;
	float zRange = 10;
	int random = rand() % 10000;
	particles[i].life = 0.5 + particleLife*(randNumber[random] * 50) * 0.01f;
	particles[i].fade = 0.01f;
	particles[i].pos.x = (float)(xRange / 2 - xRange * randNumber[random * 2 % 10000]) + baseX;
	particles[i].pos.y = (float)(yRange / 2 - xRange * randNumber[random * 3 % 10000]) + baseY;
	particles[i].pos.z = (float)(zRange / 2 - zRange * randNumber[random * 4 % 10000]) + baseZ;
	particles[i].vel.x = (particles[i].pos[0] - baseX);
	particles[i].vel.y = 2 * (particles[i].pos[1] - baseY);
	particles[i].vel.z = (particles[i].pos[2] - baseZ);
	particles[i].acc.x = -(particles[i].pos[0] - baseX) / 40;
	particles[i].acc.y = -(particles[i].pos[1] - baseY) / 5;
	particles[i].acc.z = -(particles[i].pos[2] - baseZ) / 40;
	particles[i].arc.x = randNumber[(random * 5) % 10000];
	particles[i].arc.y = randNumber[(random * 6) % 10000];
	particles[i].arc.z = randNumber[(random * 7) % 10000];
}

void shipPart::particleUpdate(int i)
{
	if (particles[i].life < 0)return;
	particles[i].pos.x += particles[i].vel.x;
	particles[i].pos.y += particles[i].vel.y;
	particles[i].pos.z += particles[i].vel.z;
	particles[i].vel.x += particles[i].acc.x;
	particles[i].vel.y += particles[i].acc.y;
	particles[i].vel.z += particles[i].acc.z;
	particles[i].life -= particles[i].fade;
}
