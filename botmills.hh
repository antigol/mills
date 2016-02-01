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

	void play(MillState state, int player);

	inline const MillState& getResult() const { return result; }

private:
	bool private_play(MillState state, int player, int deepness, MillState& result, double& score);

	void run() override;

	MillState result;
	MillState state;
	int player;
	QTime timer;
	int maxtime;
};

#endif // BOTMILLS_H
