#include <GL/glew.h>
//#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <string>
#include <iostream>
#define START_X		-4.0
#define START_Y		-2.5
#define START_Z		0
#define LENGTH_X	0.1
#define LENGTH_Y	0.1
#define WAVE_COUNT		6
#define STRIP_COUNT		80
#define STRIP_LENGTH	50
#define DATA_LENGTH		STRIP_LENGTH*2*(STRIP_COUNT-1)
#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768

static const GLfloat wave_para[6][6] = {
	{ 1.6,	0.12,	0.9,	0.06,	0.0,	0.0 },
	{ 1.3,	0.1,	1.14,	0.09,	0.0,	0.0 },
	{ 0.2,	0.01,	0.8,	0.08,	0.0,	0.0 },
	{ 0.18,	0.008,	1.05,	0.1,	0.0,	0.0 },
	{ 0.23,	0.005,	1.15,	0.09,	0.0,	0.0 },
	{ 0.12,	0.003,	0.97,	0.14,	0.0,	0.0 }
};
static const GLfloat gerstner_pt_a[22] = {
	0.0,0.0, 41.8,1.4, 77.5,5.2, 107.6,10.9,
	132.4,17.7, 152.3,25.0, 167.9,32.4, 179.8,39.2,
	188.6,44.8, 195.0,48.5, 200.0,50.0
};
static const GLfloat gerstner_pt_b[22] = {
	0.0,0.0, 27.7,1.4, 52.9,5.2, 75.9,10.8,
	97.2,17.6, 116.8,25.0, 135.1,32.4, 152.4,39.2,
	168.8,44.8, 184.6,48.5, 200.0,50.0
};

static struct {
	GLuint vertex_buffer, normal_buffer;
	GLuint vertex_shader, fragment_shader, program;
	GLuint diffuse_texture, normal_texture;

	struct {
		GLint diffuse_texture, normal_texture;
	} uniforms;

	struct {
		GLint position;
		GLint normal;
	} attributes;
} names;
static GLfloat wave_strip[STRIP_COUNT * STRIP_LENGTH * 3];
static GLfloat pt_normal[STRIP_COUNT*STRIP_LENGTH * 3] = { 0 };
static GLfloat vertex_data[DATA_LENGTH * 3] = { 0 };
static GLfloat normal_data[DATA_LENGTH * 3] = { 0 };

static int selectWhichWave[6] =
	{ 0, 0, 1, 1, 1, 1 };//当前循环应该叠加哪个波
static struct wave
{
	GLfloat time;
	GLfloat wave_len[WAVE_COUNT];
	GLfloat wave_amp[WAVE_COUNT];
	GLfloat wave_speed[WAVE_COUNT];
	GLfloat wave_dir[WAVE_COUNT];
	GLfloat wave_start[WAVE_COUNT * 2];
}waveValues;

