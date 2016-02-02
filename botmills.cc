#include "botmills.hh"

BotMills::BotMills(QObject *parent) : QThread(parent)
{
	m_maxtime = 1000;
}

void BotMills::play(MillState initialstate, int player)
{
	this->m_initialstate = initialstate;
	this->m_player = player;

	start();
}

bool BotMills::private_play(const MillState& initialstate, int player, int deepness, MillState& result)
{
	QVector<MillState> opportunities = initialstate.possibilities(player);

	double bestscore = -1000;
	QVector<MillState> bestopportunity;
	for (const MillState& opp : opportunities) {
		double sc;

		if (opp.getKilled(1-player) > 6) {
			sc = 100 + deepness;
		} else if (deepness > 0) {
			if (!private_play_more(opp, 1-player, deepness-1, sc, -1000.0, 1000.0)) return false;
			sc = -sc;
		} else {
			sc = opp.getKilled(1-player) - opp.getKilled(player);
		}

		if (sc > bestscore) {
			bestscore = sc;
			bestopportunity.clear();
			bestopportunity << opp;
		} else if (sc == bestscore) {
			bestopportunity << opp;
		}
	}

	if (bestscore >= 10) m_whowin = player;
	if (bestscore <= 10) m_whowin = 1 - player;
	//qDebug("deepness %d player %d : score %.1f (%d possibilities)", deepness, player, bestscore, opportunities.size());
	result = bestopportunity[qrand() % bestopportunity.size()];
	return true;
}

bool BotMills::private_play_more(const MillState& initialstate, int player, int deepness, double& score, double alpha, double beta)
{
	QVector<MillState> opportunities = initialstate.possibilities(player);

	if (opportunities.isEmpty()) {
		score = 0.0;
		return true;
	}

	if (m_chrono.elapsed() > m_maxtime) return false;

	score = -1000.0;
	for (const MillState& opp : opportunities) {
		double sc;

		if (opp.getKilled(1-player) > 6) {
			sc = 10 * (deepness + 1);
		} else if (deepness > 0) {
			if (!private_play_more(opp, 1-player, deepness-1, sc, -beta, -alpha)) return false;
			sc = -sc;
		} else {
			sc = opp.getKilled(1-player) - opp.getKilled(player);
			//sc -= opp.eatable(player).size();
		}

		if (sc > score) {
			score = sc;
			if (score > alpha) {
				alpha = score;
				if (alpha >= beta) return true;
			}
		}
	}
	return true;
}

void BotMills::run()
{
	m_chrono.start();
	m_whowin = -1;

	if (m_initialstate.possibilities(m_player).isEmpty()) {
		m_result = m_initialstate;
		return;
	}

	int lasttime = 0;
	int deepness;
	for (deepness = 2; m_maxtime - m_chrono.elapsed() > lasttime; ++deepness) {
		lasttime = m_chrono.elapsed();
		private_play(m_initialstate, m_player, deepness, m_result);
		lasttime = m_chrono.elapsed() - lasttime;
	}
	qDebug("deepness %d: time %d", deepness, m_chrono.elapsed());
}
