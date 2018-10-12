#include "main.h"
#include "shader.h"
#include "model.h"
#include "light.h"
#include "eye.h"
#include "calwave.h"
#include "particle.h"
#include "rectangle.h"
#define SPEED 25.0f
#define G_PI 3.14159265358979323846f
#define WAVE_NUM 4
#define WAVE_COL 1
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

#define waveOn 0
#define DEBUG  1
vector<Scene*> globe;
vector<Light*> light;

Eye *eye;
Mouse *mouse;
Keyboard *key;
systemp s_blast;
systemp s_fire;
systemp s_ship;
float randNumber[10000];
extern Shader *elementShader;
extern Shader *texShader;
extern Shader *shadowShader;
extern Shader *texParticleShader;
rectangle *r[FIRENUM];
glm::vec3 initialColor(1.f, 0.996f, 0.3569f);
glm::vec3 fadeColor(0.396f, 0.0824f, 0.04705f);
void renderGlobe() {
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(45.f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 4.f, 32384.0f);
	glm::mat4 view = glm::lookAt(eye->pos, eye->pos + eye->dir, glm::vec3(0.0, 1.0, 0.0));
	elementShader->use();
	elementShader->setMat4("u_projection", projection);
	elementShader->setMat4("u_view", view);
	elementShader->setVec3("u_eyePos", eye->pos);
	for (int i = 0; i < light.size(); i++) {
		char tmp[64];
		sprintf(tmp, "u_lightsMatrix[%d]", i);
		elementShader->setMat4(tmp, light[i]->lightMatrix);
		sprintf(tmp, "u_lightInfo[%d].u_lightType", i);
		elementShader->setInt(tmp, light[i]->type);
		sprintf(tmp, "u_lightInfo[%d].u_lightPos", i);
		elementShader->setVec3(tmp, light[i]->pos);
		sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
		elementShader->setVec3(tmp, light[i]->diffuse);
		sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
		elementShader->setVec3(tmp, light[i]->ambient);
		sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
		elementShader->setFloat(tmp, light[i]->specular);
		sprintf(tmp, "u_lightInfo[%d].u_lightDir", i);
		elementShader->setVec3(tmp, light[i]->direction);
		sprintf(tmp, "u_lightInfo[%d].u_cutOff", i);
		elementShader->setFloat(tmp, light[i]->cutoff);
	}
	texShader->use();
	texShader->setMat4("u_projection", projection);
	texShader->setMat4("u_view", view);
	texShader->setVec3("u_eyePos", eye->pos);
	for (int i = 0; i < light.size(); i++) {
		char tmp[64];
		sprintf(tmp, "u_lightsMatrix[%d]", i);
		texShader->setMat4(tmp, light[i]->lightMatrix);
		sprintf(tmp, "u_lightInfo[%d].u_lightType", i);
		texShader->setInt(tmp, light[i]->type);
		sprintf(tmp, "u_lightInfo[%d].u_lightPos", i);
		texShader->setVec3(tmp, light[i]->pos);
		sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
		texShader->setVec3(tmp, light[i]->diffuse);
		sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
		texShader->setVec3(tmp, light[i]->ambient);
		sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
		texShader->setFloat(tmp, light[i]->specular);
		sprintf(tmp, "u_lightInfo[%d].u_lightDir", i);
		texShader->setVec3(tmp, light[i]->direction);
		sprintf(tmp, "u_lightInfo[%d].u_cutOff", i);
		texShader->setFloat(tmp, light[i]->cutoff);
	}
	texParticleShader->use();
	texParticleShader->setMat4("u_projection", projection);
	texParticleShader->setMat4("u_view", view);
	int ln = light.size();
	for (int i = 0; i < ln; i++) {
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D, light[i]->textureHandle);
	}
	for (auto s : globe) {
		s->show();
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (int i = 0; i < FIRENUM; i++) {
		//r[i]->enable = true;
		r[i]->show();
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
}

static void GLFInit() {
	glewInit();
	glEnable(GL_DEPTH_TEST);
	elementShader = new Shader("shader//element.vert", "shader//element.frag");
	texShader = new Shader("shader//tex.vert", "shader//tex.frag");
	shadowShader = new Shader("shader//shadow.vert", "shader//shadow.frag");
	texParticleShader = new Shader("shader//rectangle.vert", "shader//rectangle.frag");
	
	eye = new Eye(glm::vec3(0.0f, 64.0f, 64.0f));
	mouse = new Mouse();
	key = new Keyboard();
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	for (int i = 0; i < 10000; i++) {
		randNumber[i] = rand() % 10000 * 0.0001;
	}
}
static void GLFDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	elementShader->use();
	elementShader->setInt("u_lightNum", light.size());
	texShader->use();
	texShader->setInt("u_lightNum", light.size());
	for (auto &l : light) {
		l->genShadow();
	}
	renderGlobe();

	glutSwapBuffers();
}
static void GLFReshape(int w, int h) {
	SCR_WIDTH = w;
	SCR_HEIGHT = h;
}
static void GLFMouse(int x, int y) {
	mouse->tmp.x = float(x);
	mouse->tmp.y = float(y);

	glutPostRedisplay();
}
static void GLFDrag(int x, int y) {
	if (mouse->state[GLUT_LEFT_BUTTON] == GLUT_DOWN)
		eye->rotate(float(x - mouse->pre.x) / 200, float(y - mouse->pre.y) / 200);
	if (mouse->state[GLUT_RIGHT_BUTTON] == GLUT_DOWN)
		eye->move(float(x - mouse->pre.x) / 2, float(y - mouse->pre.y) / 2);

	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	glutPostRedisplay();
}
static void GLFClick(int button, int state, int x, int y) {
	mouse->state[button] = state;
	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN) {
		mouse->state[GLUT_LEFT_BUTTON] = GLUT_DOWN;
	}

	glutPostRedisplay();
}
static void GLFWheel(int wheel, int dir, int x, int y) {
	if (dir > 0)eye->zoom(.8f);
	if (dir < 0)eye->zoom(1.2f);

	glutPostRedisplay();
}
static void GLFSpecialDown(int key, int x, int y) {
	if (key == GLF_KEY_CTRL)::key->ctrl = true;
	if (key == GLF_KEY_SHIFT)::key->shift = true;
	if (key == GLF_KEY_ALT)::key->alt = true;
}
static void GLFSpecialUp(int key, int x, int y) {
	if (key == GLF_KEY_CTRL)::key->ctrl = false;
	if (key == GLF_KEY_SHIFT)::key->shift = false;
	if (key == GLF_KEY_ALT)::key->alt = false;
}
static double speed;
static double acceleration = 0;
static float axic = 0;
static int flag = 0;
static int flag2 = 0;
static int flag_hit = 0;
static void GLFKeyDown(unsigned char cAscii, int x, int y) {
	static int p = 0;
	if (cAscii == 'w') {
		acceleration += 0.5;
		if (speed < 10)
			speed += acceleration;
	}
	if (cAscii == 's') {
		acceleration -= 0.5;
		if (speed > -10)
			speed += acceleration;
	}
	if (cAscii == 'a') {
		axic += 0.01;
	}
	if (cAscii == 'd') {
		axic -= 0.01;
	}
	if (cAscii == 'z') {
		flag = 1;
	}
	if (cAscii == 'x') {
		flag = -1;
	}
}
static void GLFKeyUp(unsigned char cAscii, int x, int y) {
	speed = 0;
	acceleration = 0;
}
int x2[1] = { 0 };
int y2[1] = { 0 };
int z2[1] = { 0 };
static void GLFTimer(int p) {
	static double z_moves = 0;
	static double x_moves = 0;
	static double z_moves_p = 0;
	static double x_moves_p = 0;
	static double z_moves_pao = 0;
	static double x_moves_pao = 0;
	static float axic_pao = axic;
	static float axic_pro = 0;
	static int time = 0;
	static int blast_time = 0;
	double g = 0.3;
	double phy = G_PI * 12 / 180;
	glm::mat4x4 trans1;
	z_moves += speed*cos(axic);
	x_moves += speed*sin(axic);
	eye->pos.x -= speed*sin(axic);
	eye->pos.z -= speed*cos(axic);
	glm::mat4x4 trans = glm::translate(glm::mat4x4(), glm::vec3(-x_moves, -240, -z_moves));
	trans = glm::rotate(trans, axic, glm::vec3(0, 1, 0));
	globe[0]->textures[8]->model = trans;

	light[1]->pos = glm::vec3(20.f, 140.f, 20.f);
	glm::mat4x4 trans2 = glm::translate(glm::mat4x4(), glm::vec3(-x_moves, -0, -z_moves));
	trans2 = glm::rotate(trans2, axic, glm::vec3(0, 1, 0));
	glm::vec4 temp1 = trans2 * glm::vec4(light[1]->pos.x, light[1]->pos.y, light[1]->pos.z, 1.0f);
	light[1]->pos = glm::vec3(temp1.x, temp1.y, temp1.z);
	
	light[1]->direction = glm::vec3(0.f, -160.f, -1150.f);
	//trans2 = glm::rotate(trans2, axic, glm::vec3(0, 1, 0));
	glm::vec4 temp2 = trans2 * glm::vec4(light[1]->direction.x, light[1]->direction.y, light[1]->direction.z, 1.0f);
	light[1]->direction = glm::vec3(temp2.x, temp2.y, temp2.z);

	x_moves_pao = x_moves;
	z_moves_pao = z_moves;
	if (flag == 0) {
		time = 0;
		axic_pro = 0;
		globe[0]->elements[400]->enable = false;
	}
	if (flag == 1) {
		flag2 = 0;
		s_blast.systemInit(0, 0, 0, BLAST, 0);
		//s_fire.systemInit(0, 0, 0, FIRE, 0);
		if (time == 0) {
			axic_pro = axic;
			x_moves_p = x_moves;
			z_moves_p = z_moves;
		}
		time += 2;
		globe[0]->elements[400]->enable = true;
		axic_pao = axic_pro;
		x_moves_pao = x_moves_p;
		z_moves_pao = z_moves_p;
	}
	trans1 = glm::translate(trans1, glm::vec3(SPEED * -time*cos(phy)*sin(axic_pao), SPEED * time*sin(phy) - 0.5*time*time*g, SPEED * -time*cos(phy)*cos(axic_pao)));
	trans1 = glm::translate(trans1, glm::vec3(-x_moves_pao, 0, -z_moves_pao));
	trans1 = glm::rotate(trans1, axic, glm::vec3(0, 1, 0));
	trans1 = glm::translate(trans1, glm::vec3(0, 7, -179));
	globe[0]->elements[400]->model = trans1;
	glm::vec4 temp = trans1*glm::vec4(0, 0, 0, 1);
	if (SPEED * time*sin(phy) - 0.5*time*time*g < -38) {
		flag = -1;
	}
	if (SPEED * time*sin(phy) - 0.5*time*time*g < 10 && flag && !flag_hit) {
		glm::vec4 result = trans1 * glm::vec4(0.f, 0.f, 0.f, 1.f);
		GLfloat x1 = -425, z1 = -1200, x2 = 425, z2 = -1100;
		
		if (x1 < result.x && result.x < x2 &&
			z1 < result.z && result.z < z2) {
			flag_hit = 1;
			s_fire.systemInit(0, 20, -1150, FIRE, 0);
			s_ship.systemInit(0, 40, -1130, SHIP, 0);
		}
		else
			printf("Miss!");
	}
	if (flag_hit == 1) {
		s_fire.systemLoop();
		s_ship.systemLoop();
	}

	if (flag == -1 && flag_hit != 1) {
		for (int i = 0; i < 400; i++) {
			globe[0]->elements[i]->model = glm::translate(glm::mat4x4(), glm::vec3(temp.x - globe[0]->elements[0]->pos[0], temp.y - globe[0]->elements[0]->pos[1], temp.z - globe[0]->elements[0]->pos[2]));
			globe[0]->elements[i]->enable = false;
		}
		s_blast.systemInit(temp.x, temp.y, temp.z, BLAST, 0);
		//s_fire.systemInit(temp.x, temp.y+30, temp.z, FIRE, 0);
		flag2 = 1;
		flag = 0;
		time = 0;
		axic_pro = 0;
	}
	if (flag2 == 1) {
		s_blast.systemLoop();
		//s_fire.systemLoop();
		blast_time++;
		if (blast_time > 40) {
			blast_time = 0;
			flag2 = 0;
			for (int i = 0; i < 400; i++) {
				globe[0]->elements[i]->model = glm::translate(glm::mat4x4(), glm::vec3(0 - globe[0]->elements[0]->pos[0], 0 - globe[0]->elements[0]->pos[1], 0 - globe[0]->elements[0]->pos[2]));
				globe[0]->elements[i]->enable = false;
			}
			s_blast.systemInit(0, 0, 0, BLAST, 0);
		}
	}
	Wave::calWave(0.4, 80, 50);
#if waveOn
	globe[0]->textures[0]->timerLoop();
	for (int i = 0; i < WAVE_COL; ++i)
	{
		for(int j = 0; j < WAVE_NUM; ++j)
		globe[0]->textures[i*4+j + 9]->timerLoop();
	}
#else
	globe[0]->textures[0]->timerLoop();
	globe[0]->textures[9]->timerLoop();
	globe[0]->textures[10]->timerLoop();
	globe[0]->textures[11]->timerLoop();
	//globe[0]->textures[12]->timerLoop();
	//globe[0]->textures[13]->timerLoop();
	//globe[0]->textures[14]->timerLoop();
	//globe[0]->textures[15]->timerLoop();
#endif
	if (flag == -1) {
		for (int i = 0; i < 400; i++) {
			globe[0]->elements[i]->model = glm::translate(glm::mat4x4(), glm::vec3(0 - globe[0]->elements[0]->pos[0], 0 - globe[0]->elements[0]->pos[1], 0 - globe[0]->elements[0]->pos[2]));
			flag = 0;
			globe[0]->elements[i]->enable = false;
		}
		s_blast.systemInit(0, 0, 0, BLAST, 0);
	}
	glutTimerFunc(20, GLFTimer, 0);
	glutPostRedisplay();
}
void build() {
	static Scene tmp;
	for (int i = 0; i < FIRENUM; i++) {
		r[i] = new rectangle();
		r[i]->setShape(4, 8);
		r[i]->fill("source//file.jpg");
		r[i]->color = glm::vec4(initialColor, 1.f);
	}
	/*tmp.elements.push_back((new Element())->load("source//BB-61+Battleship1.obj"));
	tmp.elements[0]->model = glm::translate(glm::mat4x4(), glm::vec3(0, -250, 0));
	tmp.elements.push_back((new Element())->load("source//qiu1.obj"));
	tmp.elements[1]->model = glm::translate(glm::mat4x4(), glm::vec3(0, -53, 0));
	
	//tmp.elements.push_back((new Element(true, glm::vec3(1.f), glm::vec3(1.2f)))->load("source//castle.obj"));
	//tmp.textures.push_back((new Texture())->load("source//pic.obj"));*/
	for (int i = 0; i < 400; i++) {
		tmp.elements.push_back((new Element()));
		tmp.elements[i]->addBall(1, 2, 1, 1, 1);
		tmp.elements[i]->pure = true;
		tmp.elements[i]->enable = false;
	}
	//tmp.elements.push_back((new Element())->load("source//iowa_lod0_jpg_combined.obj"));
	//tmp.elements[400]->model = glm::translate(glm::mat4x4(), glm::vec3(0, 20, 0));
	tmp.elements.push_back((new Element()));
	tmp.elements[400]->addBall(2, 20, 0.4, 0.4, 0.4);
	tmp.elements[400]->model = glm::translate(glm::mat4x4(), glm::vec3(0, -20, 0));
	tmp.elements.push_back((new Element())->load("source//mk32_lod0_jpg.obj"));
	//tmp.elements[i]->addBall(5, 5, 0, 0, 0);
	tmp.elements[401]->pure = true;
	tmp.elements[401]->enable = false;
	tmp.elements.push_back((new Element())->load("source//mk32_lod0_jpg.obj"));
	//tmp.elements[i]->addBall(5, 5, 0, 0, 0);
	tmp.elements[402]->pure = true;
	tmp.elements[402]->enable = false;
	//tmp.elements.push_back((new Element())->load("source//BB-61+Battleship_fix.obj"));
	//tmp.elements[401]->model = glm::translate(glm::mat4x4(), glm::vec3(0, -160, -1150)) * glm::rotate(glm::mat4x4(), 90.0f, glm::vec3(0.0, 1.0, 0.0)) *  glm::scale(glm::mat4x4(), glm::vec3(0.5, 0.5, 0.5));
	
	tmp.textures.push_back(new Wave());
	//tmp.elements.push_back((new Element())->load("source//ground.obj"));
#if waveOn
	tmp.textures[0]->model = glm::translate(glm::mat4x4(), glm::vec3(3000, -38, 2000));
#else
	tmp.textures[0]->model = glm::translate(glm::mat4x4(), glm::vec3(1000, -18, 0));
#endif
	tmp.textures[0]->model = glm::scale(tmp.textures[0]->model, glm::vec3(250, 250, 200));
	tmp.textures[0]->model = glm::rotate(tmp.textures[0]->model, G_PI / 2.f, glm::vec3(1, 0, 0));

	tmp.textures.push_back((new Texture())->load("source//pic.obj"));
	glm::mat4x4 trans = glm::scale(glm::mat4x4(), glm::vec3(1, 1, 1));
	trans = glm::translate(trans, glm::vec3(0, -5.2, 0));
	tmp.textures[1]->model = trans;
	tmp.textures[1]->enable = false;
	tmp.textures.push_back((new Texture())->load("source//skyUp.obj"));
	tmp.textures[2]->pure = true;
	tmp.textures.push_back((new Texture())->load("source//skyBottom.obj"));
	tmp.textures[3]->pure = true;
	tmp.textures.push_back((new Texture())->load("source//skyFront.obj"));
	tmp.textures[4]->pure = true;
	tmp.textures.push_back((new Texture())->load("source//skyBehind.obj"));
	tmp.textures[5]->pure = true;
	tmp.textures.push_back((new Texture())->load("source//skyLeft.obj"));
	tmp.textures[6]->pure = true;
	tmp.textures.push_back((new Texture())->load("source//skyRight.obj"));
	tmp.textures[7]->pure = true;
	tmp.textures.push_back((new Texture())->load("source//iowa_lod0_jpg_combined.obj"));
	tmp.textures[8]->model = glm::translate(glm::mat4x4(), glm::vec3(0, -10, 0));
#if waveOn
	for (int i = 0; i < WAVE_COL; ++i) {
		for (int j = 0; j < WAVE_NUM; ++j) {
			tmp.textures.push_back(new Wave());
			//tmp.elements.push_back((new Element())->load("source//ground.obj"));
			tmp.textures[i * 4 + j + 9]->model = glm::translate(glm::mat4x4(), glm::vec3(3000 - (j + 1) * 1580, -38, 2000 - 970 * i));
			tmp.textures[i * 4 + j + 9]->model = glm::scale(tmp.textures[i * 4 + j + 9]->model, glm::vec3(100, 100, 100));
			//tmp.textures[i+9]->model = glm::rotate(tmp.textures[i + 9]->model, G_PI * (i+1), glm::vec3(0, 1, 0));
			tmp.textures[i * 4 + j + 9]->model = glm::rotate(tmp.textures[i * 4 + j + 9]->model, G_PI / 2.f, glm::vec3(1, 0, 0));
		}
	}
	tmp.textures.push_back((new Texture())->load("source//iowa_lod0_jpg_combined.obj"));
	tmp.textures[9 + WAVE_NUM*WAVE_COL]->model = glm::translate(glm::mat4x4(), glm::vec3(0, -250, -1150)) * glm::rotate(glm::mat4x4(), 90.0f, glm::vec3(0.0, 1.0, 0.0));
#else
	tmp.textures.push_back(new Wave());
	tmp.textures[9]->model = glm::translate(glm::mat4x4(), glm::vec3(1000 - 1 * 3950, -18, 0));
	tmp.textures[9]->model = glm::scale(tmp.textures[9]->model, glm::vec3(250, 250, 200));
	tmp.textures[9]->model = glm::rotate(tmp.textures[9]->model, G_PI / 2.f, glm::vec3(1, 0, 0));
	
	tmp.textures.push_back(new Wave());
	tmp.textures[10]->model = glm::translate(glm::mat4x4(), glm::vec3(1000, -18, 0-1950));
	tmp.textures[10]->model = glm::scale(tmp.textures[10]->model, glm::vec3(250, 250, 200));
	tmp.textures[10]->model = glm::rotate(tmp.textures[10]->model, G_PI / 2.f, glm::vec3(1, 0, 0));
	
	tmp.textures.push_back(new Wave());
	tmp.textures[11]->model = glm::translate(glm::mat4x4(), glm::vec3(1000 - 1 * 3950, -18, 0-1950));
	tmp.textures[11]->model = glm::scale(tmp.textures[11]->model, glm::vec3(250, 250, 200));
	tmp.textures[11]->model = glm::rotate(tmp.textures[11]->model, G_PI / 2.f, glm::vec3(1, 0, 0));
	
	/*tmp.textures.push_back(new Wave());
	tmp.textures[12]->model = glm::translate(glm::mat4x4(), glm::vec3(1000, -18, 1950));
	tmp.textures[12]->model = glm::scale(tmp.textures[12]->model, glm::vec3(250, 250, 200));
	tmp.textures[12]->model = glm::rotate(tmp.textures[12]->model, G_PI / 2.f, glm::vec3(1, 0, 0));
	
	tmp.textures.push_back(new Wave());
	tmp.textures[13]->model = glm::translate(glm::mat4x4(), glm::vec3(1000 - 1 * 3950, -18, 1950));
	tmp.textures[13]->model = glm::scale(tmp.textures[13]->model, glm::vec3(250, 250, 200));
	tmp.textures[13]->model = glm::rotate(tmp.textures[13]->model, G_PI / 2.f, glm::vec3(1, 0, 0));*/
	
	/*tmp.textures.push_back(new Wave());
	tmp.textures[14]->model = glm::translate(glm::mat4x4(), glm::vec3(1000, -18, -2*1950));
	tmp.textures[14]->model = glm::scale(tmp.textures[14]->model, glm::vec3(250, 250, 200));
	tmp.textures[14]->model = glm::rotate(tmp.textures[14]->model, G_PI / 2.f, glm::vec3(1, 0, 0));

	tmp.textures.push_back(new Wave());
	tmp.textures[15]->model = glm::translate(glm::mat4x4(), glm::vec3(1000 - 1 * 3950, -18, -2 * 1950));
	tmp.textures[15]->model = glm::scale(tmp.textures[15]->model, glm::vec3(250, 250, 200));
	tmp.textures[15]->model = glm::rotate(tmp.textures[15]->model, G_PI / 2.f, glm::vec3(1, 0, 0));*/

	tmp.textures.push_back((new Texture())->load("source//iowa_lod0_jpg_combined.obj"));
	tmp.textures[12]->model = glm::translate(glm::mat4x4(), glm::vec3(0, -230, -1150)) * glm::rotate(glm::mat4x4(), 90.0f, glm::vec3(0.0, 1.0, 0.0));
#endif

	
	globe.push_back(&tmp);
	light.push_back(new Light(0, 0, glm::vec3(-200.f, 200.f, 18.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.1f, .1f, .1f), 1.f, glm::vec3(), 0.f));
	light.push_back(new Light(0, 1, glm::vec3(20.f, 140.f, 20.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(.2f, .2f, .2f), .4f, glm::vec3(0.f, -160.f, -1150.f), 0.997f));
	//light.push_back(new Light(2, glm::vec3(24.f, 32, 10.f), glm::vec3(.2f, .2f, .2f), glm::vec3(.0f, .0f, .0f), .4f));
	s_blast = systemp(x2, y2, z2, BLAST, 1);
	s_fire = systemp(x2, y2, z2, FIRE, 1);
	//s_fire.systemInit(0, 100, -1150, FIRE, 0);
	s_ship = systemp(x2, y2, z2, SHIP, 1);
	//s_ship.systemInit(0, 0, 0, SHIP, 0);
}
int main(int argc, char *argv[]) {
#if waveOn
	Wave::initVector();
#endif
#if DEBUG
	Wave::initVector();
#endif
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Battleship Simulator");
	GLFInit();

	glutDisplayFunc(GLFDisplay);
	glutReshapeFunc(GLFReshape);
	glutPassiveMotionFunc(GLFMouse);
	glutMotionFunc(GLFDrag);
	glutMouseFunc(GLFClick);
	glutMouseWheelFunc(GLFWheel);
	glutKeyboardFunc(GLFKeyDown);
	glutKeyboardUpFunc(GLFKeyUp);
	glutTimerFunc(2, GLFTimer, 0);
	glutSpecialFunc(GLFSpecialDown);
	glutSpecialUpFunc(GLFSpecialUp);

	build();
	glutMainLoop();
	return 0;
}
