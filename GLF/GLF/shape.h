#pragma once
#ifndef  SHINA_LIB_SHAPE_H
#include "main.h"
#include "shader.h"

extern Shader *paticleShader;
class sphere
{
private:
	using vec3 = glm::vec3;
	using mat4 = glm::mat4;
	using vec4 = glm::vec4;
	GLfloat step;
	GLfloat radius;
	mat4 model;
	std::string name;
	std::vector <GLfloat>pos;
	std::vector<GLuint>index;
	vec4 color;


	void pushPos(float pos1 = 0, float pos2 = 0, float pos3 = 0) {
		pos.push_back(pos1);
		pos.push_back(pos2);
		pos.push_back(pos3);
	}
	GLfloat *getPos() {
		return toArray<GLfloat>(&pos);
	}
	GLuint*getIndex()
	{
		return toArray<GLuint>(&index);
	}
public:
	//for shader
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	//end for shader

	sphere(GLfloat _radius=-1,GLfloat _step=0.1,std::string _name="",vec4 _color=vec4(1,1,1,1)):
		step(_step),radius(_radius),name(_name),color(_color)
	{
		glGenVertexArrays(1, &vao);
		//system("pause");
		glGenBuffers(1, &vbo);
	
		glGenBuffers(1, &ebo);
	}
	~sphere()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
	sphere& operator= (sphere e) {
		cout << "1111" << endl;
		if (vao != e.vao)glDeleteVertexArrays(1, &vao);
		if (vbo != e.vbo)glDeleteVertexArrays(1, &vbo);
		if (ebo != e.ebo)glDeleteVertexArrays(1, &ebo);
		

		pos = e.pos;
		step = e.step;
		radius = e.radius;
		index = e.index;
		color = e.color;

		model = e.model;

		vao = e.vao;
		ebo = e.ebo;
		vbo = e.vbo;

		return e;
	}

	// you just need to call once
	/*
		@param: return: true for success, false for fail
	*/
	bool draw()
	{
		if (radius <= 0)
		{
			return false;
		}
		std::vector<GLfloat>temp_position;
		//计算球上的顶点的位置
		for (GLfloat row_angle = -90; row_angle <= 90; row_angle += step)
		{
			for (GLfloat col_angle = 0; col_angle < 360; col_angle += step)
			{
				GLfloat r = radius*glm::cos(glm::radians(row_angle));
				GLfloat x = r*glm::cos(glm::radians(col_angle));
				GLfloat y = radius*glm::sin(glm::radians(row_angle));
				GLfloat z = r*glm::sin(glm::radians(col_angle));
				temp_position.push_back(x);
				temp_position.push_back(y);
				temp_position.push_back(z);
			}
		}
		int row = 180 / step + 1;
		int col = 360 / step;
		int k = col*(row - 2) * 6 * 8;
		//GLfloat*vertiices = new GLfloat[k];

		int count = 0;
		for (int i = 0; i < row; i++)
		{
			if (i != 0 && i != row - 1)
			{
				for (int j = 0; j < col; j++)
				{
					k = i*col + j;
					pushPos(temp_position[(k + col) * 3], temp_position[(k + col) * 3 + 1], temp_position[(k + col) * 3 + 2]);

					int index = k + 1;
					if (j == col - 1)
						index -= col;

					pushPos(temp_position[index * 3], temp_position[index * 3 + 1], temp_position[index * 3 + 2]);

					pushPos(temp_position[k * 3], temp_position[k * 3 + 1], temp_position[k * 3 + 2]);
				}
				for (int j = 0; j < col; j++)
				{
					k = i*col + j;
					pushPos(temp_position[(k - col) * 3], temp_position[(k - col) * 3 + 1], temp_position[(k - col) * 3 + 2]);

					int index = k - 1;
					if (j == 0)
						index += col;
					pushPos(temp_position[index * 3], temp_position[index * 3 + 1], temp_position[index * 3 + 2]);

					pushPos(temp_position[k * 3], temp_position[k * 3 + 1], temp_position[k * 3 + 2]);
				}
			}
		}

	}

	/*
		@param: note: call this in render,remind to draw first
	*/
	void show()
	{
		if (pos.size() == 0)	return;
		paticleShader->use();

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, pos.size() * 4, getPos(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
		glDrawArrays(GL_TRIANGLES, 0, pos.size() / 3);
	}
};


#endif // ! SHINA_LIB_SHAPE_H
