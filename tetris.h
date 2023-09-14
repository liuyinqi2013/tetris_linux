#ifndef _h_tetris_h
#define _h_tetris_h

#include <stdio.h>
#include <string.h>
#include <map>
#include <unistd.h>

#include <cassert>
#include <termios.h>

#include <algorithm>

#include <math.h>
#include <time.h>
#include <vector>
#include <fcntl.h>
#include <memory>

#include <sys/time.h>

#include <sys/select.h>
#include <sys/types.h>

#include "config.h"
#include "block.h"

unsigned long int UTime()
{
	struct timeval val = {0};
	gettimeofday(&val, NULL);
	return (val.tv_sec * 1000000 + val.tv_usec);
}

class Tetris
{
public:
	Tetris();
	~Tetris() {}

	void Init();
	void Reset();

	void Run();

private:

	void Draw();
	void DrawGameInfo();
	void Next();
	int ToLevel(int score);
	unsigned int WaitDuration();

	bool GameOver();

	void ShowGameOverInfo();

	void BlockLeft();
	void BlockRight();
	void BlockDown();
	void BlockRotate();
	void BlockFastDown();

private:
	int m_score;
	int m_level;
	int m_duration;
	float m_rate[4];
	
	Canvas* m_canvas;
	Frame* m_infoFrame;
	Frame* m_gameFrame;
	GameGird* m_gameGird;
	GameGird* m_infoGird;
	CBlock* m_currBlock;
	CBlock* m_nextBlock;
	vector<CBlock*> m_vecBlock;
};

Tetris::Tetris()
{

}

void Tetris::Init()
{
	m_duration = 900;

	m_rate[0] = 1;
	m_rate[1] = 1.5;
	m_rate[2] = 2;
	m_rate[3] = 2;

	m_canvas = new Canvas(100, 200);
	m_gameGird = new GameGird(m_canvas, 10, 5, 20, 15);
	m_gameFrame = new Frame(m_gameGird);
	m_infoGird = new GameGird(m_canvas, 10 + m_gameFrame->W() + 1, 5, 20, 7);
	m_infoFrame = new Frame(m_infoGird);

	m_vecBlock.push_back(new CBlock(NULL, 2, b0));
	m_vecBlock.push_back(new CBlock(NULL, 3, b1));
	m_vecBlock.push_back(new CBlock(NULL, 3, b2));
	m_vecBlock.push_back(new CBlock(NULL, 3, b3));
	m_vecBlock.push_back(new CBlock(NULL, 3, b4));
	m_vecBlock.push_back(new CBlock(NULL, 4, b5));

	Reset();
}

void Tetris::Reset() 
{
	m_gameGird->Reset();

	m_score = 0;
	m_level = 1;
	
	int index = rand() % m_vecBlock.size();
	m_currBlock = m_vecBlock[index];
	m_nextBlock = m_vecBlock[(index + 1) % m_vecBlock.size()];

	m_currBlock->SetParent(m_gameGird);
	m_currBlock->SetOrigin(m_gameGird->Col() / 2, 0);

	m_nextBlock->SetParent(m_infoGird);
	m_nextBlock->SetOrigin(1, 6);

	m_currBlock->Random();
	m_nextBlock->Random();

	Draw();
}

void Tetris::Run()
{
	int buf[128] = { 0 };

	fd_set rdset, tmpset;
	FD_ZERO(&rdset);
	FD_SET(0, &rdset);
	struct timeval timeout = { 0, 500 };

	Draw();

	unsigned long int last = UTime();
	unsigned long int now = UTime();

	while (1)
	{
		if (GameOver())
		{
			ShowGameOverInfo();
			int n = read(0, buf, 128);
			if (buf[n - 1] == 'q')
				break;
			else if (buf[n - 1] == 'c')
				Reset();
		}

		tmpset = rdset;
		timeout.tv_sec = 0;
		timeout.tv_usec = WaitDuration() * 1000;

		now = UTime();

		int n = select(1, &tmpset, NULL, NULL, &timeout);
		if (n > 0)
		{
			int len = read(0, buf, 128);
			if (len > 0 && now - last <=  WaitDuration() * 2000)
			{
				switch (buf[len-1])
				{
				case 'w':
					BlockRotate();
					break;
				case 's':
					m_gameGird->DownHit(m_currBlock) ? Next() : BlockDown();
					break;
				case 'a':
					m_gameGird->LeftHit(m_currBlock) ? Sound_Bi() : BlockLeft();
					break;
				case 'd':
					m_gameGird->RightHit(m_currBlock) ? Sound_Bi() : BlockRight();
					break;
				case ' ':
					BlockFastDown();
					break;
				default:
					Sound_Bi();
				}
				continue;
			}
		}

		if (m_gameGird->DownHit(m_currBlock) ) 
		{
			Next();
		} 
		else
		{
			BlockDown();
		}

		last = now;
	}
}

