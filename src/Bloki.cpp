#include <core/Application.hpp>

/*
	 ________  ___  ___        ___   _______          _______
	/  _____/ /  / /  /       /  /  /  ___  \        /  ___  \
   /  /___   /  / /  /       /  /  /  /__/  /       /  /__/  /
  /  ____/  /  / /  /       /  /  /  ______/       /  ______/
 /  /      /  / /  /_____  /  /  /  /             /  /    __
/__/      /__/ /________/ /__/  /__/             /__/    /_/
*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE   hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application app;
	app.Start();

	return 0;
}