#ifndef MILLSTATE_HH
#define MILLSTATE_HH

#include <QVector>

/* Players : 0 and 1
 *
 * 00       01       02
 *    08    09    10
 *       16 17 18
 * 07 15 23    19 11 03
 *       22 21 20
 *    14    13    12
 * 06       05       04
 */

const QVector<int> connectedto[3*8] = {
	{1,7},   {0,2,9},     {1,3},   {2,4,11},     {3,5},   {4,6,13},     {5,7},   {0,6,15}, // 0-7
	{9,15},  {1,8,17,10}, {9,11},  {3,10,19,12}, {11,13}, {5,12,14,21}, {13,15}, {7,14,23,8}, //8-15
	{17,23}, {16,18,9},   {17,19}, {18,11,20},   {19,21}, {20,22,13},   {21,23}, {15,16,22} // 16-23
};

const int mills[16][3] = {
	{0,1,2}, {8,9,10}, {16,17,18}, {7,15,23}, {19,11,3}, {22,21,20}, {14,13,12}, {6,5,4},
	{0,7,6}, {8,15,14}, {16,23,22}, {1,9,17}, {21,13,5}, {18,19,20}, {10,11,12}, {2,3,4}
};

const int millsfrom[24][2][3] = {
	{{0, 1, 2}, {0, 7, 6}},
	{{0, 1, 2}, {1, 9, 17}},
	{{0, 1, 2}, {2, 3, 4}},
	{{19, 11, 3}, {2, 3, 4}},
	{{6, 5, 4}, {2, 3, 4}},
	{{6, 5, 4}, {21, 13, 5}},
	{{6, 5, 4}, {0, 7, 6}},
	{{7, 15, 23}, {0, 7, 6}},
	{{8, 9, 10}, {8, 15, 14}},
	{{8, 9, 10}, {1, 9, 17}},
	{{8, 9, 10}, {10, 11, 12}},
	{{19, 11, 3}, {10, 11, 12}},
	{{14, 13, 12}, {10, 11, 12}},
	{{14, 13, 12}, {21, 13, 5}},
	{{14, 13, 12}, {8, 15, 14}},
	{{7, 15, 23}, {8, 15, 14}},
	{{16, 17, 18}, {16, 23, 22}},
	{{16, 17, 18}, {1, 9, 17}},
	{{16, 17, 18}, {18, 19, 20}},
	{{19, 11, 3}, {18, 19, 20}},
	{{22, 21, 20}, {18, 19, 20}},
	{{22, 21, 20}, {21, 13, 5}},
	{{22, 21, 20}, {16, 23, 22}},
	{{7, 15, 23}, {16, 23, 22}}
};

class MillState
{
public:
	MillState();

	void add(int player, int pos);
	void move(int from, int to);
	void remove(int pos);

	bool ismill(int pos) const;
	QVector<int> eatable(int player) const;
	QVector<MillState> possibilities(int player) const;

	inline int getPlayerAt(int pos) const { return cs[pos]; }
	inline int getNotPlaced(int player) const { return notplaced[player]; }
	inline int getOnBoard(int player) const { return onboard[player]; }
	inline int getRemoved(int player) const { return removed[player]; }

	bool operator ==(const MillState& other) const;

private:
	int notplaced[2];
	int onboard[2];
	int removed[2];
	int cs[24]; // -1=empty

	void eataftermill(QVector<MillState>& poss, MillState& state, int player, int pos) const;

	friend uint qHash(const MillState& key, uint seed);
};

uint qHash(const MillState& key, uint seed = 0);

#endif // MILLSTATE_HH
