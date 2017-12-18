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
#define WAVE_COUNT		6
#define STRIP_COUNT		80
#define STRIP_LENGTH	50
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
static GLfloat wave_strip[STRIP_COUNT * STRIP_LENGTH * 3];
static struct wave
{
	GLfloat time[WAVE_COUNT];
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
	if(xy_result == Gerstner[i])
		return Gerstner[i] * (amplitude / 50.0);
	if (xy_result > Gerstner[i])
		return ((Gerstner[i+3] - Gerstner[i+1]) * (Gerstner[i+2] - Gerstner[i])
		     / (Gerstner[i+2] - Gerstner[i])
			 + Gerstner[i+3]) * amplitude / 50.0;
	//z'=(y1-y0)(x1-x0)
	//   --------------  + y1
	//      (x1-x0)
	//z = z' * 振幅 / 50
}

static void calcu_Wave()
{
	GLfloat d, waveZ;
	int i = 0, w = 0;
	d = (wave_strip[i] - waveValues.wave_amp[w*2]) + (wave_strip[i+1] - waveValues.wave_amp[w*2+1]) * tan(waveValues.wave_dir[w])
		* cos(waveValues.wave_dir[w]);
	waveZ += waveValues.wave_amp[w] - calcu_GerstnerZ(waveValues.wave_len[w], waveValues.wave_amp[w], d + waveValues.wave_speed[w], gerstner_pt_a);
	return;
}
int main()
{

	return 0;
}
