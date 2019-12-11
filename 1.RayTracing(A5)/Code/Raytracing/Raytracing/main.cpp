#include"raytracer.h"
int main() {
	Raytracer* raytracer = new Raytracer;
	raytracer->SetInput( "final.txt" );
	raytracer->SetOutput( "picture.bmp" );
	//raytracer->Run();
	raytracer->MultiThreadRun();
	//raytracer->MultiThreadRunDebugRun(0,640,0,380,false);
	//raytracer->MultiThreadRunDebugRun(554, 555, 301, 302, false);
	return 0;
}