bool Tetris::GameOver()
{
	return m_gameGird->RowCount(0) > 0;
}

int Tetris::ToLevel(int score)
{
	return 1 + score / 400;
}

unsigned int Tetris::WaitDuration()
{
	if (m_level <= 14) return 800 - 50 * (m_level - 1);
	return 100;
}

void Tetris::ShowGameOverInfo()
{
	m_gameGird->ClearRow(10);
	m_gameGird->ClearRow(11);
	m_gameGird->ClearRow(12);

	m_gameGird->SetCellVal(10, 6, "Game Over.");
	m_gameGird->SetCellVal(11, 6, "Press Q Quit Game.");
	m_gameGird->SetCellVal(12, 6, "Press C Continue.");

	m_gameGird->DrawRow(10);
	m_gameGird->DrawRow(11);
	m_gameGird->DrawRow(12);
}

void Tetris::Next() {
	usleep(50000);
	m_gameGird->Copy(m_currBlock);
	while(1) 
	{
		int cnt = m_gameGird->doCheckAndEraseRows();
		if (cnt == 0) break;
		
		m_score += 100 * cnt;
		m_level = ToLevel(m_score);
		DrawGameInfo();
	}
	
	m_nextBlock->Hide();
	m_currBlock = m_nextBlock;

	m_currBlock->SetParent(m_gameGird);
	m_currBlock->SetOrigin(m_gameGird->Col() / 2, 0);


	int index = rand() % m_vecBlock.size();
	if (m_vecBlock[index] == m_currBlock) {
		m_nextBlock = m_vecBlock[(index + 1) % m_vecBlock.size()];
	} else {
		m_nextBlock = m_vecBlock[index];
	}

	m_nextBlock->Random();

	m_nextBlock->SetParent(m_infoGird);
	m_nextBlock->SetOrigin(1, 6);
	m_nextBlock->Draw();
}

void Tetris::Draw()
{
	m_canvas->Drew();
	m_gameFrame->Draw();
	m_infoFrame->Draw();
	DrawGameInfo();
	m_gameGird->PlayAnimation();
	m_currBlock->Draw();
	m_nextBlock->Draw();
	
	fflush(stdout);
}

void Tetris::DrawGameInfo()
{
	char buf[32] = {0};
	int n = snprintf(buf, 32, "-LEVEL:%5d", m_level);
	m_infoGird->SetCellVal(0, 0, string(buf, n));
	m_infoGird->SetCellVal(1, 0, "--------------");
	n = snprintf(buf, 32, "-SCORE:%5d", m_score);
	m_infoGird->SetCellVal(2, 0, string(buf, n));
	m_infoGird->SetCellVal(3, 0, "--------------");
	m_infoGird->SetCellVal(4, 0, "--NEXT:");
	m_infoGird->Draw();
}

void Tetris::BlockLeft()
{
	m_currBlock->Hide();
	m_currBlock->Left();
	m_currBlock->Draw();
}

void Tetris::BlockRight()
{
	m_currBlock->Hide();
	m_currBlock->Right();
	m_currBlock->Draw();
}

void Tetris::BlockDown()
{
	m_currBlock->Hide();
	m_currBlock->Down();
	m_currBlock->Draw();
}

void Tetris::BlockRotate()
{
	m_currBlock->Hide();
	m_currBlock->Rotate();
	m_currBlock->Draw();
}

void Tetris::BlockFastDown()
{
	int d = m_gameGird->DownHitDuration(m_currBlock);
	if (!d) 
		return;

	m_currBlock->Hide();
	m_currBlock->Down(d);
	m_currBlock->Draw();
}

#endif