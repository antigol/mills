#include "botmills.hh"

BotMills::BotMills(QObject *parent) : QObject(parent)
{

}

MillState BotMills::play(MillState start, int player)
{
	if (start.getNotPlaced(player) > 0) {
		for (int i = 0; i < 24; ++i) {
			if (start.getPlayerAt(i) == -1) {
				start.add(player, i);
				return start;
			}
		}
	}

	for (int i = 0; i < 24; ++i) {
		if (start.getPlayerAt(i) == player) {
			for (int j : connectedto[i]) {
				if (start.getPlayerAt(j) == -1) {
					start.move(i, j);
					return start;
				}
			}
		}
	}

	return start;
}
