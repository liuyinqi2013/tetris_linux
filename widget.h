#ifndef _h_graph_h
#define _h_graph_h

#include <string.h>
#include <string>
#include <vector>
#include "term.h"

using namespace std;

class Widget 
{
public:
	Widget(Widget *parent, int x, int y, int w, int h) :
	_parent(parent),
	_x(x),
	_y(y),
	_width(w),
	_hight(h)
	{
	}

	virtual ~Widget() {}
	virtual void Drew() {};

	virtual int X() 
	{  
		return _parent ? _parent->_x + _x : _x; 
	}

	virtual int Y() 
	{ 
		return _parent ? _parent->_y + _y :  _y;
	}

	virtual int W() { return _width; }
	virtual int H() { return _hight; }

	void SetParent(Widget *parent)
	{
		_parent = parent;
	}

protected:
	Widget* _parent;
	int _x;
	int _y;
	int _width;
	int _hight;
	
};

class Canvas : public Widget 
{
public:
	Canvas(int w, int h) : Widget(NULL, 0, 0, w, h) {}
	virtual ~Canvas() {}

	virtual void Drew() 
	{
		for (int i = 0; i < _hight; i++) 
		{
			for (int i = 0; i < _width; i++) 
			{
				printf(" ");
			}

			printf("\n");
		}
	}
};

class Frame : public Widget 
{
public:
	Frame(Widget *parent, int x, int y, int w, int h) :
	Widget(parent, x, y, w, h)
	{
	}

	Frame(Widget *parent) :
	Widget(parent, -1, -1, parent->W() + 1, parent->H() + 1)
	{
	}

	~Frame() {}

	virtual void Draw()
	{
		DrawRectangle(X(), Y(), W(), H());
	}
};

class Gird : public Widget {
public:
	Gird(Widget *parent, int x, int y, int row, int col) : 
	Widget(parent, x, y, col, row),
	m_row(row),
	m_col(col),
	m_cellWidth(1),
	m_cellHeight(1)
	{
		m_arr.assign(m_row * m_row, string());
	}

	virtual ~Gird() {}

	virtual void Draw()
	{
		for (int i = 0; i < m_row; ++i)
		{
			for (int j = 0; j < m_col; ++j)
			{
				DrawCell(i, j);
			}
		}
	}

	void DrawCell(int row, int col) 
	{
		printf("\e[%d;%dH%s", Y() + row * m_cellHeight, X() + col * m_cellWidth, m_arr[row * m_col + col].c_str());
		fflush(stdout);
	}

	void HideCell(int row, int col) 
	{
		Move(X() + col * m_cellWidth, Y() + row * m_cellHeight);
		for (int i = 0; i < m_cellWidth; i++)  printf(" ");
		fflush(stdout);
	}

	void HideRow(int row) 
	{
		for (int j = 0; j < m_col; ++j) HideCell(row, j);
	}

	void HideRows(int begin, int end) 
	{
		for (int i = begin; i < end; ++i) HideRow(i);
	}

	void DrawRow(int row) 
	{
		for (int j = 0; j < m_col; ++j) DrawCell(row, j);
	}

	void DrawRows(int begin, int end) 
	{
		for (int i = begin; i < end; ++i) DrawRow(i);
	}

	void Clear()
	{
		m_arr.clear();
		m_arr.assign(m_row * m_row, string(" "));
	}

	void ClearRow(int row) 
	{
		for (int j = 0; j < m_col; ++j) SetCellVal(row, j, " ");
	}

	virtual int W() { return m_col * m_cellWidth; }
	virtual int H() { return m_row * m_cellHeight; }


	int Row() { return m_row; }
	int Col() { return m_col; }

	void SetCellVal(int row, int col, const string& data)
	{
		m_arr[row * m_col + col] = data;
	}
	void SetCellWidth(int w) { m_cellWidth = w;}
	void SetCellHeight(int h) { m_cellHeight = h; }

protected:
	int m_row;
	int m_col;
	int m_cellWidth;
	int m_cellHeight;
	vector<string> m_arr;
};


#endif