static GLfloat calcu_GerstnerZ(GLfloat length, GLfloat amplitude, GLfloat xy_result, const GLfloat Gerstner[22])
{//线性内插函数计算Gerstner波在三维网格上的高度z波动，理论上需要套入公式一步一步计算，为了优化，采用
//线性内插的方法近似。此法可大大提高效率。
	//xy_result为在三维空间中的x y坐标经过一定转换规则转化而成的坐标，该坐标可当作x坐标代入二维的
	//Gerstner波方程从而计算出高度值z
	xy_result = xy_result * 400.0 / length;
	//此处先将真正的xy_result值按照[0, 400]的domain, map到相应的虚值上
	while (xy_result < 0.0)//波的特性，此时点在左侧，移动回0到400之间
		xy_result += 400.0;
	while (xy_result > 400.0)//波的特性，此时点在右侧，移动回0到400之间
		xy_result -= 400.0;
	if (xy_result > 200.0)//由于该波上半周期与下半周期对称，所以如果点在下半周期，移动回第一上半周期
		xy_result = 400.0 - xy_result;
	//
	int i = 0;
	while (i <= 18 && xy_result < Gerstner[i] || xy_result >= Gerstner[i+2])
		i += 2;
	//线性插值法的使用条件，必须寻找到一组将其夹在中间的2个点
	if(xy_result == Gerstner[i])//当前的点的横坐标正好与
		return Gerstner[i+1] * (amplitude / 50.0);
	if (xy_result > Gerstner[i])
		return ((Gerstner[i+3] - Gerstner[i+1]) * (xy_result - Gerstner[i])
		     / (Gerstner[i+2] - Gerstner[i])
			 + Gerstner[i+1]) * amplitude / 50.0;
	//z'=(z1-z0)(x-x0)
	//   --------------  + z0
	//      (x1-x0)
	//z = z' * 振幅 / 50
}
static void normalize(float in[], float out[], int dimensional)
{
	int i = 0;
	float l = 0.0;

	for (i = 0; i < dimensional; i++)
		l += in[i] * in[i];
	if (l < 0.0000001) //发现sqrt(x*x + y*y + z*z)过小，需要重新计算
	{
		l = 0.0;
		for (i = 0; i < dimensional; i++)
			in[i] *= 10000.0;//扩大10000倍，这样能让小数部分后几位不被float的精度问题所抛弃
		for (i = 0; i < dimensional; i++)
			l += in[i] * in[i];
	}
	l = sqrt(l);
	for (i = 0; i < dimensional; i++)
		out[i] /= l;

	return;
}
static void calcu_Wave(float wave_hight_scale)
{
	GLfloat d, waveZ = 0.0;
	int index = 0;
	for (int i = 0; i < STRIP_COUNT; ++i)//计算所有网格的点的高度值
	{
		for (int j = 0; j < STRIP_LENGTH; ++j)
		{
			waveZ = 0.0;
			for (int w = 0; w < WAVE_COUNT; ++w)
			{
				d = (wave_strip[index] - waveValues.wave_amp[w * 2]) + (wave_strip[index + 1] - waveValues.wave_start[w * 2 + 1]) * tan(waveValues.wave_dir[w])
					* cos(waveValues.wave_dir[w]);
				if (selectWhichWave[w] == 1)
					waveZ += calcu_GerstnerZ(waveValues.wave_len[w], waveValues.wave_amp[w], d + waveValues.wave_speed[w] * waveValues.time, gerstner_pt_a);
				else
					waveZ += calcu_GerstnerZ(waveValues.wave_len[w], waveValues.wave_amp[w], d + waveValues.wave_speed[w] * waveValues.time, gerstner_pt_b);
			}
			wave_strip[index + 2] = START_Z + waveZ * wave_hight_scale;
			index += 3;
		}
	}
	index = 0;
	for (int i = 0; i<STRIP_COUNT; i++)//计算当前点的法线
	{
		for (int j = 0; j<STRIP_LENGTH; j++)
		{
			int p0 = index - STRIP_LENGTH * 3, p1 = index + 3, p2 = index + STRIP_LENGTH * 3, p3 = index - 3;
			float xa, ya, za, xb, yb, zb;
			if (i > 0) {
				if (j > 0) {
					xa = wave_strip[p0] - wave_strip[index], ya = wave_strip[p0 + 1] - wave_strip[index + 1], za = wave_strip[p0 + 2] - wave_strip[index + 2];
					xb = wave_strip[p3] - wave_strip[index], yb = wave_strip[p3 + 1] - wave_strip[index + 1], zb = wave_strip[p3 + 2] - wave_strip[index + 2];
					pt_normal[index] += ya*zb - yb*za;
					pt_normal[index + 1] += xb*za - xa*zb;
					pt_normal[index + 2] += xa*yb - xb*ya;
				}
				if (j < STRIP_LENGTH - 1) {
					xa = wave_strip[p1] - wave_strip[index], ya = wave_strip[p1 + 1] - wave_strip[index + 1], za = wave_strip[p1 + 2] - wave_strip[index + 2];
					xb = wave_strip[p0] - wave_strip[index], yb = wave_strip[p0 + 1] - wave_strip[index + 1], zb = wave_strip[p0 + 2] - wave_strip[index + 2];
					pt_normal[index] += ya*zb - yb*za;
					pt_normal[index + 1] += xb*za - xa*zb;
					pt_normal[index + 2] += xa*yb - xb*ya;
				}
			}
			if (i < STRIP_COUNT - 1) {
				if (j > 0) {
					xa = wave_strip[p3] - wave_strip[index], ya = wave_strip[p3 + 1] - wave_strip[index + 1], za = wave_strip[p3 + 2] - wave_strip[index + 2];
					xb = wave_strip[p2] - wave_strip[index], yb = wave_strip[p2 + 1] - wave_strip[index + 1], zb = wave_strip[p2 + 2] - wave_strip[index + 2];
					pt_normal[index] += ya*zb - yb*za;
					pt_normal[index + 1] += xb*za - xa*zb;
					pt_normal[index + 2] += xa*yb - xb*ya;
				}
				if (j < STRIP_LENGTH - 1) {
					xa = wave_strip[p2] - wave_strip[index], ya = wave_strip[p2 + 1] - wave_strip[index + 1], za = wave_strip[p2 + 2] - wave_strip[index + 2];
					xb = wave_strip[p1] - wave_strip[index], yb = wave_strip[p1 + 1] - wave_strip[index + 1], zb = wave_strip[p1 + 2] - wave_strip[index + 2];
					pt_normal[index] += ya*zb - yb*za;
					pt_normal[index + 1] += xb*za - xa*zb;
					pt_normal[index + 2] += xa*yb - xb*ya;
				}
			}
			normalize(&pt_normal[index], &pt_normal[index], 3);

			index += 3;
		}
	}
	int pt;
	for (int c = 0; c<(STRIP_COUNT - 1); c++)
	{
		for (int l = 0; l<2 * STRIP_LENGTH; l++)
		{
			if (l % 2 == 1) {
				pt = c*STRIP_LENGTH + l / 2;
			}
			else {
				pt = c*STRIP_LENGTH + l / 2 + STRIP_LENGTH;
			}
			index = STRIP_LENGTH * 2 * c + l;
			for (int i = 0; i<3; i++) {
				vertex_data[index * 3 + i] = wave_strip[pt * 3 + i];
				normal_data[index * 3 + i] = pt_normal[pt * 3 + i];
			}
		}
	}
	return;
	//Calculate wave_strip[z], poly_normal[] and pt_normal[]
	/*int index = 0;
	float d, wave;
	for (int i = 0; i<STRIP_COUNT; i++)
	{
		for (int j = 0; j<STRIP_LENGTH; j++)
		{
			wave = 0.0;
			for (int w = 0; w<WAVE_COUNT; w++) {
				d = (wave_strip[index] - waveValues.wave_start[w * 2] + (wave_strip[index + 1] - waveValues.wave_start[w * 2 + 1]) * tan(waveValues.wave_dir[w])) * cos(waveValues.wave_dir[w]);
				if (selectWhichWave[w] == 1) {
					wave += waveValues.wave_amp[w] - calcu_GerstnerZ(waveValues.wave_len[w], waveValues.wave_amp[w], d + waveValues.wave_speed[w] * waveValues.time, gerstner_pt_a);
				}
				else {
					wave += waveValues.wave_amp[w] - calcu_GerstnerZ(waveValues.wave_len[w], waveValues.wave_amp[w], d + waveValues.wave_speed[w] * waveValues.time, gerstner_pt_b);
				}
			}
			wave_strip[index + 2] = START_Z + wave*wave_hight_scale;
			index += 3;
		}
	}

	index = 0;
	for (int i = 0; i<STRIP_COUNT; i++)
	{
		for (int j = 0; j<STRIP_LENGTH; j++)
		{
			int p0 = index - STRIP_LENGTH * 3, p1 = index + 3, p2 = index + STRIP_LENGTH * 3, p3 = index - 3;
			float xa, ya, za, xb, yb, zb;
			if (i > 0) {
				if (j > 0) {
					xa = wave_strip[p0] - wave_strip[index], ya = wave_strip[p0 + 1] - wave_strip[index + 1], za = wave_strip[p0 + 2] - wave_strip[index + 2];
					xb = wave_strip[p3] - wave_strip[index], yb = wave_strip[p3 + 1] - wave_strip[index + 1], zb = wave_strip[p3 + 2] - wave_strip[index + 2];
					pt_normal[index] += ya*zb - yb*za;
					pt_normal[index + 1] += xb*za - xa*zb;
					pt_normal[index + 2] += xa*yb - xb*ya;
				}
				if (j < STRIP_LENGTH - 1) {
					xa = wave_strip[p1] - wave_strip[index], ya = wave_strip[p1 + 1] - wave_strip[index + 1], za = wave_strip[p1 + 2] - wave_strip[index + 2];
					xb = wave_strip[p0] - wave_strip[index], yb = wave_strip[p0 + 1] - wave_strip[index + 1], zb = wave_strip[p0 + 2] - wave_strip[index + 2];
					pt_normal[index] += ya*zb - yb*za;
					pt_normal[index + 1] += xb*za - xa*zb;
					pt_normal[index + 2] += xa*yb - xb*ya;
				}
			}
			if (i < STRIP_COUNT - 1) {
				if (j > 0) {
					xa = wave_strip[p3] - wave_strip[index], ya = wave_strip[p3 + 1] - wave_strip[index + 1], za = wave_strip[p3 + 2] - wave_strip[index + 2];
					xb = wave_strip[p2] - wave_strip[index], yb = wave_strip[p2 + 1] - wave_strip[index + 1], zb = wave_strip[p2 + 2] - wave_strip[index + 2];
					pt_normal[index] += ya*zb - yb*za;
					pt_normal[index + 1] += xb*za - xa*zb;
					pt_normal[index + 2] += xa*yb - xb*ya;
				}
				if (j < STRIP_LENGTH - 1) {
					xa = wave_strip[p2] - wave_strip[index], ya = wave_strip[p2 + 1] - wave_strip[index + 1], za = wave_strip[p2 + 2] - wave_strip[index + 2];
					xb = wave_strip[p1] - wave_strip[index], yb = wave_strip[p1 + 1] - wave_strip[index + 1], zb = wave_strip[p1 + 2] - wave_strip[index + 2];
					pt_normal[index] += ya*zb - yb*za;
					pt_normal[index + 1] += xb*za - xa*zb;
					pt_normal[index + 2] += xa*yb - xb*ya;
				}
			}
			normalize(&pt_normal[index], &pt_normal[index], 3);
				//printf("%d\t%d\n", index / 3 / STRIP_LENGTH, (index / 3) % STRIP_LENGTH);

			index += 3;
		}
	}

	//Calculate vertex_data[] according to wave_strip[], and normal_data[] according to pt_normal[]
	int pt;
	for (int c = 0; c<(STRIP_COUNT - 1); c++)
	{
		for (int l = 0; l<2 * STRIP_LENGTH; l++)
		{
			if (l % 2 == 1) {
				pt = c*STRIP_LENGTH + l / 2;
			}
			else {
				pt = c*STRIP_LENGTH + l / 2 + STRIP_LENGTH;
			}
			index = STRIP_LENGTH * 2 * c + l;
			for (int i = 0; i<3; i++) {
				vertex_data[index * 3 + i] = wave_strip[pt * 3 + i];
				normal_data[index * 3 + i] = pt_normal[pt * 3 + i];
			}
		}
	}*/
}

