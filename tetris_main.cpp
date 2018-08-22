#include <stdio.h>
#include <string.h>
#include <map>
#include <unistd.h>
#include <pthread.h>

#include <cassert>
#include <termios.h>

#include <algorithm>

#include <math.h>
#include <time.h>
#include <vector>
#include <fcntl.h>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <sys/time.h>

#include <sys/select.h>
#include <sys/types.h>

using namespace std;

#define MAX_SIZE 4

unsigned long long UTime()
{
	struct timeval val = {0};
	gettimeofday(&val, NULL);
	return (val.tv_sec * 1000000 + val.tv_usec);
}

void DrawRectangle(int x, int y, int w, int h)
{
	printf("\e[%d;%dH+", y, x);
	for(int i = 1; i < w; i++)
	{
		printf("\e[%d;%dH-", y, x + i);
		printf("\e[%d;%dH-", y + h, x + i);
	}

	for(int i = 1; i < h; i++)
	{
		printf("\e[%d;%dH|", y + i, x);
		printf("\e[%d;%dH|", y + i, x + w);
	}


	printf("\e[%d;%dH+", y, x);
	printf("\e[%d;%dH+", y, x + w);
	printf("\e[%d;%dH+", y + h, x);
	printf("\e[%d;%dH+", y + h, x + w);	


}

inline void Clear()
{
	printf("\e[0;0H\e[2J");
}

void Move(int x, int y)
{
	printf("\e[%d;%dH", y, x);
}


class Frame
{
	public:

		Frame(int type = 0) :
			m_x(10),
			m_y(10),
			m_width(20),
			m_height(20),
			m_type(type)
	{
	}

		Frame(int x, int y, int w, int h, int type = 0) :
			m_x(x),
			m_y(y),
			m_width(w),
			m_height(h),
			m_type(type)
	{
	}

		~Frame() {}

		virtual void Draw()
		{
			DrawRectangle(m_x - 2, m_y - 1, m_width * 2 + 2, m_height + 1);	
		}

		void Move(int dx, int dy)
		{
			m_x += dx;
			m_y += dy;
		}


		void Up() { --m_y; }

		void Down() { ++m_y; }

		void Left() { --m_x; }

		void Right(){ ++m_x; }

	public:

		int m_x;
		int m_y;

		int m_width;
		int m_height;
		int m_type;
};


class GameFrame : public Frame
{
public:
	GameFrame(int x, int y, int w, int h) :
	 Frame(x, y, w, h, 1),
	 m_col(w),
	 m_row(h)
	{

		m_data = new char[m_row * m_col];
		memset((char*)m_data, 0, m_row * m_col);

		/*
		for (int i = 0; i < m_row; ++i)
       		{
                	for (int j = 0; j < m_col; ++j)
                	{
                        	m_data[i * m_row + j] = 0;

               		}
       		}
		*/
	}
	
	~GameFrame()
	{
		delete[] m_data;
	}
	
	virtual void Draw();

public:
	int m_col;
	int m_row;
	char* m_data;
};

void GameFrame::Draw()
{
	Frame::Draw();
	for (int i = 0; i < m_row; ++i)
	{
		for (int j = 0; j < m_col; ++j)
		{
                        if (m_data[i * m_col + j] == 1)
                                printf("\e[%d;%dH■", (m_y + i), (m_x +  j * 2));
			
		}
	}
}


class Block
{
	public:
		Block(int x, int y, int type, Frame * pFrame = NULL) :
			m_x(x),
			m_y(y),
			m_status(0),
			m_type(type),
			m_gameFrame(pFrame)
		{
			/*
			for (int i = 0; i < MAX_SIZE; ++i)
			{
				memset(m_data + i, 0, MAX_SIZE) ;
			}
			*/
			
			//memset((char*)m_data, 0, 4 * 4 * 4) ;
			/*
			m_data[0][1][1] = 1;
			m_data[0][2][0] = 1;
			m_data[0][2][1] = 1;
			m_data[0][2][2] = 1;
			*/

			Init();
		}
		
		Block(const Block & rhs) :
		        m_x(rhs.m_x),
                        m_y(rhs.m_y),
                        m_status(rhs.m_status),
                        m_type(rhs.m_type),
                        m_gameFrame(rhs.m_gameFrame)

