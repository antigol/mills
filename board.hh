#ifndef BOARD_HH
#define BOARD_HH

#include <QWidget>
#include <QGraphicsScene>
#include "millstate.hh"

class Board : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit Board(QObject *parent = 0);

private:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouse) override;

	bool ismill(int cell);

	QList<QGraphicsEllipseItem*> piecesA, piecesB;

	QGraphicsEllipseItem* selected;
	int turn;
	bool choosetoremove;
	int amountInA;
	int amountInB;

	const QColor colorA = Qt::yellow;
	const QColor colorB = Qt::red;
};

#endif // BOARD_HH
