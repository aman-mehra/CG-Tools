//triangle.cpp

#include "triangle.h"

double determinant(double mat[]){
    double det = mat[0]*(mat[4]*mat[8]-mat[7]*mat[5]) - mat[3]*(mat[1]*mat[8]-mat[7]*mat[2]) + mat[6]*(mat[1]*mat[5]-mat[2]*mat[4]);
    return det;
}

bool Triangle::intersect(Ray& r) const
{
	Vector3D x_e = r.getOrigin();
	Vector3D x_d = r.getDirection();

	double A[] = {
    	p1.X()-p2.X(), p1.Y()-p2.Y(), p1.Z()-p2.Z(), // first column
    	p1.X()-p3.X(), p1.Y()-p3.Y(), p1.Z()-p3.Z(), // second column
    	x_d.X(), x_d.Y(), x_d.Z()// third column
    	};

    double det_A = determinant(A);

    double D1[] = {
    	p1.X()-x_e.X(), p1.Y()-x_e.Y(), p1.Z()-x_e.Z(), // first column
    	p1.X()-p3.X(), p1.Y()-p3.Y(), p1.Z()-p3.Z(), // second column
    	x_d.X(), x_d.Y(), x_d.Z()// third column
    	};

    double beta = determinant(D1)/det_A;

    double D2[] = {
    	p1.X()-p2.X(), p1.Y()-p2.Y(), p1.Z()-p2.Z(), // first column
    	p1.X()-x_e.X(), p1.Y()-x_e.Y(), p1.Z()-x_e.Z(), // second column
    	x_d.X(), x_d.Y(), x_d.Z()// third column
    	};

    double gamma = determinant(D2)/det_A;

    double D3[] = {
    	p1.X()-p2.X(), p1.Y()-p2.Y(), p1.Z()-p2.Z(), // first column
    	p1.X()-p3.X(), p1.Y()-p3.Y(), p1.Z()-p3.Z(), // second column
    	p1.X()-x_e.X(), p1.Y()-x_e.Y(), p1.Z()-x_e.Z(),// third column
    	};

    double t = determinant(D3)/det_A;

	Vector3D norm = crossProduct(p2-p1,p3-p1);

    if (dotProduct(norm,x_d) < 0){norm = -1*norm;}

	//now check if discr. is posivtive or zero, then only we have an intersection!
	if(beta + gamma <= 1.0)
	{

		if(beta >= 0 && gamma >= 0)
		{
			r.setParameter(t, norm, this);
			return true;
		}
	}
	return false;

}
