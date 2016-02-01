#ifndef BOTMILLS_H
#define BOTMILLS_H

#include <QObject>
#include "millstate.hh"

class BotMills : public QObject
{
	Q_OBJECT
public:
	explicit BotMills(QObject *parent = 0);

	MillState play(MillState start, int player);
};

#endif // BOTMILLS_H
