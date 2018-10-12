#pragma once
#include "model.h"
#define START_X		-4.0
#define START_Y		-2.5
#define START_Z		0
#define LENGTH_X	0.1
#define LENGTH_Y	0.1
#define WAVE_COUNT		6
#define STRIP_COUNT		160
#define STRIP_LENGTH 	100
#define DATA_LENGTH		STRIP_LENGTH*2*(STRIP_COUNT-1)
#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768
static int selectWhichWave[6] =
{ 0, 0, 1, 1, 1, 1 };//当前循环应该叠加哪个波
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
static struct wave
{
	//GLfloat time;
	GLfloat wave_len[WAVE_COUNT];
	GLfloat wave_amp[WAVE_COUNT];
	GLfloat wave_speed[WAVE_COUNT];
	GLfloat wave_dir[WAVE_COUNT];
	GLfloat wave_start[WAVE_COUNT * 2];
}waveValues;
class Wave :public Texture {
public:
	static GLfloat wave_time;
	static GLfloat wave_strip[STRIP_COUNT * STRIP_LENGTH * 3];
	static vector<float> v_pos;
	static vector<float> v_normal;
	static vector<float> v_coord;
	Wave() : Texture(true, glm::vec3(.8f, .8f, .8f), glm::vec3(.2f, .2f, 1.2f), .4f){
		for (int i = 0; i < DATA_LENGTH *3-2; i++) {
			pos.push_back(0);
			pos.push_back(0);
			pos.push_back(0);
			normal.push_back(0);
			normal.push_back(0);
			normal.push_back(0);
			coord.push_back(0);
			coord.push_back(0);
		}
		initWave();
		pic("source//Water_Texture.jpg");
	}
	static void initVector() {
		for (int i = 0; i < DATA_LENGTH * 3 - 2; i++) {
			v_pos.push_back(0);
			v_pos.push_back(0);
			v_pos.push_back(0);
			v_normal.push_back(0);
			v_normal.push_back(0);
			v_normal.push_back(0);
			v_coord.push_back(0);
			v_coord.push_back(0);
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
	void initWave() {
		wave_time = 0.0;
		for (int w = 0; w<WAVE_COUNT; w++)
		{
			waveValues.wave_len[w] = wave_para[w][0];
			waveValues.wave_amp[w] = wave_para[w][1];
			waveValues.wave_dir[w] = wave_para[w][2];
			waveValues.wave_speed[w] = wave_para[w][3];
			waveValues.wave_start[w * 2] = wave_para[w][4];
			waveValues.wave_start[w * 2 + 1] = wave_para[w][5];
		}
	}
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
		while (i <= 18 && xy_result < Gerstner[i] || xy_result >= Gerstner[i + 2])
			i += 2;
		//线性插值法的使用条件，必须寻找到一组将其夹在中间的2个点
		if (xy_result == Gerstner[i])//当前的点的横坐标正好与
			return Gerstner[i + 1] * (amplitude / 50.0);
		if (xy_result > Gerstner[i])
			return ((Gerstner[i + 3] - Gerstner[i + 1]) * (xy_result - Gerstner[i])
				/ (Gerstner[i + 2] - Gerstner[i])
				+ Gerstner[i + 1]) * amplitude / 50.0;
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
	static void calWave(float height, float length, float width) {
		static GLfloat vertex_data[DATA_LENGTH * 3] = { 0 };
		static GLfloat pt_normal[STRIP_COUNT*STRIP_LENGTH * 3] = { 0 };
		static GLfloat normal_data[DATA_LENGTH * 3] = { 0 };
		static GLfloat color_data[DATA_LENGTH * 3] = { 0 };
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
						waveZ += calcu_GerstnerZ(waveValues.wave_len[w], waveValues.wave_amp[w], d + waveValues.wave_speed[w] * wave_time, gerstner_pt_a);
					else
						waveZ += calcu_GerstnerZ(waveValues.wave_len[w], waveValues.wave_amp[w], d + waveValues.wave_speed[w] * wave_time, gerstner_pt_b);
				}
				wave_strip[index + 2] = START_Z + waveZ * height;
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
		for (int i = 0; i < DATA_LENGTH * 3; i++) {
			if (i % 3 != 2) {
				color_data[i] = 0;
			}
			else {
				color_data[i] = 1;
			}
		}
		for (int i = 0; i < DATA_LENGTH * 3 - 6; i += 3) {
			for (int j = 0; j < 9; j+=3) {
				v_pos[3 * i + j] = vertex_data[i + j];
				v_pos[3 * i + j + 1] = vertex_data[i + j + 1];
				v_pos[3 * i + j + 2] = vertex_data[i + j + 2];
				v_normal[3 * i + j] = -normal_data[i + j];
				v_normal[3 * i + j + 1] = -normal_data[i + j + 1];
				v_normal[3 * i + j + 2] = -normal_data[i + j + 2];
				float tex_x = (v_pos[3 * i + j] + wave_time / 20) / 8 + 0.5;
				float tex_y = 0.5 - (v_pos[3 * i + j + 1] + wave_time / 25) / 5;
				v_coord[2 * i + 2 * j / 3] = tex_x;
				v_coord[2 * i + 2 * j / 3 + 1] = tex_y;
			}
		}
		return;
	}
	virtual void timerLoop(){
		pos = v_pos;
		normal = v_normal;
		coord = v_coord;
		wave_time += 0.2;
	}
};
GLfloat Wave::wave_time = 0;
GLfloat Wave::wave_strip[STRIP_COUNT * STRIP_LENGTH * 3] = { 0 };
vector<float> Wave::v_pos = { 0 };
vector<float> Wave::v_normal = { 0 };
vector<float> Wave::v_coord = { 0 };
