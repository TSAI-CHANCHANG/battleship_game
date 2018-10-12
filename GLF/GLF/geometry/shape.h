#ifndef GLF_GEOMETRY_SHAPE
#define GLF_GEOMETRY_SHAPE
#include "main.h"
#include "arith.h"
#include "ray.h"
#include "box.h"

class Triangle {

};
class Square {
private:
	Point p1, p2, p3, p4;
	int widthPixel, heightPixel;
	Vector norm;
public:
	Square() {};
	Square(Point p1, Point p2, Point p3, Point p4, int width = 100, int height = 100) :
		p1(p1), p2(p2), p3(p3), p4(p4), widthPixel(width), heightPixel(height) {
		norm = cross(p2 - p1, p3 - p1);
	};
	~Square() {};

	Box bound();
	glm::vec2 intersect(Ray r);
	Point coord(glm::vec2 c);
	glm::mat4 model();
};

#endif