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

	QList<QGraphicsEllipseItem*> piecesA, piecesB;

	MillState state;
};

#endif // BOARD_HH
