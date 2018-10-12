#ifndef GLF_EYE
#define GLF_EYE
#include "main.h"
#include "geometry/ray.h"

class Eye {
public:
	Eye();
	Eye(glm::vec3 pos);
	~Eye();

	glm::vec3 pos, dir, up;
	float theta, phy;
	float dist;

	glm::mat4 model;

	glm::mat4 view();
	void move(float dx, float dy);
	void rotate(float angx, float angy);
	void zoom(float prop);
	Ray ray(int x, int y);
};

#endif
