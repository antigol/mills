#ifndef MILLSTATE_HH
#define MILLSTATE_HH

#include <QVector>

/* Players : A and B
 * Player A begins
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

const int mills[][3] = {
	{0,1,2}, {8,9,10}, {16,17,18}, {7,15,23}, {19,11,3}, {22,21,20}, {14,13,12}, {6,5,4},
	{0,7,6}, {8,15,14}, {16,23,22}, {1,9,17}, {21,13,5}, {18,19,20}, {10,11,12}, {2,3,4}
};

class MillState
{
public:
	MillState();

	bool isInPlacementPhase();
	bool isPlayerATurn();
	inline bool isPlayerBTurn() { return !isPlayerATurn(); }

private:
	int oa; // pieces out for A
	int ob; // pieces out for B
	int ka; // pieces killed for A
	int kb; // pieces killed for B
	int turn; // 1=A, 2=B
	int cs[24]; // 0=empty, 1=A, 2=B
};

#endif // MILLSTATE_HH