		{
			Init();
		}

		~Block()
		{
		}

		virtual void Draw();

		void Move(int dx, int dy)
		{
			m_x += dx;
			m_y += dy;
		}

		void SetFrame(Frame * pFrame)
		{
			m_gameFrame = pFrame;
		}

		void Up() { --m_y; }

		void Down() {  ++m_y; }

		void Left() {  --m_x; }

		void Right(){  ++m_x; }

		void SetX(int x) { m_x = x; }

		void SetY(int y) { m_y = y; }

		void Rotate();

		void RRotate();

		void Init();

		void Random() { m_status = rand() % 4;}

		bool LeftHit();
		
		bool RightHit();

		bool DownHit();

		Block& operator= (const Block & rhs);

		void Paste();

	protected:
		int m_x;
		int m_y;
		int m_d;
		int m_status;
		int m_type;
		char m_data[4][4][4];

		Frame * m_gameFrame;
};

void Block::Init()
{
	memset((char*)m_data, 0, 4 * 4 * 4);

	switch(m_type)
	{
	case 0:
		m_d = 3;
		m_data[0][1][1] = 1;
                m_data[0][2][0] = 1;
                m_data[0][2][1] = 1;
                m_data[0][2][2] = 1;
		break;
	case 1:
		m_d = 2;
		m_data[0][0][0] = 1;
                m_data[0][0][1] = 1;
                m_data[0][1][0] = 1;
                m_data[0][1][1] = 1;
		break;
	case 2:
		m_d = 4;
		m_data[0][0][0] = 1;
                m_data[0][1][0] = 1;
                m_data[0][2][0] = 1;
                m_data[0][3][0] = 1;
		break;
	case 3:
		m_d = 3;
		m_data[0][0][0] = 1;
                m_data[0][0][1] = 1;
                m_data[0][0][2] = 1;
                m_data[0][1][0] = 1;
		break;
	case 4:
		m_d = 4;
		m_data[0][1][0] = 1;
                m_data[0][1][1] = 1;
                m_data[0][2][2] = 1;
                m_data[0][2][3] = 1;
		break;
	default:
                m_d = 2;
                m_data[0][0][0] = 1;
                m_data[0][0][1] = 1;
                m_data[0][1][0] = 1;
                m_data[0][1][1] = 1;
	}

	for (int n = 1; n < 4; ++n)
	{
		for (int i = 0; i < m_d; ++i)
		{
			for (int j = 0; j < m_d;  ++j)
			{
				m_data[n][i][j] = m_data[n - 1][j][m_d - 1 - i];
			}
		}
	}
}


void Block::Draw()
{
	if (!m_gameFrame) return;

	char (*data)[4] = m_data[m_status];

	for (int i = 0; i < m_d; ++i)
	{
		for (int j = 0; j < m_d; ++j)
		{
			if (data[i][j] == 1)
				printf("\e[%d;%dH■", (m_gameFrame->m_y + m_y + i), (m_gameFrame->m_x + (m_x + j) * 2));
		}
	}
}

void Block::Rotate()
{
	m_status = (m_status + 1) % 4;
}

void Block::RRotate()
{
	m_status = (m_status - 1) % 4;	
	if (m_status < 0) m_status = 3;
}

bool Block::LeftHit()
{
	char (*data)[4] = m_data[m_status];

	for (int i = 0; i < m_d; ++i)
	{
		for (int j = 0; j < m_d; ++j)
		{
			if (data[i][j] == 1)
			{
				if (m_x + j <= 0) 
				{
					return true;
				}
				else if (m_gameFrame && 
					m_y + i >= 0 &&
					m_y + i < m_gameFrame->m_height &&
					m_gameFrame->m_type == 1)
				{
					GameFrame * pGameFrame = (GameFrame*)m_gameFrame;
					int index = (m_y + i) * pGameFrame->m_col + (m_x + j);
					if (pGameFrame->m_data[index] == 1 || pGameFrame->m_data[index - 1] == 1)	
						return true;
				}

				break;
			}
		}
	}

	return false;
		
}
		
