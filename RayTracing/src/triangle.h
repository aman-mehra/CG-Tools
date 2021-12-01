//triangle.h
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object.h"
#include "ray.h"
#include "vector3D.h"
#include "color.h"

class Triangle : public Object
{
private:
	Vector3D p1;
	Vector3D p2;
	Vector3D p3;

public:
	Triangle(const Vector3D& _p1, const Vector3D& _p2, const Vector3D& _p3, Material* mat):
		Object(mat), p1(_p1), p2(_p2), p3(_p3)
	{
		isSolid = true;
	}
	
	virtual bool intersect(Ray& r) const;

};

double determinant(double mat[]);
#endif
