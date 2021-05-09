#include "Application3D.h"

int main()
{
	// allocation
	auto app = new Application3D();

	// initialise and loop
	app->run("OPENGL DEMO", 1920, 1080, false);

	// deallocation
	delete app;

	return 0;
}