bool Block::RightHit()
{
	char (*data)[4] = m_data[m_status];

	for (int i = 0; i < m_d; ++i)
	{
		for (int j = m_d - 1; j >= 0; --j)
		{
			if (data[i][j] == 1)
			{
				if (m_x + j >= m_gameFrame->m_width - 1) 
				{
					return true;
				}
				else if (m_gameFrame && 
					m_y + i >= 0 && 
					m_y + i < m_gameFrame->m_height && 
					m_gameFrame->m_type == 1)
				{
					GameFrame * pGameFrame = (GameFrame*)m_gameFrame;
					int index = (m_y + i) * pGameFrame->m_col + (m_x + j);
					if (pGameFrame->m_data[index] == 1 || pGameFrame->m_data[index + 1] == 1)	
						return true;
				}

				break;
			}
		}
	}

	return false;
}

bool Block::DownHit()
{
	char (*data)[4] = m_data[m_status];

	for (int i = 0; i < m_d; ++i)
	{
		for (int j = m_d - 1; j >= 0; --j)
		{
			if (data[j][i] == 1)
			{
				if (m_y + j >= m_gameFrame->m_height - 1) 
				{
					return true;
				}
				else if (m_gameFrame && 
					m_x + i >= 0 && 
					m_x + i < m_gameFrame->m_width && 
					m_gameFrame->m_type == 1)
				{
					GameFrame * pGameFrame = (GameFrame*)m_gameFrame;
					int index = (m_y + j) * pGameFrame->m_col +  (m_x + i);
					int index1 = (m_y + j + 1) * pGameFrame->m_col +  (m_x + i);
					if (pGameFrame->m_data[index] == 1 || pGameFrame->m_data[index1] == 1)	
						return true;
				}

				break;
			}
		}
	}
	
	return false;

}

Block& Block::operator= (const Block & rhs)
{
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	m_status = rhs.m_status;
	m_type = rhs.m_type;
	m_gameFrame = rhs.m_gameFrame;

	memcpy(m_data, rhs.m_data, 4 * 4 * 4);

	return *this;
}

void Block::Paste()
{
        char (*data)[4] = m_data[m_status];

        for (int i = 0; i < m_d; ++i)
        {
                for (int j = 0; j < m_d; ++j)
                {
                        if (data[i][j] == 1)
                        {
                                if (m_gameFrame &&
                                        m_y + i >= 0 &&
                                        m_y + i < m_gameFrame->m_height &&
                                        m_gameFrame->m_type == 1)
                                {
                                        GameFrame * pGameFrame = (GameFrame*)m_gameFrame;
					int index = (m_y + i) * pGameFrame->m_col + (m_x + j);
                                        pGameFrame->m_data[index] = 1;
                                }
                        }
                }
        }

}

class TetrisGame
{
public:
	TetrisGame();
	~TetrisGame() {}
	
	void Init();
	
	void Run();
	
private:
	
	void Draw();

	bool GameOver();

	void ShowGameOverInfo();

private:
	int m_score;
	boost::shared_ptr<Frame> m_infoFrame;
	boost::shared_ptr<GameFrame> m_gameFrame;
	vector<boost::shared_ptr<Block> > m_vecBlock;
	boost::shared_ptr<Block> m_currBlock;
	boost::shared_ptr<Block> m_nextBlock;
};

TetrisGame::TetrisGame()
{
	m_infoFrame = boost::make_shared<Frame>(44, 10, 8, 20);
        m_gameFrame = boost::make_shared<GameFrame>(10, 10, 15, 20);

        for (int i = 0; i < 5; ++i)
        {
                m_vecBlock.push_back(boost::make_shared<Block>(7, 0, i, m_gameFrame.get()));
        }
}


void TetrisGame::Init()
{
	m_score = 0;
	m_currBlock = m_vecBlock[rand() % m_vecBlock.size()];
	m_nextBlock = m_vecBlock[rand() % m_vecBlock.size()];

	m_currBlock->Random();
	m_nextBlock->Random();
}

