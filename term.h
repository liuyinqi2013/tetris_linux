#ifndef _h_term_h
#define _h_term_h

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

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

enum Color
{
    BLACK   = 30,
    RED     = 31,
    GREEN   = 32,
    YELLOW  = 33,
    BLUE    = 34,
    PURPLE  = 35,
    SKYBLUE = 36,
    WHITE   = 37,
};

inline std::string ColorString(const std::string& strIn, int color)
{
	std::stringstream ss;
	ss << "\e[" << color << "m" << strIn << "\e[0m";
	return ss.str();

}

inline std::string Red(const std::string& strIn)
{
    return ColorString(strIn, Color::RED);
}

inline std::string Black(const std::string& strIn)
{
    return ColorString(strIn, Color::BLACK);
}

inline std::string Green(const std::string& strIn)
{
    return ColorString(strIn, Color::GREEN);
}

inline std::string Yellow(const std::string& strIn)
{
    return ColorString(strIn, Color::YELLOW);
}

inline std::string Blue(const std::string& strIn)
{
    return ColorString(strIn, Color::BLUE);
}

inline std::string SkyeBlue(const std::string& strIn)
{
    return ColorString(strIn, Color::SKYBLUE);
}

inline std::string White(const std::string& strIn)
{
    return ColorString(strIn, Color::WHITE);
}

#endif