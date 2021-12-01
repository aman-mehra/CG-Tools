#include "ray.h"

bool Ray::setParameter(const float par, const  Vector3D& norm, const Object *obj)
{
	if(par < t && par > SMALLEST_DIST)
	{
		hit = true;
		t = par;
		object = obj;
		normal = norm;
		normal.normalize();
		return true;
	}
	return false;
}
