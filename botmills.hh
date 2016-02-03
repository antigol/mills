#ifndef BOTMILLS_H
#define BOTMILLS_H

#include <QThread>
#include <QTime>
#include "millstate.hh"

class BotMills : public QThread
{
	Q_OBJECT
public:
	explicit BotMills(QObject *parent = 0);

	void play(MillState initialstate, int player);

	inline const MillState& getResult() const { return m_result; }

	inline int maxTime() const { return m_maxtime; }
	inline void setMaxTime(int maxtime) { m_maxtime = maxtime; }
	inline bool whowin() const { return m_whowin; }

private:
	bool private_play(int deepness);
	double private_play_more(const MillState& initialstate, int player, int deepness, double alpha, double beta, bool& ok);

	void run() override;

	MillState m_result;
	MillState m_initialstate;
	int m_player;
	QTime m_chrono;
	int m_maxtime;
	int m_whowin;
};

#endif // BOTMILLS_H
