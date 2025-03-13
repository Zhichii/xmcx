#pragma once

// A lightweight command line interface. 

#include "chh.hpp"

namespace CLI {

	void cls() {
#ifdef _WIN32
		COORD coord;
		coord.X = 0; coord.Y = 0;
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(console, coord);
#endif
#ifdef _linux_
#endif
	}

	struct Widget;
	struct Application;

}