void *readShader(const char *filename, GLint *length)
{
	FILE *f;
	fopen_s(&f, filename, "r");
	void *buffer;

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = malloc(*length + 1);
	*length = fread(buffer, 1, *length, f);
	fclose(f);
	((char*)buffer)[*length] = '\0';

	return buffer;
}
static GLuint initShader(GLenum type, const char *filename)
{
	GLint length;//存放着色器代码的长度
	GLchar *source = (GLchar *)readShader(filename, &length);
	GLuint shader;
	GLint shader_success;

	char infoLog[2048] = { '\0' };

	if (!source)
		return 0;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, &length);
	free(source);//释放分配存放文件内容的内存
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_success);
	if (!shader_success)
	{
		glGetShaderInfoLog(shader, 2048, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}
static void installShaders(void)
{
	names.vertex_shader = initShader(GL_VERTEX_SHADER, "wave-gerstner-light.vs");
	names.fragment_shader = initShader(GL_FRAGMENT_SHADER, "wave-gerstner-light.fs");

	GLint program_success;
	char infoLog[2048];
	names.program = glCreateProgram();
	glAttachShader(names.program, names.vertex_shader);
	glAttachShader(names.program, names.fragment_shader);
	glLinkProgram(names.program);
	glGetProgramiv(names.program, GL_LINK_STATUS, &program_success);
	if (!program_success)
	{
		glGetProgramInfoLog(names.program, 2048, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glUseProgram(names.program);
	glDeleteShader(names.vertex_shader);
	glDeleteShader(names.fragment_shader);
	return;
}
static void initWave(void)
{
	//Initialize waveValuess{}
	waveValues.time = 0.0;
	for (int w = 0; w<WAVE_COUNT; w++)
	{
		waveValues.wave_len[w] = wave_para[w][0];
		waveValues.wave_amp[w] = wave_para[w][1];
		waveValues.wave_dir[w] = wave_para[w][2];
		waveValues.wave_speed[w] = wave_para[w][3];
		waveValues.wave_start[w * 2] = wave_para[w][4];
		waveValues.wave_start[w * 2 + 1] = wave_para[w][5];
	}

	//Initialize wave_strip[]
	int index = 0;
	for (int i = 0; i<STRIP_COUNT; i++)
	{
		for (int j = 0; j<STRIP_LENGTH; j++)
		{
			wave_strip[index] = START_X + i*LENGTH_X;
			wave_strip[index + 1] = START_Y + j*LENGTH_Y;
			index += 3;
		}
	}
}
static void initGL(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLfloat materAmbient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat materSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightAmbient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat envirAmbient[] = { 1.0, 1.0, 1.0, 1.0 };
	glUniform4fv(glGetUniformLocation(names.program, "materAmbient"), 1, materAmbient);
	glUniform4fv(glGetUniformLocation(names.program, "materSpecular"), 1, materSpecular);
	glUniform4fv(glGetUniformLocation(names.program, "lightDiffuse"), 1, lightDiffuse);
	glUniform4fv(glGetUniformLocation(names.program, "lightAmbient"), 1, lightAmbient);
	glUniform4fv(glGetUniformLocation(names.program, "lightSpecular"), 1, lightSpecular);
	glUniform4fv(glGetUniformLocation(names.program, "envirAmbient"), 1, envirAmbient);

	names.attributes.position = glGetAttribLocation(names.program, "position");
	glGenBuffers(1, &names.vertex_buffer);

	names.attributes.normal = glGetAttribLocation(names.program, "normal");
	glGenBuffers(1, &names.normal_buffer);

	/*names.diffuse_texture = initTexture(diff_texture.c_str());
	names.uniforms.diffuse_texture = glGetUniformLocation(names.program, "textures[0]");
	glUniform1i(names.uniforms.diffuse_texture, 0);

	names.normal_texture = initTexture(norm_texture.c_str());
	names.uniforms.normal_texture = glGetUniformLocation(names.program, "textures[1]");
	glUniform1i(names.uniforms.normal_texture, 1);*/

	glm::mat4 Projection = glm::perspective(45.0f, (float)(SCREEN_WIDTH / SCREEN_HEIGHT), 1.0f, 100.f);
	glm::mat4 viewTransMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
	glm::mat4 viewRotateMat = glm::rotate(viewTransMat, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ModelViewMat = glm::scale(viewRotateMat, glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat3 NormalMat = glm::transpose(glm::inverse(glm::mat3(ModelViewMat)));
	glUniformMatrix4fv(glGetUniformLocation(names.program, "modelViewMat"), 1, GL_FALSE, glm::value_ptr(ModelViewMat));
	glUniformMatrix4fv(glGetUniformLocation(names.program, "perspProjMat"), 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix3fv(glGetUniformLocation(names.program, "normalMat"), 1, GL_FALSE, glm::value_ptr(NormalMat));
}
int main(int argc, char *argv)
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Gerstner waves", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	installShaders();
	initWave();
	initGL();
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		calcu_Wave(0.75);

		glUniform1f(glGetUniformLocation(names.program, "time"), waveValues.time);

		glBindBuffer(GL_ARRAY_BUFFER, names.vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
		glVertexAttribPointer(names.attributes.position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
		glEnableVertexAttribArray(names.attributes.position);

		glBindBuffer(GL_ARRAY_BUFFER, names.normal_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal_data), normal_data, GL_STATIC_DRAW);
		glVertexAttribPointer(names.attributes.normal, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
		glEnableVertexAttribArray(names.attributes.normal);

		for (int c = 0; c<(STRIP_COUNT - 1); c++)
			glDrawArrays(GL_LINE_LOOP, STRIP_LENGTH * 2 * c, STRIP_LENGTH * 2);

		glfwSwapBuffers(window);
		glfwPollEvents();

		waveValues.time += 0.05;
	}
	glfwTerminate();
	return 0;
}
