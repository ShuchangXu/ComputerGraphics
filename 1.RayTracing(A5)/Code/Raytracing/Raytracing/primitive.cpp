#include"primitive.h"
#include<sstream>
#include<float.h>
#include<cstdio>
#include<string>
#include<cmath>
#include<iostream>
#include<cstdlib>
#include <time.h>
#define sgn(x) ((x > 0) ? 1 : -1 )

const int BEZIER_MAX_DEGREE = 5;
const int Combination[BEZIER_MAX_DEGREE + 1][BEZIER_MAX_DEGREE + 1] =
{	1, 0, 0, 0, 0, 0,//degree = 0
	1, 1, 0, 0, 0, 0,
	1, 2, 1, 0, 0, 0,
	1, 3, 3, 1, 0, 0,
	1, 4, 6, 4, 1, 0,
	1, 5, 10,10,5, 1 //degree = 5
};

const int MAX_COLLIDE_TIMES = 10;
const int MAX_COLLIDE_RANDS = 10;
const int RANDOM_INITIALIZATION = 6;
const int MAX_NEWTON_ITERATION = 20;
const double RAND_AMP = 0.5;
const double learning_rate = 0.5;

double ran() {
	//srand((unsigned)time(NULL));
	return double(rand() % 32768) / 32768;
}


std::pair<double, double> ExpBlur::GetXY()
{
	double x,y;
	x = ran();
	x = pow(2, x)-1;
	y = rand();
	return std::pair<double, double>(x*cos(y),x*sin(y));
}

Material::Material() {
	color = absor = Color();
	refl = refr = 0;
	diff = spec = 0;
	rindex = 0;
	drefl = 0;
	texture = NULL;
	blur = new ExpBlur();
}

void Material::Input( std::string var , std::stringstream& fin ) {
	if ( var == "color=" ) color.Input( fin );
	if ( var == "absor=" ) absor.Input( fin );
	if ( var == "refl=" ) fin >> refl;
	if ( var == "refr=" ) fin >> refr;
	if ( var == "diff=" ) fin >> diff;
	if ( var == "spec=" ) fin >> spec;
	if ( var == "drefl=" ) fin >> drefl;
	if ( var == "rindex=" ) fin >> rindex;
	if ( var == "texture=" ) {
		std::string file; fin >> file;
		texture = new Bmp;
		texture->Input( file );
	}
	if ( var == "blur=" ) {
		std::string blurname; fin >> blurname;
		if(blurname == "exp")
			blur = new ExpBlur();
		else
			blur = new ExpBlur();
	}
}

Primitive::Primitive() {
	sample = rand();
	material = new Material;
	next = NULL;
}

Primitive::Primitive( const Primitive& primitive ) {
	*this = primitive;
	material = new Material;
	*material = *primitive.material;
}

Primitive::~Primitive() {
	delete material;
}

void Primitive::Input( std::string var , std::stringstream& fin ) {
	material->Input( var , fin );
}

Sphere::Sphere() : Primitive() {
	De = Vector3( 0 , 0 , 1 );
	Dc = Vector3( 0 , 1 , 0 );
}

void Sphere::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "De=" ) De.Input( fin );
	if ( var == "Dc=" ) Dc.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Sphere::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	Vector3 P = ray_O - O;
	double b = -P.Dot( ray_V );//actually this is -b/2
	double det = b * b - P.Module2() + R * R;//Actually this is det/4
	CollidePrimitive ret;

	if ( det > EPS ) {
		det = sqrt( det );
		double x1 = b - det  , x2 = b + det;

		if ( x2 < EPS ) return ret;
		if ( x1 > EPS ) {
			ret.dist = x1;
			ret.front = true;
		} else {
			ret.dist = x2;
			ret.front = false;
		} 
	} else 
		return ret;

	ret.C = ray_O + ray_V * ret.dist;
	ret.N = ( ret.C - O ).GetUnitVector();
	if ( ret.front == false ) ret.N = -ret.N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Sphere::GetTexture(CollidePrimitive* collision) {
	Vector3 crash_C = collision->C;
	Vector3 I = ( crash_C - O ).GetUnitVector();
	double a = acos( -I.Dot( De ) );
	double b = acos( std::min( std::max( I.Dot( Dc ) / sin( a ) , -1.0 ) , 1.0 ) );
	double u = a / PI , v = b / 2 / PI;
	if ( I.Dot( Dc * De ) < 0 ) v = 1 - v;
	return material->texture->GetSmoothColor( u , v );
}


