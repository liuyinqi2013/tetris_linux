#include "tetris.h"

GlobalOpt opt;

int main()
{
	Tetris game;
	game.Init();
	game.Run();

	return 0;
}
