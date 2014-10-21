#pragma once
#include "cocos2d.h"
struct board
{
	uint64_t board;
	int score;
	int max;
};
enum DIRECTION
{
	LEFT,
	RIGHT,
	UP,
	DOWN
};
class Slover2048
{
public:
	board move(DIRECTION direction);
private:
	board _cur_board;
	std::list<board> _pre_boards;
	static uint32_t score_tbl[65536];
	static uint16_t row_left_tbl[65536];
	static uint16_t row_right_tbl[65536];
	static uint64_t col_up_tbl[65536];
	static uint64_t col_down_tbl[65536];
	
	static int inited;
	static int init_tbls();
};