void Plane::Input( std::string var , std::stringstream& fin ) {
	if ( var == "N=" ) N.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Plane::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	N = N.GetUnitVector();
	double d = N.Dot( ray_V );
	CollidePrimitive ret;
	if ( fabs( d ) < EPS ) return ret;
	double l = ( N * R - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return ret;

	ret.dist = l;
	ret.front = ( d < 0 );
	ret.C = ray_O + ray_V * ret.dist;
	ret.N = ( ret.front ) ? N : -N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Plane::GetTexture(CollidePrimitive* collision) {
	Vector3 crash_C = collision->C;
	double u = crash_C.Dot( Dx ) / Dx.Module2();
	double v = crash_C.Dot( Dy ) / Dy.Module2();
	return material->texture->GetSmoothColor( u , v );
}

void Square::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Square::Collide( Vector3 ray_O , Vector3 ray_V ) {
	CollidePrimitive ret;
	ray_V = ray_V.GetUnitVector();
	Vector3 N = (Dx * Dy).GetUnitVector();
	double d = N.Dot(ray_V);
	if (fabs(d) < EPS) return ret;
	double l = (O - ray_O).Dot(N) / d;
	if (l < EPS) return ret;

	Vector3 C = ray_O + ray_V * l;
	if (fabs(Dx.Dot(C - O)) > Dx.Dot(Dx)) return ret;
	if (fabs(Dy.Dot(C - O)) > Dy.Dot(Dy)) return ret;

	ret.dist = l;
	ret.front = (d < 0);
	ret.C = C;
	ret.N = (ret.front) ? N : -N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Square::GetTexture(CollidePrimitive* collision) {
	Vector3 crash_C = collision->C;
	double u = (crash_C - O).Dot( Dx ) / Dx.Module2() / 2 + 0.5;
	double v = (crash_C - O).Dot( Dy ) / Dy.Module2() / 2 + 0.5;
	return material->texture->GetSmoothColor( u , v );
}

void Cylinder::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O1=" ) O1.Input( fin );
	if ( var == "O2=" ) O2.Input( fin );
	if ( var == "R=" ) fin>>R; 
	if (var == "Axes") {
		N = (O2 - O1).GetUnitVector();
		Nx = N.GetAnVerticalVector();
		Ny = N * Nx;
	}
	Primitive::Input( var , fin );
}

CollidePrimitive Cylinder::SideCollide( Vector3 ray_O , Vector3 ray_V ) {
    ray_V = ray_V.GetUnitVector();
    Vector3 V = (O2 - O1).GetUnitVector();
    Vector3 P = ray_O - O1;
    
    Vector3 temp1 = ray_V - ray_V.Dot(V)*V;
    Vector3 temp2 = P - P.Dot(V)*V;
    double a = temp1.Module2();
    double b = temp1.Dot(temp2);//actually this is b/2
    double c = temp2.Module2() - R * R;
    double det = b * b - a * c;//Actually this is det/4
    CollidePrimitive ret;
    
    double dist = 0.0;
    bool front = false;
	Vector3 C;
    
    if(det > EPS){
        det = sqrt(det)/a;
        double x1 = - b/a - det , x2 = - b/a + det;
        if(x2 < EPS) return ret;
        if(x1 > EPS) {
			dist = x1;
			C = ray_O + ray_V * dist;
			front = true;
			if (V.Dot(C - O1) < EPS || V.Dot(C - O2) > EPS) {
				dist = x2;
				C = ray_O + ray_V * dist;
				front = false;
				if (V.Dot(C - O1) < EPS || V.Dot(C - O2) > EPS) {
					return ret;
				}
			}
        } else {
            dist = x2;
			C = ray_O + ray_V * dist;
            front = false;
			if (V.Dot(C - O1) < EPS || V.Dot(C - O2) > EPS) {
				return ret;
			}
        }
    }else
        return ret;
    
    ret.dist = dist;
    ret.front = front;
    ret.C = C;
    Vector3 O1C = ret.C - O1;
    ret.N = (O1C - O1C.Dot(V)*V).GetUnitVector();
    if ( ret.front == false ) ret.N = -ret.N;
    ret.isCollide = true;
    ret.collide_primitive = this;
    
	return ret;
}

CollidePrimitive Cylinder::BaseCollide( Vector3 ray_O , Vector3 ray_V , bool isO1) {
    CollidePrimitive ret;
    ray_V = ray_V.GetUnitVector();
    Vector3 O = isO1 ? O1 : O2;
    Vector3 V = (O2 - O1).GetUnitVector();
    double d = V.Dot(ray_V);
    if ( fabs( d ) < EPS ) return ret;
    double t = V.Dot(O - ray_O) / d;
    if(t < EPS) return ret;
    Vector3 C = ray_O + t * ray_V;
    if( (C - O).Module2() - R * R  > EPS ) return ret;
    
    ret.dist = t;
    ret.C = C;
    int front = sgn(d)* (isO1 ? 1: -1);
    ret.front = (front > 0);
    ret.N = front * V;
    
    ret.isCollide = true;
    ret.collide_primitive = this;
    
    return ret;
}

CollidePrimitive Cylinder::Collide( Vector3 ray_O , Vector3 ray_V ) {
    CollidePrimitive rets[3] = {
        SideCollide(ray_O,ray_V),
        BaseCollide(ray_O,ray_V,true),
        BaseCollide(ray_O,ray_V,false)
    };
    
    CollidePrimitive ret;
    
    for(int i = 0; i < 3; i++){
        if(rets[i].isCollide){
            if(rets[i].dist < ret.dist){
                ret = rets[i];
            }
        }
    }
    return ret;
}

Color Cylinder::GetTexture(CollidePrimitive* collision) {
	Vector3 delta = collision->C - O1;
	double u = (delta.Dot(N)) / (O2 - O1).Module();
	double theta = atan2(delta.Dot(Ny), delta.Dot(Nx));
	return material->texture->GetSmoothColor( std::fmod(theta, 2*PI) , u );
}

void Bezier::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O1=" ) O1.Input( fin );
	if ( var == "O2=" ) O2.Input( fin );
	if ( var == "P=" ) {
		degree++;
		double newZ, newR;
		fin>>newZ>>newR;
		R.push_back(newR);
		Z.push_back(newZ);
	}
	if ( var == "Cylinder" ) {
		double maxR = 0;
		double minR = 1e100;
		for(int i=0;i<R.size();i++){
			if (R[i] > maxR) maxR = R[i];
			if (R[i] < minR) minR = R[i];
		}
		boundingCylinderInner = new Cylinder(O1, O2, minR);
		boundingCylinderOuter = new Cylinder(O1, O2, maxR);
		N = (O2 - O1).GetUnitVector();
		Nx = N.GetAnVerticalVector();
		Ny = N * Nx;
	}
	Primitive::Input( var , fin );
}

CollidePrimitive Bezier::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	CollidePrimitive ret;

	CollidePrimitive boundCollisions[3] = {
		boundingCylinderOuter->SideCollide(ray_O,ray_V),
		boundingCylinderOuter->BaseCollide(ray_O,ray_V,true),
		boundingCylinderOuter->BaseCollide(ray_O,ray_V,false)//,
		//boundingCylinderInner->SideCollide(ray_O,ray_V),
		//boundingCylinderInner->BaseCollide(ray_O,ray_V,true),
		//boundingCylinderInner->BaseCollide(ray_O,ray_V,false)
	};

	int collisionCnt = 0;
	for (int i = 0; i < 3; i++) {
		if (boundCollisions[i].isCollide) {
			collisionCnt += 1;
		}
	}
	if (collisionCnt < 1) return ret;

	for (int i = 0; i < 3; i++) {
		if (boundCollisions[i].isCollide) {
			//Base collides
			if ( (i == 1) && ((boundCollisions[i].C - O1).Module() < R[0] + EPS) ) {
				ret.u = 0;
				ret.v = 0;
				ret.dist = boundCollisions[i].dist;
				ret.C = boundCollisions[i].C;
				ret.N = boundCollisions[i].N;
				ret.front = boundCollisions[i].front;

				ret.isAccurate = true;
				ret.isCollide = true;
				ret.collide_primitive = this;

				return ret;
			}

			if ((i == 2) && ((boundCollisions[i].C - O2).Module() < R[R.size()-1] + EPS)) {
				ret.u = 0;
				ret.v = 1;
				ret.dist = boundCollisions[i].dist;
				ret.C = boundCollisions[i].C;
				ret.N = boundCollisions[i].N;
				ret.front = boundCollisions[i].front;

				ret.isAccurate = true;
				ret.isCollide = true;
				ret.collide_primitive = this;

				return ret;
			}

			Vector3 delta0 = boundCollisions[i].C - O1;
			double t0 = boundCollisions[i].dist;
			double u0 = (delta0.Dot(N)) / (O2 - O1).Module();
			double theta0 = atan2(delta0.Dot(Ny), delta0.Dot(Nx));// return value [-PI, PI]

			double t = t0, u = u0, theta = theta0;

			CollidePrimitive collision = Bezier::CollideWithParams(ray_O, ray_V, t, u, theta);
			if (collision.isCollide)
				if (collision.dist < ret.dist)
					ret = collision;

			//Init with random params
			for (int i = 0; i < RANDOM_INITIALIZATION / collisionCnt; i++) {
				t = t0 * ((1 - RAND_AMP) + 2 * RAND_AMP * ran());
				u = ran();
				theta = theta0 * ((1 - RAND_AMP) + 2 * RAND_AMP * ran());// -PI + 2 * PI * i / (double)RANDOM_INITIALIZATION;

				//u = ran();
				//Plane ranPlane(N, Nx, Ny, -N.Dot(O1 + u * (O2 - O1)));
				//CollidePrimitive planeCollision = ranPlane.Collide(ray_O, ray_V);
				//if (!planeCollision.isCollide) continue;
				//t = planeCollision.dist;
				//Vector3 delta = planeCollision.C - O1;
				//theta = -PI + 2 * PI * i / (double)RANDOM_INITIALIZATION;

				CollidePrimitive collision = Bezier::CollideWithParams(ray_O, ray_V, t, u, theta);
				if (collision.isCollide)
					if (collision.dist < ret.dist)
						ret = collision;
			}
		}
	}
    return ret;
}

