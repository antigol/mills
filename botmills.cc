#include "botmills.hh"

BotMills::BotMills(QObject *parent) : QThread(parent)
{
	m_maxtime = 500;
}

void BotMills::play(MillState initialstate, int player)
{
	this->m_initialstate = initialstate;
	this->m_player = player;

	start();
}

constexpr double infinity = 10000.0;

bool BotMills::private_play(int deepness)
{
	QVector<MillState> opportunities = m_initialstate.possibilities(m_player);
	int ir = opportunities.indexOf(m_result);
	if (ir != -1) {
		opportunities[ir] = opportunities[0];
		opportunities[0] = m_result;
	}

	bool ok = true;

	double alpha = -infinity;
	double beta  =  infinity;

	double v = -infinity;
	QList<MillState> bestopportunities;

	for (const MillState& opp : opportunities) {

		double s = -private_play_more(opp, 1-m_player, deepness - 1, -beta, -alpha, ok);
		if (!ok) return false;
		// alpha <= s <= beta
		//qDebug("%.1f <= %.1f <= %.1f", alpha, s, beta);

		if (s > v) {
			v = s;
			bestopportunities.clear();
			bestopportunities << opp;
		} else if (s == v) {
			bestopportunities << opp;
		}

		if (v > alpha) alpha = v;
	}

	if (v >= 10) m_whowin = m_player;
	if (v <= 10) m_whowin = 1 - m_player;
	//qDebug("deepness %d : score %.1f (%d bestopportunities)", deepness, v, bestopportunities.size());
	m_result = bestopportunities[qrand() % bestopportunities.size()];
	return true;
}

double BotMills::private_play_more(const MillState& state, int player, int deepness, double alpha, double beta, bool& ok)
{
	if (state.getKilled(player) > 6) {
		return -10 * (deepness + 1);
	} else if (deepness == 0) {
		return state.getKilled(1-player) - state.getKilled(player);
	}

	QVector<MillState> opportunities = state.possibilities(player);
	if (opportunities.isEmpty()) {
		return 0.0;
	}
	if (m_chrono.elapsed() > m_maxtime) {
		ok = false;
		return 0.0;
	}


	double v = -infinity;
	for (const MillState& opp : opportunities) {
		double s = -private_play_more(opp, 1-player, deepness - 1, -beta, -alpha, ok);
		if (!ok) return 0.0;
		// alpha <= s <= beta

		if (s > v) v = s;
		if (s > alpha) alpha = s;
		// beta cuf-off
		if (beta < v) break; // the strict inequality is needed, otherwise => dumb AI
	}

	return v;
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
	int deepness = 0;
	while (m_maxtime - m_chrono.elapsed() > lasttime) {
		lasttime = m_chrono.elapsed();
		deepness++;
		if (!private_play(deepness)) {
			deepness--;
			break;
		}
		lasttime = m_chrono.elapsed() - lasttime;
	}
	qDebug("deepness %d: time %d", deepness, m_chrono.elapsed());
}
