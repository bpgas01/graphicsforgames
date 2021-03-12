#include "Application3D.h"

int main()
{
	// allocation
	auto app = new Application3D();

	// initialise and loop
	app->run("AIE", 800, 800, false);

	// deallocation
	delete app;

	return 0;
}