CollidePrimitive Bezier::CollideWithParams( Vector3 ray_O , Vector3 ray_V , double t, double u, double theta) {
    CollidePrimitive ret;
    Eigen::Vector3d args(t,u,theta);
    
    bool isFZero = false;
    
    for(int i = 0; i < MAX_NEWTON_ITERATION; i++){
        t = args.x();
        u = args.y();
        theta = args.z();
		//std::cout << t << "," << u << "," << theta << std::endl;
        if (u<-.5 || u>1.5)break;
        
        Eigen::Vector3d F = Bezier::F(t,u,theta,ray_O,ray_V).ToEigen();
        Eigen::Matrix3d dF = Bezier::dF(t,u,theta,ray_V);
        if (F.norm() < 1e-3) {
            isFZero = true;
            break;
        }
        args = args - (dF.inverse() * F) * learning_rate;
    }
    
    if (!isFZero) return ret;
    if (t < -EPS) return ret;
    if (u < -EPS || u > 1 + EPS) return ret;
    
	ret.u = std::fmod(theta, 2 * PI);
	ret.v = u;
    ret.dist = t;
    ret.C = C(t, ray_O, ray_V);
    Vector3 dS_u = dP(u, Z) * (O2 - O1) +
                   dP(u, R) * ( cos(theta) * Nx + sin(theta) * Ny);
    Vector3 dS_theta = P(u, R) * ( - sin(theta) * Nx + cos(theta) * Ny);
    ret.N = (dS_theta * dS_u).GetUnitVector();
    ret.front = (ret.N).Dot(ray_V) < -EPS;
    ret.N *= (ret.front ? 1 : -1);

	//std::cout << ret.C.x << "," << ret.C.y << "," << ret.C.z << std::endl;
	//std::cout << ret.N.x << "," << ret.N.y << "," << ret.N.z << std::endl;
    
	ret.isAccurate = false;
    ret.isCollide = true;
    ret.collide_primitive = this;
    
    return ret;
}

