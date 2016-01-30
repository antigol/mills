#include "millstate.hh"

MillState::MillState()
{
	for (int i = 0; i < 24; ++i) cs[i] = 0;
	oa = ob = 9;
	ka = kb = 0;
	turn = 1;
}

bool MillState::isInPlacementPhase()
{
	return ob > 0;
}

bool MillState::isPlayerATurn()
{
	return turn == 1;
}
