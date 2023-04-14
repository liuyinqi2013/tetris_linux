#ifndef _h_block_h
#define _h_block_h

#include <stdio.h>
#include <string.h>
#include "widget.h"

char b0[] = 
{
	1, 1,
	1, 1
};

char b1[] = 
{
	0, 0, 0,
	0, 1, 0,
	1, 1, 1
};

char b2[] = 
{
	0, 1, 1,
	0, 1, 0,
	0, 1, 0
};

char b3[] = 
{
	0, 1, 1,
	1, 1, 0,
	0, 0, 0
};

char b4[] = 
{
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
};

class GameGird ;

class CBlock : public Widget
{
public:
	CBlock(Widget *parent, int d, char* data, int col = 0, int row = 0): 
	Widget(parent, col * 2, row, d, d)
	{
		m_col = col;
		m_row = row;
		m_d = d;

		m_data[0] = (char*)malloc(d*d);
		memcpy(m_data[0], data, d*d);

		char* src = m_data[0];
		for (int i = 1; i < 4; i++) 
		{
			m_data[i] = (char*)malloc(d*d);
			for (int row = 0; row < m_d; ++row)
			{
				for (int col = 0; col < m_d; ++col)
				{
					m_data[i][col*d + d - 1 - row] = src[row * d+ col];
				}
			}
			src = m_data[i];
		}
	}

	virtual ~CBlock()
	{
		for (int i = 0; i < 4; i++) free(m_data[i]);
	}

	virtual void Draw()
	{
		for (int row = 0; row < m_d; ++row)
		{
			for (int col = 0; col < m_d; ++col)
			{
				if (Value(row, col) == 1)
					printf("\e[%d;%dH■", Y() + row + m_row, X() + 2 * (col + m_col));
			}
		}
		fflush(stdout);
	}

	virtual void Hide()
	{
		for (int row = 0; row < m_d; ++row)
		{
			for (int col = 0; col < m_d; ++col)
			{
				if (Value(row, col) == 1)
					printf("\e[%d;%dH ", Y() + row + m_row, X() + 2 * (col + m_col));
			}
		}
		fflush(stdout);
	}

	void Rotate()
	{
		m_status = (m_status + 1) % 4;
	}

	void Random()
	{
		m_status = rand() % 4;
	}

	void Move(int col, int row)
	{
		m_col += col;
		m_row += row;
	}

	char Value(int row, int col) const
	{
		return m_data[m_status][row * m_d + col];
	}

	void Up() { --m_row; }

	void Down() { ++m_row; }

	void Left() { --m_col; }

	void Right() { ++m_col; }

	void SetOrigin(int col, int row) { m_col = col, m_row= row;}

private:
	friend class GameGird;
	
	int m_col;
	int m_row;
	int m_d;
	int m_status;
	char* m_data[4];
};

class GameGird : public Gird
{
public:
	GameGird(Widget *parent, int x, int y, int row, int col) :
		Gird(parent, x, y, row, col)
	{
		SetCellWidth(2);
		m_data = new char[row * col];
		memset(m_data, 0, row * col);
	}

	virtual ~GameGird()
	{
		delete[] m_data;
	}

	void Reset() 
	{
		memset(m_data, 0, m_row * m_col);
		Clear();
	}

	void SetFullALL() 
	{
		for(int row = 0; row < m_row; row++)
		{
			for(int col = 0; col < m_col; col++)
			{
				SetValue(row, col, 1);
			}
		}
	}

	void SetValue(int row, int col, char v) 
	{
		m_data[row * m_col + col] = v;
		if (v) {
			SetCellVal(row, col, "■");
		} else {
			SetCellVal(row, col, " ");
		}
	}

	bool IsFullRow(int row) 
	{
		return RowCount(row) == m_col;
	}
	
	bool IsEmptyRow(int row) 
	{
		return RowCount(row) == 0;
	}

	int RowCount(int row) 
	{
		int count = 0;
		for (int col = 0; col < m_col; ++col)
		{
			if (Value(row, col)) ++count;
		}
		return count;
	}

	void RowTwinkling(int row) 
	{
		RowTwinklings(row, row + 1);
	}

	void RowTwinklings(int begin, int end) 
	{
		for (int i = 0; i < 4; i++)
		{
			HideRows(begin, end);
			usleep(400000);
			DrawRows(begin, end);
			usleep(400000);
		}
	}

	void EraseRows(int begin, int end) 
	{
		int n = end - begin;
		if (n == 0) {
			return;
		}

		for (int row = begin - 1; row >= 0; row--)
		{
			for (int col = 0; col < m_col; col++)
			{
				SetValue(row + n, col, Value(row, col));
			}
		}

		for (int row = 0; row < n; row++)
		{
			for (int col = 0; col < m_col; col++)
			{
				SetValue(row, col, 0);
			}
		}
	}

	char Value(int row, int col) 
	{
		return m_data[row * m_col + col];
	}

	void Copy(const CBlock* b)
	{
		for (int row = 0; row < b->m_d; row++)
		{
			for (int col = 0; col < b->m_d; col++)
			{
				char v = b->Value(row, col);
				if (v) SetValue(row + b->m_row, col + b->m_col, v);
			}
		}
	}

	bool LeftHit(const CBlock* b) 
	{
		for (int col = 0; col < b->m_d; col++)
		{
			for (int row = 0; row < b->m_d; row++)
			{
				if (!b->Value(row, col)) continue;
				else if ((col + b->m_col) == 0 || 
					Value(row + b->m_row, col + b->m_col) ||
					Value(row + b->m_row, col + b->m_col - 1)) {
						return true;
				}
			}
		}

		return false;
	}

	bool RightHit(const CBlock* b) 
	{
		for (int col = b->m_d - 1; col >= 0; col--)
		{
			for (int row = 0; row < b->m_d; row++)
			{
				if (!b->Value(row, col)) continue;
				else if ((col + b->m_col) == m_col - 1 || 
					Value(row + b->m_row, col + b->m_col) ||
					Value(row + b->m_row, col + b->m_col + 1)) {
						return true;
				}
			}
		}

		return false;
	}

	bool DownHit(const CBlock* b) 
	{
		for (int row = b->m_d - 1; row >= 0; row--)
		{
			for (int col = 0; col < b->m_d; col++)
			{
				if (!b->Value(row, col)) continue;
				else if ((row + b->m_row) == m_row - 1|| 
					Value(row + b->m_row, col + b->m_col) ||
					Value(row + b->m_row + 1, col + b->m_col)) {
						return true;
				}
			}
		}
		return false;
	}

	int doCheckAndEraseRows() 
	{
		int row;
		int end = 0;
		int fullCnt = 0;
		for (row = m_row - 1; row >= 0; row--)
		{
			int cnt = RowCount(row);

			if (cnt == m_col) {
				if (end == 0) end = row;
				fullCnt++;
			} else if (!cnt || fullCnt) {
				break;
			}
		}

		if (fullCnt > 0)
		{
			RowTwinklings(end + 1 - fullCnt, end + 1);
			EraseRows(end + 1 - fullCnt, end + 1);
			Draw();
		}

		return fullCnt;
	}

	void PlayAnimation() 
	{
		SetFullALL();
		Draw();
		usleep(1000*1000);
		Reset();
		Draw();
		usleep(1000*1000);
	}

private:

	char* m_data;
};
#endif