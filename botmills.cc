#include "botmills.hh"

BotMills::BotMills(QObject *parent) : QObject(parent)
{

}

MillState BotMills::play(MillState state, int player)
{
	QVector<MillState> poss = state.possibilities(player);

	return poss[qrand() % poss.size()];
}
