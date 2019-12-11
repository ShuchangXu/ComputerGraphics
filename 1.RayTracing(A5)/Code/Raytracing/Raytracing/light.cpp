#include"light.h"
#include<sstream>
#include<string>
#include<cmath>
#include<cstdlib>
#define ran() ( double( rand() % 32768 ) / 32768 )

Light::Light() {
	sample = rand();
	next = NULL;
	lightPrimitive = NULL;
}

void Light::Input( std::string var , std::stringstream& fin ) {
	if ( var == "color=" ) color.Input( fin );
}

void PointLight::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	Light::Input( var , fin );
}


double PointLight::CalnShade( Vector3 C , Primitive* primitive_head , int shade_quality ) {
	Vector3 V = O - C;
	double dist = V.Module();
	for ( Primitive* now = primitive_head ; now != NULL ; now = now->GetNext() )
	{
		CollidePrimitive tmp = now->Collide(C, V);
		if ( EPS < (dist - tmp.dist) )  return 0;
	}

	return 1;
}

void SquareLight::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Light::Input( var , fin );
}


double SquareLight::CalnShade( Vector3 C , Primitive* primitive_head , int shade_quality ) {
	int shade = 0;
	for (int i = -2; i < 2; i++)
		for (int j = -2; j < 2; j++)
			for (int k = 0; k < shade_quality; k++) {
				Vector3 V = O - C + Dx * ((ran() + i) / 2) + Dy * ((ran() + j) / 2);
				double dist = V.Module();

				for (Primitive* now = primitive_head; now != NULL; now = now->GetNext()) {
					CollidePrimitive tmp = now->Collide(C, V);
					if (tmp.isCollide) {
						if (tmp.dist - dist < -EPS) {
							shade++;
							break;
						}
					}
				}
			}

	return 1 - (double)shade / (16.0 * shade_quality);
	//return 1;
}

Primitive* SquareLight::CreateLightPrimitive()
{
	PlaneAreaLightPrimitive* res = new PlaneAreaLightPrimitive(O, Dx, Dy, color);
	lightPrimitive = res;
	return res;
}



void SphereLight::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "R=" ) fin>>R;
	Light::Input( var , fin );
}


double SphereLight::CalnShade( Vector3 C , Primitive* primitive_head , int shade_quality ) {
	int shade = 0;
	for (int i = 0; i < 16; i++) {
		for (int k = 0; k < shade_quality; k++) {
			double theta = (2 * ran() - 1) * PI;
			double psi = (2 * ran() - 1) * PI / 2;
			Vector3 V = O - C + Vector3(R*cos(psi)*cos(theta), R*cos(psi)*sin(theta), R*sin(psi));
			double dist = V.Module();

			for (Primitive* now = primitive_head; now != NULL; now = now->GetNext()) {
				CollidePrimitive tmp = now->Collide(C, V);
				if (tmp.isCollide) {
					if (tmp.dist - dist < -EPS) {
						shade++;
						break;
					}
				}
			}
		}

	}
			
	return 1 - (double)shade / (16.0 * shade_quality);
}


Primitive* SphereLight::CreateLightPrimitive()
{
	SphereLightPrimitive* res = new SphereLightPrimitive(O, R, color);
	lightPrimitive = res;
	return res;
}

