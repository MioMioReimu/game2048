#include "board.h"
board Slover2048::move(DIRECTION direction)
{

}
#define COL_MASK 0x000f000f000f000fULL
static inline uint64_t unpack_col(uint16_t row)
{
	uint64_t tmp = row;
	return (tmp | (tmp << 12ULL) | (tmp << 24ULL) | (tmp << 36ULL)) & COL_MASK;
}
static inline uint16_t reverse_row(uint16_t row)
{
	return (row >> 12) | ((row >> 4) & 0x00f0) | ((row << 4) & 0x0f00) | (row << 12);
}
// Transpose rows/columns in a board:
//   0123       048c
//   4567  -->  159d
//   89ab       26ae
//   cdef       37bf
static inline uint64_t transpose(uint64_t x)
{
	uint64_t a1 = x & 0xF0F00F0FF0F00F0FULL;
	uint64_t a2 = x & 0x0000F0F00000F0F0ULL;
	uint64_t a3 = x & 0x0F0F00000F0F0000ULL;
	uint64_t a = a1 | (a2 << 12) | (a3 >> 12);
	uint64_t b1 = a & 0xFF00FF0000FF00FFULL;
	uint64_t b2 = a & 0x00FF00FF00000000ULL;
	uint64_t b3 = a & 0x00000000FF00FF00ULL;
	return b1 | (b2 >> 24) | (b3 << 24);
}
uint32_t Slover2048::score_tbl[65536];
uint16_t Slover2048::row_left_tbl[65536];
uint16_t Slover2048::row_right_tbl[65536];
uint64_t Slover2048::col_up_tbl[65536];
uint64_t Slover2048::col_down_tbl[65536];
int Slover2048::inited = Slover2048::init_tbls();
int Slover2048::init_tbls()
{
	for (uint32_t row = 0; row < 65536; ++row)
	{
		uint16_t row_line[4] = {
			(row >> 0) & 0xf,
			(row >> 4) & 0xf,
			(row >> 8) & 0xf,
			(row >> 12) & 0xf,
		};
		//row score
		int score = 0;
		for (int c = 0; c < 4; ++c)
		{
			if (row_line[c] > 0)
			{
				//score是所有数字之和以及中间合并结果
				//2^k必然是由2个2^(k-1)组合而成，所以S(k) = 2^k + 2*S(k-1); S(1) = 0
				//故 S(k)/ 2^k = S(k-1)/2^(k-1) + 1 即可求出 Sk = (k-1)*2^k
				score += (row_line[c] - 1)
					 * (1 << row_line[c]);
			}
		}
		score_tbl[row] = score;
		//从第二个数开始查找
		for (int i = 1; i < 4; ++i)
		{
			//如果零 则看下一个啦
			if (row_line[i] == 0)
			{
				continue;
			}
			else
			{
				//非零的话，开始计算沉降位置
				int j = i - 1;
				for (; j >= 0; --j)
				{//找到沉降的位置，即第一个非0 或者最底下 的位置
					if (row_line[j] != 0)
						break;
				}
				//如果是沉降到最底下的位置，而且最底下的位置是0
				if (row_line[j] == 0)
				{
					row_line[j] = row_line[i];
					row_line[i] = 0;
				}//如果沉降并相加
				else if (row_line[j] == row_line[i])
				{
					++row_line[j];
					row_line[i] = 0;
				}
				else//如果沉降但是不能相加
				{
					row_line[j + 1] = row_line[i];
					if (j + 1 != i)
						row_line[i] = 0;
				}
			}
		}
		uint16_t result_row =
			(row_line[0] << 0) |
			(row_line[1] << 4) |
			(row_line[2] << 8) |
			(row_line[3] << 12);
		uint16_t rev_result_row = reverse_row(result_row);
		uint16_t rev_row = reverse_row(row);

		row_left_tbl	[row]		= row					^	result_row;
		row_right_tbl	[rev_row]	= rev_row				^	rev_result_row;
		col_up_tbl		[row]		= unpack_col(row)		^	unpack_col(result_row);
		col_down_tbl	[rev_row]	= unpack_col(rev_row)	^	unpack_col(rev_result_row);
	}
}