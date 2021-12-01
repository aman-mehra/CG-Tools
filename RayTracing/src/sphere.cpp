//sphere.cpp

#include "sphere.h"

bool Sphere::intersect(Ray& r) const
{
	Vector3D O = r.getOrigin();
	Vector3D Dir = r.getDirection();

	Vector3D centerVector = O - position;
	double a = 1.0;
	double b = 2*dotProduct(Dir, centerVector);
	double c = dotProduct(centerVector, centerVector) - radius*radius;
	double discriminant = b*b - 4.0*a*c;

	Vector3D norm;

	//now check if discr. is posivtive or zero, then only we have an intersection!
	if(discriminant >=0.0)
	{

		if(discriminant == 0)
		{
			double t;
			t = -b/(2.0*a);
			norm = (O + (t*Dir)) - position;
			r.setParameter(t, norm, this);
			return true;
		}
		else
		{
			//Calculate both roots
			double D = sqrt(discriminant);
			double t1 = (-b +D)/(2.0*a);
			double t2 = (-b -D)/(2.0*a);

			norm = (O + (t1*Dir)) - position;
			bool b1 = r.setParameter(t1, norm, this);
			norm = (O + (t2*Dir)) - position;
			bool b2 = r.setParameter(t2, norm, this);

			return b1||b2;    
		}
	}
	return false;

}
