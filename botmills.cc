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
	QVector<MillState> children = m_initialstate.possibilities(m_player);
	int ir = children.indexOf(m_result);
	if (ir != -1) {
		children[ir] = children[0];
		children[0] = m_result;
	}

	bool ok = true;

	double alpha = -infinity;
	double beta  =  infinity;

	double bestValue = -infinity;
	QList<MillState> bestMoves;

	for (const MillState& child : children) {

		double v = -negamax(child, 1-m_player, deepness - 1, -beta, -alpha, ok);
		if (!ok) return false;
		// alpha <= v <= beta

		if (v > bestValue) {
			bestValue = v;
			bestMoves.clear();
			bestMoves << child;
		} else if (v == bestValue) {
			bestMoves << child;
		}

		if (v > alpha) alpha = v;
	}

	if (bestValue >= 10) m_whowin = m_player;
	if (bestValue <= 10) m_whowin = 1 - m_player;
	m_result = bestMoves[qrand() % bestMoves.size()];
	return true;
}

double BotMills::negamax(const MillState& state, int player, int deepness, double alpha, double beta, bool& ok)
{
	if (state.getKilled(player) > 6) {
		return -10 * (deepness + 1);
	} else if (deepness == 0) {
		return state.getKilled(1-player) - state.getKilled(player);
	}

	QVector<MillState> children = state.possibilities(player);
	if (children.isEmpty()) {
		return 0.0;
	}
	if (m_chrono.elapsed() >= m_maxtime) {
		ok = false;
		return 0.0;
	}


	double bestValue = -infinity;
	for (const MillState& child : children) {
		double v = -negamax(child, 1-player, deepness - 1, -beta, -alpha, ok);
		if (!ok) return 0.0;
		// alpha <= v <= beta

		if (v > bestValue) bestValue = v;
		if (v > alpha) alpha = v;
		// beta cuf-off
		if (alpha >= beta) break;
	}

	return bestValue;
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
