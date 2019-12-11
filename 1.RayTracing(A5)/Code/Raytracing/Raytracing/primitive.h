#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include"color.h"
#include"vector3.h"
#include"bmp.h"
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include "Eigen/Dense"

extern const double EPS;
extern const double PI;
const double BIG_DIST = 1e100;

class Blur {
public:
	virtual std::pair<double, double> GetXY() = 0;
};

class ExpBlur : public Blur {
public:
	std::pair<double, double> GetXY();
};

class Material {
public:
	Color color , absor;
	double refl , refr;
	double diff , spec;
	double rindex;
	double drefl;
	Bmp* texture;
	Blur* blur;

	Material();
	~Material() {}

	void Input( std::string , std::stringstream& );
};

struct CollidePrimitive;

class Primitive {
protected:
	int sample;
	Material* material;
	Primitive* next;

public:

	Primitive();
	Primitive( const Primitive& );
	~Primitive();
	
	int GetSample() { return sample; }
	Material* GetMaterial() { return material; }
	Primitive* GetNext() { return next; }
	void SetNext( Primitive* primitive ) { next = primitive; }

	virtual void Input( std::string , std::stringstream& );
	virtual CollidePrimitive Collide( Vector3 ray_O , Vector3 ray_V ) = 0;
	virtual Color GetTexture(CollidePrimitive* collision) = 0;
	virtual bool IsLightPrimitive(){return false;}
};

struct CollidePrimitive {
	bool isCollide;
	Primitive* collide_primitive;
	Vector3 N , C;//C is cross point, N is normal
	double dist;//
	bool front;// render the front face
	bool isAccurate;
	double u, v;
	CollidePrimitive() { isCollide = false; collide_primitive = NULL; dist = BIG_DIST; isAccurate = true; }
	Color GetTexture(){return collide_primitive->GetTexture(this);}
};

class Sphere : public Primitive {
protected:
	Vector3 O , De , Dc;//TODO: what is De, DC? related to texture coordinates
	double R;

public:
	Sphere();
	~Sphere() {}

	void Input( std::string , std::stringstream& );
	CollidePrimitive Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture(CollidePrimitive* collision);
};

class SphereLightPrimitive : public Sphere{
public:
	SphereLightPrimitive(Vector3 pO, double pR, Color color) : Sphere()
	{O = pO; R = pR; material->color = color; }
	bool IsLightPrimitive(){return true;}
};

class Plane : public Primitive {
protected:
	Vector3 N , Dx , Dy;
	double R;// In Ax+By+Cz+d=dot(P-P0,N)=0, -d=R*|N|^2=dot(P0,N)

public:
	Plane() : Primitive() {}
	Plane(Vector3 pN, Vector3 pDx, Vector3 pDy, double pR) : Primitive() { N = pN; Dx = pDx; Dy = pDy; R = pR; }
	~Plane() {}

	void Input( std::string , std::stringstream& );
	CollidePrimitive Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture(CollidePrimitive* collision);
};

class Square : public Primitive {
protected:
	Vector3 O , Dx , Dy;

public:
	Square() : Primitive() {}
	~Square() {}

	void Input( std::string , std::stringstream& );
	CollidePrimitive Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture(CollidePrimitive* collision);
};

class PlaneAreaLightPrimitive : public Square{
public:
	PlaneAreaLightPrimitive(Vector3 pO, Vector3 pDx, Vector3 pDy, Color color): Square()
	{O = pO; Dx = pDx; Dy = pDy; material->color = color; }
	bool IsLightPrimitive(){return true;}
};

class Cylinder : public Primitive {
	Vector3 O1, O2;
	double R;
	Vector3 N, Nx, Ny;

public:
	Cylinder() : Primitive() {}
	Cylinder(Vector3 pO1, Vector3 pO2, double pR) : Primitive() {O1 = pO1; O2 = pO2; R = pR; }
	~Cylinder() {}

	void Input( std::string , std::stringstream& );
	CollidePrimitive Collide( Vector3 ray_O , Vector3 ray_V );
    CollidePrimitive SideCollide( Vector3 ray_O , Vector3 ray_V );
    CollidePrimitive BaseCollide( Vector3 ray_O , Vector3 ray_V , bool isO1 );
	Color GetTexture(CollidePrimitive* collision);
};

class Bezier : public Primitive {
	Vector3 O1, O2;
	Vector3 N, Nx, Ny;
	std::vector<double> R;
	std::vector<double> Z;
	int degree;
	Cylinder* boundingCylinderInner;
	Cylinder* boundingCylinderOuter;

public:
	Bezier() : Primitive() { boundingCylinderInner = NULL; boundingCylinderOuter = NULL; degree = -1; }
	~Bezier() {}

	void Input( std::string , std::stringstream& );
	CollidePrimitive Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture(CollidePrimitive* collision);

private:
    double B(double u, int n, int i);//coefficients b_{i,n}(u)
    double P(double u, std::vector<double> ctrlPoints);
    double dP(double u, std::vector<double> ctrlPoints);
    Vector3 C(double t, Vector3 ray_O , Vector3 ray_V);
    Vector3 S(double u, double theta);
    Vector3 F(double t, double u, double theta, Vector3 ray_O , Vector3 ray_V);
    Eigen::Matrix3d dF(double t, double u, double theta, Vector3 ray_V);
    CollidePrimitive CollideWithParams( Vector3 ray_O , Vector3 ray_V , double t, double u, double theta);
};

#endif