Color Bezier::GetTexture(CollidePrimitive* collision) {
	return material->texture->GetSmoothColor(collision->u , collision->v );
}

double Bezier::B(double u, int n, int i){
    return Combination[n][i] * pow(1-u,n-i) * pow(u,i);
}

double Bezier::P(double u, std::vector<double> ctrlPoints){
    double ret = 0.0;
    for(int i=0; i < degree+1; i++){
        ret += B(u,degree,i) * ctrlPoints[i] ;
    }
    return ret;
}

double Bezier::dP(double u, std::vector<double> ctrlPoints){
    double ret = 0.0;
    for(int i=0; i < degree; i++){
        ret += B(u,degree-1,i) * (ctrlPoints[i+1]-ctrlPoints[i]);
    }
    ret = ret * degree;
    return ret;
}

Vector3 Bezier::C(double t, Vector3 ray_O , Vector3 ray_V){
    return ray_O + t * ray_V;
}

Vector3 Bezier::S(double u, double theta){
    return O1 +
           P(u, Z) * (O2 - O1) +
           P(u, R) * ( cos(theta) * Nx + sin(theta) * Ny);
}

Vector3 Bezier::F(double t, double u, double theta, Vector3 ray_O , Vector3 ray_V){
    return S(u,theta) - C(t, ray_O, ray_V);
}

Eigen::Matrix3d Bezier::dF(double t, double u, double theta, Vector3 ray_V){
    Vector3 dF_t = -ray_V;
    Vector3 dF_u = dP(u, Z) * (O2 - O1) +
                   dP(u, R) * ( cos(theta) * Nx + sin(theta) * Ny);
    Vector3 dF_theta = P(u, R) * ( - sin(theta) * Nx + cos(theta) * Ny);
    
    Eigen::Matrix3d ret(3,3);
    ret << dF_t.ToEigen(), dF_u.ToEigen(), dF_theta.ToEigen();// init as column vectors
    return ret;
}