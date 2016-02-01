#ifndef BOARD_HH
#define BOARD_HH

#include <QWidget>
#include <QGraphicsScene>
#include "millstate.hh"
#include "botmills.hh" // to move to mainwindow

class Board : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit Board(QObject *parent = 0);
	void setState(const MillState& state);
	void setTurn(int turn);

private:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouse) override;


	int turn;
	QList<QGraphicsEllipseItem*> pieces[2];
	QGraphicsEllipseItem* selected;
	bool choosetoremove;

	MillState state;

	const QColor color[2] = {Qt::yellow, Qt::red};

	BotMills bot; // to move to mainwindow
};

#endif // BOARD_HH
