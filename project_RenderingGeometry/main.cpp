#include "RenderingGeometry.h"
#include "..\bootstrap\Application.h"

int main() {
	
	auto app = new RenderingGeometry();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}