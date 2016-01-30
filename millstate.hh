#ifndef MILLSTATE_HH
#define MILLSTATE_HH

/* Players : A and B
 * Player A begins
 *
 *  00       01       02
 *    08    09    10
 *       16 17 18
 * 07 15 23    19 11 03
 *       22 21 20
 *    14    13    12
 * 06       05       04
 */

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