void TetrisGame::Run()
{
	int buf[2] = {0};

	unsigned long long lastOptTime = UTime();

        /*
	int flags;
	if ((flags = fcntl(0, F_GETFL)) == -1) return ;
 		
	flags |= O_NONBLOCK;		
	if (fcntl(0, F_SETFL, flags) == -1) return ; 
	*/

	fd_set rdset, tmpset;
	
	FD_ZERO(&rdset);
	FD_SET(0, &rdset);


	struct timeval timeout = {0, 500};

	Draw();

	while(1)
	{
		if (GameOver())
		{
			ShowGameOverInfo();

			/*
			flags &= ~O_NONBLOCK;
        		if (fcntl(0, F_SETFL, flags) == -1) return ;
			*/

			read(0, buf, 1);
			if (buf[0] == 'q')
				break;
			else if (buf[0] == 'c')
				Init();
			
	
			/*
			flags |= O_NONBLOCK;
        		if (fcntl(0, F_SETFL, flags) == -1) return ;
			*/
				
		}
		
		if (UTime() - lastOptTime > 1000 * 500 * 2)
		{
			if (m_currBlock->DownHit())
			{
				m_currBlock->Paste();
				m_currBlock = m_nextBlock;
				m_nextBlock = m_vecBlock[rand() % m_vecBlock.size()];
				m_nextBlock->Random();
			}
			else
			{
				m_currBlock->Down();	
			}

			lastOptTime = UTime();
			Draw();
		}
		else
		{	
			tmpset = rdset;

			timeout.tv_sec = 0;
           		timeout.tv_usec = 400;

			int n = select(1, &tmpset, NULL, NULL, &timeout);

			if (n > 0)
			{

				int len = read(0, buf, 1);

				if (len == 1)
				{ 

					switch(buf[0])
					{
						case 'w':
							m_currBlock->Rotate();
							break;
						case 's':
							m_currBlock->Down();
							break;
						case 'a':
							m_currBlock->Left();
							break;
						case 'd':
							m_currBlock->Right();
							break;
						default:
							printf("\a");
					}

					Draw();
				}
			}
		}

		usleep(10);

	}
}

bool TetrisGame::GameOver()
{
	for (int i = 0; i < m_gameFrame->m_col; ++i)
	{
		if (m_gameFrame->m_data[i] == 1)
			return true;
	}

	return false;
}

void TetrisGame::ShowGameOverInfo()
{
}

void TetrisGame::Draw()
{
	Clear();

	m_gameFrame->Draw();
	m_infoFrame->Draw();
	m_currBlock->Draw();

	Block next(*m_nextBlock);

	next.SetFrame(m_infoFrame.get());

	next.SetX(0);
	next.SetY(3);
	next.Draw();
	
	fflush(stdout);
}

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

GlobalOpt globalOpt;



int main()
{
	/*
	srand(time(NULL));

	struct termios told;
	struct termios tnew;

	tcgetattr(0, &told);
	tnew = told;

	tnew.c_lflag &= ~(ICANON | ECHO | ECHOE | IEXTEN);
	tnew.c_oflag &= ~ONLCR;
	tnew.c_cc[VMIN] = 1;
	tnew.c_cc[VTIME] = 0;


	tcsetattr(0, TCSANOW, &tnew);

	printf("\e[?25l\n");
	*/

	TetrisGame game;

	game.Init();

	game.Run();

	/*

	printf("\e[0;0H\e[2J");

	//test_rotate();
	//printf("\e[%d;%dH", 20, 10);

	//DrawRectangle(10, 10, 40, 20);

	//Move(30, 20);


	GameFrame f(10, 10, 15, 20);
	Block b(2, 3, 0, &f);

	b.Random();

	f.Draw();
	b.Draw();

	fflush(stdout);

	int buf[2] = {0};
	while (buf[0] != 'q')
	{
		read(0, buf, 1);
		switch(buf[0])
		{
			case 'w':
				b.Rotate();
				break;
			case 's':
				b.Down();
				break;
			case 'a':
				b.Left();
				break;
			case 'd':
				b.Right();
				break;
			default:
				;
				//printf("%c", buf[0]);
		}
		Clear();

		f.Draw();
		b.Draw();

		fflush(stdout);
	}


	for(int i = 0; i < 10 ; i++)
	{
		printf("\e[35m%-3d%c\e[0m\e[4D\e[?25l", (i + 1) * 100 /10, '%');
		fflush(stdout);
		sleep(1);
	}

	*/

	//printf("\e[?25h\n");

	//Move(0, 31);

	//tcsetattr(0, TCSANOW, &told);

	return 0;
}
