#include "botmills.hh"

BotMills::BotMills(QObject *parent) : QThread(parent)
{
	maxtime = 6000;
}

void BotMills::play(MillState state, int player)
{
	this->state = state;
	this->player = player;

	start();
}

bool BotMills::private_play(MillState state, int player, int deepness, MillState& result, double& score)
{
	QVector<MillState> possibilities = state.possibilities(player);

	if (possibilities.isEmpty()) {
		result = state;
		score = 0.0;
		return true;
	}

	if (timer.elapsed() > maxtime) {
		qDebug("out of time");
		return false;
	}

	double bestscore = -1000;
	MillState bestpossibility;
	for (MillState s : possibilities) {
		double sc;

		if (s.getKilled(1-player) > 6) {
			sc = 100 + deepness;
		} else if (deepness > 0) {
			MillState ns;
			double nsc;
			if (!private_play(s, 1-player, deepness-1, ns, nsc)) return false;
			sc = -nsc;
		} else {
			sc = s.getKilled(1-player) - s.getKilled(player);
		}

		if (sc > bestscore) {
			bestscore = sc;
			bestpossibility = s;
		}
	}

	//qDebug("deep %d player %d : score %d (%d possibilities)", deepness, player, bestscore, possibilities.size());
	result = bestpossibility;
	score = bestscore;
	return true;
}

void BotMills::run()
{
	timer.start();

	int lasttime = 0;
	for (int deepness = 2; maxtime - timer.elapsed() > lasttime; ++deepness) {
		double score = 0.0;
		MillState output;
		lasttime = timer.elapsed();
		if (private_play(state, player, deepness, output, score)) {
			qDebug("deepness %d: score %f time %d", deepness, score, timer.elapsed());
			result = output;
		}
		lasttime = timer.elapsed() - lasttime;
	}
}
