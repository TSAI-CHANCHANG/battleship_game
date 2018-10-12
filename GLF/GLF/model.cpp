#include "main.h"
#include "shader.h"
#include "light.h"
#include "model.h"
#include "eye.h"

Shader *elementShader;
Shader *texShader;
Shader *shadowShader;
Shader *texParticleShader;

extern vector<Scene*> globe;
extern vector<Light*> light;
extern Eye *eye;

Element *Element::load(const char *filename) {
	std::ifstream fin;

	fin.open(filename);
	if (fin.is_open() == FALSE)return NULL;

	string op;
	float num1, num2, num3;

	vector<float>pos;
	vector<float>color;
	vector<float>normal;

	while (fin >> op) {
		if (op == "v") {
			fin >> num1 >> num2 >> num3;
			pos.push_back(num1);
			pos.push_back(num3);
			pos.push_back(num2);
		}
		else if (op == "vc") {
			fin >> num1 >> num2 >> num3;
			color.push_back(num1);
			color.push_back(num2);
			color.push_back(num3);
		}
		else if (op == "vn") {
			fin >> num1 >> num2 >> num3;
			normal.push_back(num1);
			normal.push_back(num2);
			normal.push_back(num3);
		}
		else if (op == "f") {
			string v1, v2, v3;
			int tmp1 = 0, tmp2 = 0, tmp3 = 0;

			fin >> v1 >> v2 >> v3;
			num1 = (float)atoi(v1.c_str() + tmp1);
			num2 = (float)atoi(v2.c_str() + tmp2);
			num3 = (float)atoi(v3.c_str() + tmp3);
			this->pushPos(pos[(int)num1 * 3 - 3], pos[(int)num1 * 3 - 2], pos[(int)num1 * 3 - 1]);
			this->pushPos(pos[(int)num2 * 3 - 3], pos[(int)num2 * 3 - 2], pos[(int)num2 * 3 - 1]);
			this->pushPos(pos[(int)num3 * 3 - 3], pos[(int)num3 * 3 - 2], pos[(int)num3 * 3 - 1]);

			tmp1 = v1.find_first_of('/', tmp1) + 1;
			tmp2 = v2.find_first_of('/', tmp2) + 1;
			tmp3 = v3.find_first_of('/', tmp3) + 1;
			if (tmp1 == 0 || tmp2 == 0 || tmp3 == 0) {
				if (active == -1) {
					this->pushColor(1, 1, 1);
					this->pushColor(1, 1, 1);
					this->pushColor(1, 1, 1);
				}
				else {
					this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
				}
			}
			else {
				num1 = (float)atoi(v1.c_str() + tmp1);
				num2 = (float)atoi(v2.c_str() + tmp2);
				num3 = (float)atoi(v3.c_str() + tmp3);
				if (num1 >= color.size() || num2 >= color.size() || num3 >= color.size()) {
					this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
				}
				else {
					this->pushColor(color[(int)num1 * 3 - 3], color[(int)num1 * 3 - 2], color[(int)num1 * 3 - 1]);
					this->pushColor(color[(int)num2 * 3 - 3], color[(int)num2 * 3 - 2], color[(int)num2 * 3 - 1]);
					this->pushColor(color[(int)num3 * 3 - 3], color[(int)num3 * 3 - 2], color[(int)num3 * 3 - 1]);
				}
			}

			tmp1 = v1.find_first_of('/', tmp1) + 1;
			tmp2 = v2.find_first_of('/', tmp2) + 1;
			tmp3 = v3.find_first_of('/', tmp3) + 1;
			if (tmp1 == 0 || tmp2 == 0 || tmp3 == 0) {
				int p1 = atoi(v1.c_str());
				int p2 = atoi(v2.c_str());
				int p3 = atoi(v3.c_str());
				glm::vec3 edge1(pos[p2 * 3 - 3] - pos[p1 * 3 - 3],
					pos[p2 * 3 - 2] - pos[p1 * 3 - 2],
					pos[p2 * 3 - 1] - pos[p1 * 3 - 1]);
				glm::vec3 edge2(pos[p3 * 3 - 3] - pos[p2 * 3 - 3],
					pos[p3 * 3 - 2] - pos[p2 * 3 - 2],
					pos[p3 * 3 - 1] - pos[p2 * 3 - 1]);
				glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
					edge2.z*edge1.x - edge1.z*edge2.x,
					edge2.x*edge1.y - edge1.x*edge2.y);
				this->pushNormal(norm.x, norm.y, norm.z);
				this->pushNormal(norm.x, norm.y, norm.z);
				this->pushNormal(norm.x, norm.y, norm.z);
			}
			else {
				num1 = (float)atoi(v1.c_str() + tmp1);
				num2 = (float)atoi(v2.c_str() + tmp2);
				num3 = (float)atoi(v3.c_str() + tmp3);
				this->pushNormal(normal[(int)num1 * 3 - 3], normal[(int)num1 * 3 - 2], normal[(int)num1 * 3 - 1]);
				this->pushNormal(normal[(int)num2 * 3 - 3], normal[(int)num2 * 3 - 2], normal[(int)num2 * 3 - 1]);
				this->pushNormal(normal[(int)num3 * 3 - 3], normal[(int)num3 * 3 - 2], normal[(int)num3 * 3 - 1]);
			}
		}
		else if (op == "mtllib") {
			std::ifstream min;
			string path = string(filename);
			unsigned int tmp = path.find_last_of('/');
			for (unsigned int i = path.length() - 1; i > tmp; i--)
				path.pop_back();
			fin >> op;
			path += op;

			min.open(path);
			if (min.is_open() == FALSE)continue;
			while (min >> op) {
				if (op == "newmtl") {
					surface.push_back(Material());
					min >> op;
					surface[surface.size() - 1].name = op;
				}
				else if (op == "Kd") {
					min >> num1 >> num2 >> num3;
					surface[surface.size() - 1].kd = glm::vec3(num1, num2, num3);
				}
				else if (op == "Ka") {
					min >> num1 >> num2 >> num3;
					surface[surface.size() - 1].ka = glm::vec3(num1, num2, num3);
				}
				else if (op == "Ks") {
					min >> num1 >> num2 >> num3;
					surface[surface.size() - 1].ks = glm::vec3(num1, num2, num3);
				}
				else {
					char *buf = new char[256];
					min.getline(buf, 256);
					delete buf;
				}
			}
		}
		else if (op == "usemtl") {
			fin >> op;
			active = -1;
			for (unsigned int i = 0; i < surface.size(); i++) {
				if (surface[i].name == op) {
					active = i;
					break;
				}
			}
		}
		else {
			char *buf = new char[256];
			fin.getline(buf, 256);
			delete buf;
		}
	}
	return this;
}
void Element::shadow() {
	if (pos.size() == 0)return;
	if (pure)return;

	shadowShader->use();
	shadowShader->setMat4("u_modelMatrix", model);

	glBindVertexArray(svao);
	glBindBuffer(GL_ARRAY_BUFFER, spositionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * 4, getPos(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindVertexArray(0);

	glBindVertexArray(svao);
	glDrawArrays(GL_TRIANGLES, 0, pos.size() / 3);
}
void Element::show() {
	if (pos.size() == 0)return;
	if (enable == false)return;
	elementShader->use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * 4, getPos(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, color.size() * 4, getColor(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, normal.size() * 4, getNormal(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindVertexArray(0);

	if(diy){
		for (int i = 0; i < light.size(); i++) {
			char tmp[64];
			sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
			elementShader->setVec3(tmp, kd * light[i]->diffuse);
			sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
			elementShader->setVec3(tmp, ka * light[i]->ambient);
			sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
			elementShader->setFloat(tmp, ks * light[i]->specular);
		}
	}

	elementShader->setInt("u_enLight", !pure);
	elementShader->setMat4("u_modelMatrix", model);
	glm::mat4x4 inv = glm::transpose(glm::inverse(model));
	elementShader->setMat4("u_normalMatrix", inv);
	for (int i = 0; i < light.size(); i++) {
		char tmp[64];
		sprintf(tmp, "u_shadowMap[%d]", i);
		elementShader->setInt(tmp, i+1);
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, pos.size() / 3);

	if (diy) {
		for (int i = 0; i < light.size(); i++) {
			char tmp[64];
			sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
			elementShader->setVec3(tmp, light[i]->diffuse);
			sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
			elementShader->setVec3(tmp, light[i]->ambient);
			sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
			elementShader->setFloat(tmp, light[i]->specular);
		}
	}
}

void Texture::pic(const char *fileName) {
	CImage *img = new CImage;
	if (!fileName) {
		return;
	}
	HRESULT hr = img->Load(fileName);
	if (!SUCCEEDED(hr)) {
		return;
	}
	Bitmap bmp;
	bmp.sizeX = img->GetWidth();
	bmp.sizeY = img->GetHeight();
	if (img->GetPitch()<0)bmp.data = (unsigned char *)img->GetBits() + (img->GetPitch()*(img->GetHeight() - 1));
	else bmp.data = (unsigned char *)img->GetBits();

	src.push_back(bmp);
}
Texture *Texture::load(const char *filename) {
	std::ifstream fin;

	fin.open(filename);
	if (fin.is_open() == FALSE)return NULL;

	string op;
	float num1, num2, num3;

	vector<float>pos;
	vector<float>coord;
	vector<float>normal;

	vector<string> texName;
	vector<string> texDir;

	while (fin >> op) {
		if (op == "v") {
			fin >> num1 >> num2 >> num3;
			pos.push_back(num1);
			pos.push_back(num3);
			pos.push_back(num2);
		}
		else if (op == "vt") {
			fin >> num1 >> num2;
			coord.push_back(num1);
			coord.push_back(num2);
		}
		else if (op == "vn") {
			fin >> num1 >> num2 >> num3;
			normal.push_back(num1);
			normal.push_back(num2);
			normal.push_back(num3);
		}
		else if (op == "f") {
			string v1, v2, v3;
			int tmp1 = 0, tmp2 = 0, tmp3 = 0;

			fin >> v1 >> v2 >> v3;
			num1 = (float)atoi(v1.c_str() + tmp1);
			num2 = (float)atoi(v2.c_str() + tmp2);
			num3 = (float)atoi(v3.c_str() + tmp3);
			this->pushPos(pos[(int)num1 * 3 - 3], pos[(int)num1 * 3 - 2], pos[(int)num1 * 3 - 1]);
			this->pushPos(pos[(int)num2 * 3 - 3], pos[(int)num2 * 3 - 2], pos[(int)num2 * 3 - 1]);
			this->pushPos(pos[(int)num3 * 3 - 3], pos[(int)num3 * 3 - 2], pos[(int)num3 * 3 - 1]);

			tmp1 = v1.find_first_of('/', tmp1) + 1;
			tmp2 = v2.find_first_of('/', tmp2) + 1;
			tmp3 = v3.find_first_of('/', tmp3) + 1;
			if (tmp1 == 0 || tmp2 == 0 || tmp3 == 0) {
				this->pushCoord();
				this->pushCoord();
				this->pushCoord();
			}
			else {
				num1 = (float)atoi(v1.c_str() + tmp1);
				num2 = (float)atoi(v2.c_str() + tmp2);
				num3 = (float)atoi(v3.c_str() + tmp3);
				this->pushCoord(coord[(int)num1 * 2 - 2], coord[(int)num1 * 2 - 1]);
				this->pushCoord(coord[(int)num2 * 2 - 2], coord[(int)num2 * 2 - 1]);
				this->pushCoord(coord[(int)num3 * 2 - 2], coord[(int)num3 * 2 - 1]);
			}

			tmp1 = v1.find_first_of('/', tmp1) + 1;
			tmp2 = v2.find_first_of('/', tmp2) + 1;
			tmp3 = v3.find_first_of('/', tmp3) + 1;
			if (tmp1 == 0 || tmp2 == 0 || tmp3 == 0) {
				int p1 = atoi(v1.c_str());
				int p2 = atoi(v2.c_str());
				int p3 = atoi(v3.c_str());
				glm::vec3 edge1(pos[p2 * 3 - 3] - pos[p1 * 3 - 3],
					pos[p2 * 3 - 2] - pos[p1 * 3 - 2],
					pos[p2 * 3 - 1] - pos[p1 * 3 - 1]);
				glm::vec3 edge2(pos[p3 * 3 - 3] - pos[p2 * 3 - 3],
					pos[p3 * 3 - 2] - pos[p2 * 3 - 2],
					pos[p3 * 3 - 1] - pos[p2 * 3 - 1]);
				glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
					edge2.z*edge1.x - edge1.z*edge2.x,
					edge2.x*edge1.y - edge1.x*edge2.y);
				this->pushNormal(norm.x, norm.y, norm.z);
				this->pushNormal(norm.x, norm.y, norm.z);
				this->pushNormal(norm.x, norm.y, norm.z);
			}
			else {
				num1 = (float)atoi(v1.c_str() + tmp1);
				num2 = (float)atoi(v2.c_str() + tmp2);
				num3 = (float)atoi(v3.c_str() + tmp3);
				this->pushNormal(normal[(int)num1 * 3 - 3], normal[(int)num1 * 3 - 2], normal[(int)num1 * 3 - 1]);
				this->pushNormal(normal[(int)num2 * 3 - 3], normal[(int)num2 * 3 - 2], normal[(int)num2 * 3 - 1]);
				this->pushNormal(normal[(int)num3 * 3 - 3], normal[(int)num3 * 3 - 2], normal[(int)num3 * 3 - 1]);
			}
		}
		else if (op == "mtllib") {
			std::ifstream min;
			string path = string(filename);
			unsigned int tmp = path.find_last_of('/');
			for (unsigned int i = path.length() - 1; i > tmp; i--)
				path.pop_back();
			fin >> op;
			path += op;

			min.open(path);
			if (min.is_open() == FALSE)continue;

			bool complete = true;
			while (min >> op) {
				if (op == "newmtl") {
					if (!complete)texDir.push_back("");
					min >> op;
					texName.push_back(op);
					complete = false;
				}
				else if (op == "src") {
					if (complete)texName.push_back("");
					min >> op;
					texDir.push_back(op);
					complete = true;
				}
				else continue;
			}
		}
		else if (op == "usemtl") {
			fin >> op;
			for (unsigned int i = 0; i < texName.size(); i++) {
				if (op == texName[i]) {
					pic(texDir[i].c_str());
					break;
				}
			}
		}
		else {
			char *buf = new char[256];
			fin.getline(buf, 256);
			delete buf;
		}
	}
	
	return this;
}
void Texture::shadow() {
	if (pos.size() == 0)return;
	if (pure)return;
	if (enable == false)return;
	shadowShader->use();
	shadowShader->setMat4("u_modelMatrix", model);

	glBindVertexArray(svao);
	glBindBuffer(GL_ARRAY_BUFFER, spositionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * 4, getPos(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindVertexArray(0);

	glBindVertexArray(svao);
	glDrawArrays(GL_TRIANGLES, 0, pos.size() / 3);
}
void Texture::show() {
	if (pos.size() == 0)return;
	if (enable == false)return;
	texShader->use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * 4, getPos(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, coordBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, coord.size() * 4, getCoord(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, normal.size() * 4, getNormal(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, src[0].sizeX, src[0].sizeY, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, src[0].data);
	
	if(diy){
		for (int i = 0; i < light.size(); i++) {
			char tmp[64];
			sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
			texShader->setVec3(tmp, kd * light[i]->diffuse);
			sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
			texShader->setVec3(tmp, ka * light[i]->ambient);
			sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
			texShader->setFloat(tmp, ks * light[i]->specular);
		}
	}

	texShader->setInt("u_enLight", !pure);
	texShader->setMat4("u_modelMatrix", model);
	glm::mat4x4 inv = glm::transpose(glm::inverse(model));
	elementShader->setMat4("u_normalMatrix", inv);
	for (int i = 0; i < light.size(); i++) {
		char tmp[64];
		sprintf(tmp, "u_shadowMap[%d]", i);
		texShader->setInt(tmp, i + 1);
	}
	texShader->setInt("u_textureMap", 0);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, pos.size() / 3);

	if (diy) {
		for (int i = 0; i < light.size(); i++) {
			char tmp[64];
			sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
			texShader->setVec3(tmp, light[i]->diffuse);
			sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
			texShader->setVec3(tmp, light[i]->ambient);
			sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
			texShader->setFloat(tmp, light[i]->specular);
		}
	}
}

void Mix::pic(const char *fileName) {
	CImage *img = new CImage;
	if (!fileName) {
		return;
	}
	HRESULT hr = img->Load(fileName);
	if (!SUCCEEDED(hr)) {
		return;
	}
	Bitmap bmp;
	bmp.sizeX = img->GetWidth();
	bmp.sizeY = img->GetHeight();
	if (img->GetPitch()<0)bmp.data = (unsigned char *)img->GetBits() + (img->GetPitch()*(img->GetHeight() - 1));
	else bmp.data = (unsigned char *)img->GetBits();

	src.push_back(bmp);
}
Mix *Mix::load(const char *filename) {
	std::ifstream fin;

	fin.open(filename);
	if (fin.is_open() == FALSE)return NULL;

	string op;
	float num1, num2, num3;

	vector<float>pos;
	vector<float>color;
	vector<float>coord;
	vector<float>normal;

	vector<string> texName;
	vector<string> texDir;

	bool tex = false;

	while (fin >> op) {
		if (op == "v") {
			fin >> num1 >> num2 >> num3;
			pos.push_back(num1);
			pos.push_back(num3);
			pos.push_back(num2);
		}
		else if (op == "vc") {
			fin >> num1 >> num2 >> num3;
			color.push_back(num1);
			color.push_back(num2);
			color.push_back(num3);
		}
		else if (op == "vt") {
			fin >> num1 >> num2;
			coord.push_back(num1);
			coord.push_back(num2);
		}
		else if (op == "vn") {
			fin >> num1 >> num2 >> num3;
			normal.push_back(num1);
			normal.push_back(num2);
			normal.push_back(num3);
		}
		else if (op == "mtllib") {
			std::ifstream min;
			string path = string(filename);
			unsigned int tmp = path.find_last_of('/');
			for (unsigned int i = path.length() - 1; i > tmp; i--)
				path.pop_back();
			fin >> op;
			path += op;

			min.open(path);
			if (min.is_open() == FALSE)continue;

			bool complete = true;
			while (min >> op) {
				if (op == "newmtl") {
					surface.push_back(Material());
					min >> op;
					surface[surface.size() - 1].name = op;

					if (!complete)texDir.push_back("");
					min >> op;
					texName.push_back(op);
					complete = false;
				}
				else if (op == "Kd") {
					min >> num1 >> num2 >> num3;
					surface[surface.size() - 1].kd = glm::vec3(num1, num2, num3);
				}
				else if (op == "Ka") {
					min >> num1 >> num2 >> num3;
					surface[surface.size() - 1].ka = glm::vec3(num1, num2, num3);
				}
				else if (op == "Ks") {
					min >> num1 >> num2 >> num3;
					surface[surface.size() - 1].ks = glm::vec3(num1, num2, num3);
				}
				else if (op == "map_Kd") {
					if (complete)texName.push_back("");
					min >> op;
					texDir.push_back(op);
					complete = true;
				}
				else {
					char *buf = new char[256];
					min.getline(buf, 256);
					delete buf;
				}
			}
		}
		else if (op == "usemtl") {
			fin >> op;

			active = -1;
			for (unsigned int i = 0; i < surface.size(); i++) {
				if (surface[i].name == op) {
					active = i;
					break;
				}
			}

			for (unsigned int i = 0; i < texName.size(); i++) {
				if (op == texName[i]) {
					pic(texDir[i].c_str());
					break;
				}
			}
		}
		else if (op == "f") {
			string v1, v2, v3;
			int tmp1 = 0, tmp2 = 0, tmp3 = 0;

			fin >> v1 >> v2 >> v3;
			num1 = (float)atoi(v1.c_str() + tmp1);
			num2 = (float)atoi(v2.c_str() + tmp2);
			num3 = (float)atoi(v3.c_str() + tmp3);
			this->element.pushPos(pos[(int)num1 * 3 - 3], pos[(int)num1 * 3 - 2], pos[(int)num1 * 3 - 1]);
			this->element.pushPos(pos[(int)num2 * 3 - 3], pos[(int)num2 * 3 - 2], pos[(int)num2 * 3 - 1]);
			this->element.pushPos(pos[(int)num3 * 3 - 3], pos[(int)num3 * 3 - 2], pos[(int)num3 * 3 - 1]);
			this->texture.pushPos(pos[(int)num1 * 3 - 3], pos[(int)num1 * 3 - 2], pos[(int)num1 * 3 - 1]);
			this->texture.pushPos(pos[(int)num2 * 3 - 3], pos[(int)num2 * 3 - 2], pos[(int)num2 * 3 - 1]);
			this->texture.pushPos(pos[(int)num3 * 3 - 3], pos[(int)num3 * 3 - 2], pos[(int)num3 * 3 - 1]);

			tmp1 = v1.find_first_of('/', tmp1) + 1;
			tmp2 = v2.find_first_of('/', tmp2) + 1;
			tmp3 = v3.find_first_of('/', tmp3) + 1;
			if (tmp1 == 0 || tmp2 == 0 || tmp3 == 0) {
				this->texture.pushCoord();
				this->texture.pushCoord();
				this->texture.pushCoord();
			}
			else {
				num1 = (float)atoi(v1.c_str() + tmp1);
				num2 = (float)atoi(v2.c_str() + tmp2);
				num3 = (float)atoi(v3.c_str() + tmp3);
				this->texture.pushCoord(coord[(int)num1 * 2 - 2], coord[(int)num1 * 2 - 1]);
				this->texture.pushCoord(coord[(int)num2 * 2 - 2], coord[(int)num2 * 2 - 1]);
				this->texture.pushCoord(coord[(int)num3 * 2 - 2], coord[(int)num3 * 2 - 1]);
			}

			tmp1 = v1.find_first_of('/', tmp1) + 1;
			tmp2 = v2.find_first_of('/', tmp2) + 1;
			tmp3 = v3.find_first_of('/', tmp3) + 1;
			if (tmp1 == 0 || tmp2 == 0 || tmp3 == 0) {
				int p1 = atoi(v1.c_str());
				int p2 = atoi(v2.c_str());
				int p3 = atoi(v3.c_str());
				glm::vec3 edge1(pos[p2 * 3 - 3] - pos[p1 * 3 - 3],
					pos[p2 * 3 - 2] - pos[p1 * 3 - 2],
					pos[p2 * 3 - 1] - pos[p1 * 3 - 1]);
				glm::vec3 edge2(pos[p3 * 3 - 3] - pos[p2 * 3 - 3],
					pos[p3 * 3 - 2] - pos[p2 * 3 - 2],
					pos[p3 * 3 - 1] - pos[p2 * 3 - 1]);
				glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
					edge2.z*edge1.x - edge1.z*edge2.x,
					edge2.x*edge1.y - edge1.x*edge2.y);
				this->element.pushNormal(norm.x, norm.y, norm.z);
				this->element.pushNormal(norm.x, norm.y, norm.z);
				this->element.pushNormal(norm.x, norm.y, norm.z);
				this->texture.pushNormal(norm.x, norm.y, norm.z);
				this->texture.pushNormal(norm.x, norm.y, norm.z);
				this->texture.pushNormal(norm.x, norm.y, norm.z);
			}
			else {
				num1 = (float)atoi(v1.c_str() + tmp1);
				num2 = (float)atoi(v2.c_str() + tmp2);
				num3 = (float)atoi(v3.c_str() + tmp3);
				this->element.pushNormal(normal[(int)num1 * 3 - 3], normal[(int)num1 * 3 - 2], normal[(int)num1 * 3 - 1]);
				this->element.pushNormal(normal[(int)num2 * 3 - 3], normal[(int)num2 * 3 - 2], normal[(int)num2 * 3 - 1]);
				this->element.pushNormal(normal[(int)num3 * 3 - 3], normal[(int)num3 * 3 - 2], normal[(int)num3 * 3 - 1]);
				this->texture.pushNormal(normal[(int)num1 * 3 - 3], normal[(int)num1 * 3 - 2], normal[(int)num1 * 3 - 1]);
				this->texture.pushNormal(normal[(int)num2 * 3 - 3], normal[(int)num2 * 3 - 2], normal[(int)num2 * 3 - 1]);
				this->texture.pushNormal(normal[(int)num3 * 3 - 3], normal[(int)num3 * 3 - 2], normal[(int)num3 * 3 - 1]);
			}
		}
	}
	return this;
}
void Mix::shadow() {
	element.shadow();
	texture.shadow();
}
void Mix::show() {
	element.show();
	texture.show();
}

void Scene::shadow() {
	for (auto &e : elements)e->shadow();
	for (auto &t : textures)t->shadow();
	for (auto &m : mixes)m->shadow();
}
void Scene::show() {
	for (auto &e : elements)
		e->show();
	for (auto &t : textures)
		t->show();
}


void Element::addBall(float radius, float slice, float R, float G, float B) {
	if (slice < 1.f)return;

	float angleSpan = 45.f/slice;
	vector<float>tmp;
	for (float i = -90; i <= 90; i += angleSpan) {
		for (float j = 0; j < 360; j += angleSpan) {
			float r = radius*glm::cos(glm::radians(i));
			float x = r*glm::cos(glm::radians(j));
			float y = radius*glm::sin(glm::radians(i));
			float z = r*glm::sin(glm::radians(j));
			tmp.push_back(x);
			tmp.push_back(y);
			tmp.push_back(z);
		}
	}

	int row = 180 / angleSpan + 1;
	int col = 360 / angleSpan;
	int k = col*(row - 2) * 6 * 8;
	int count = 0;
	for (int i = 0; i < row; i++) {
		if (i != 0 && i != row - 1) {
			for (int j = 0; j < col; j++) {
				k = i*col + j;
				glm::vec3 norm = glm::normalize(glm::vec3(tmp[(k + col) * 3], tmp[(k + col) * 3 + 1], tmp[(k + col) * 3 + 2]));
				pushPos(tmp[(k + col) * 3], tmp[(k + col) * 3 + 1], tmp[(k + col) * 3 + 2]);
				pushColor(R, G, B);
				pushNormal(norm.x, norm.y, norm.z);

				int index = k + 1;
				if (j == col - 1)
					index -= col;
				norm = glm::normalize(glm::vec3(tmp[index * 3], tmp[index * 3 + 1], tmp[index * 3 + 2]));
				pushPos(tmp[index * 3], tmp[index * 3 + 1], tmp[index * 3 + 2]);
				pushColor(R, G, B);
				pushNormal(norm.x, norm.y, norm.z);

				norm = glm::normalize(glm::vec3(tmp[k * 3], tmp[k * 3 + 1], tmp[k * 3 + 2]));
				pushPos(tmp[k * 3], tmp[k * 3 + 1], tmp[k * 3 + 2]);
				pushColor(R, G, B);
				pushNormal(norm.x, norm.y, norm.z);
			}
			for (int j = 0; j < col; j++) {
				k = i*col + j;
				glm::vec3 norm = glm::normalize(glm::vec3(tmp[(k - col) * 3], tmp[(k - col) * 3 + 1], tmp[(k - col) * 3 + 2]));
				pushPos(tmp[(k - col) * 3], tmp[(k - col) * 3 + 1], tmp[(k - col) * 3 + 2]);
				pushColor(R, G, B);
				pushNormal(norm.x, norm.y, norm.z);

				int index = k - 1;
				if (j == 0)
					index += col;
				norm = glm::normalize(glm::vec3(tmp[index * 3], tmp[index * 3 + 1], tmp[index * 3 + 2]));
				pushPos(tmp[index * 3], tmp[index * 3 + 1], tmp[index * 3 + 2]);
				pushColor(R, G, B);
				pushNormal(norm.x, norm.y, norm.z);

				norm = glm::normalize(glm::vec3(tmp[k * 3], tmp[k * 3 + 1], tmp[k * 3 + 2]));
				pushPos(tmp[k * 3], tmp[k * 3 + 1], tmp[k * 3 + 2]);
				pushColor(R, G, B);
				pushNormal(norm.x, norm.y, norm.z);
			}
		}
	}
}


