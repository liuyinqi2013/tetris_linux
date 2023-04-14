#ifndef _h_term_h
#define _h_term_h

#include <stdio.h>

class GlobalOpt
{
public:
	GlobalOpt()
	{
		srand(time(NULL));

		tcgetattr(0, &told);
		tnew = told;
		tnew.c_lflag &= ~(ICANON | ECHO | ECHOE | IEXTEN);
		tnew.c_oflag &= ~ONLCR;
		tnew.c_cc[VMIN] = 1;
		tnew.c_cc[VTIME] = 0;

		tcsetattr(0, TCSANOW, &tnew);

		printf("\e[?25l");
	}

	~GlobalOpt()
	{
		printf("\e[?25h");
		tcsetattr(0, TCSANOW, &told);
	}
public:
	struct termios told;
	struct termios tnew;
};


inline void Clear()
{
	printf("\e[0;0H\e[2J");
}

inline void Move(int x, int y)
{
	printf("\e[%d;%dH", y, x);
}

inline void Sound_Bi()
{
	printf("\a");
}

inline void DrawRectangle(int x, int y, int w, int h)
{
	printf("\e[%d;%dH+", y, x);
	for (int i = 0; i < w; i++)
	{
		printf("\e[%d;%dH-", y, x + i);
		printf("\e[%d;%dH-", y + h, x + i);
	}

	for (int i = 0; i < h; i++)
	{
		printf("\e[%d;%dH|", y + i, x);
		printf("\e[%d;%dH|", y + i, x + w);
	}

	printf("\e[%d;%dH+", y, x);
	printf("\e[%d;%dH+", y, x + w);
	printf("\e[%d;%dH+", y + h, x);
	printf("\e[%d;%dH+", y + h, x + w);
}

#endif