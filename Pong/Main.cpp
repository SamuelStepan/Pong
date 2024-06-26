#include "PongEngine.h"

int main()
{
	Pong_Engine demo;
	if (demo.Construct(450, 250, 3, 3))
		demo.Start();
	return 0;
}