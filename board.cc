#include "board.hh"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>

constexpr double a = 30, b = 65, c = 100, r = 10;
constexpr double positions[3*8][2] = {
	{-c,-c}, {0,-c}, {c,-c}, {c,0}, {c,c}, {0,c}, {-c,c}, {-c,0},
	{-b,-b}, {0,-b}, {b,-b}, {b,0}, {b,b}, {0,b}, {-b,b}, {-b,0},
	{-a,-a}, {0,-a}, {a,-a}, {a,0}, {a,a}, {0,a}, {-a,a}, {-a,0},
};

/* 00       01       02
 *    08    09    10
 *       16 17 18
 * 07 15 23    19 11 03
 *       22 21 20
 *    14    13    12
 * 06       05       04
 */

Board::Board(QObject* parent) : QGraphicsScene(parent)
{
	setSceneRect(-c-2.5*r - 2*r, -c-2*r, 2*c+4*r + 5*r, 2*c+4*r);
	addRect(-a, -a, 2*a, 2*a);
	addRect(-b, -b, 2*b, 2*b);
	addRect(-c, -c, 2*c, 2*c);
	addLine(0,-a, 0,-c);
	addLine(0,a, 0,c);
	addLine(a,0, c,0);
	addLine(-a,0, -c,0);

	for (int i = 0; i < 9; ++i) {
		auto piece = addEllipse(-r, -r, 2*r, 2*r, QPen(), QBrush(Qt::yellow));
		piece->setPos(-c-3*r, -c+i*2*c/8);
		piecesA << piece;
	}
	for (int i = 0; i < 9; ++i) {
		auto piece = addEllipse(-r, -r, 2*r, 2*r, QPen(), QBrush(QColor(240,0,0)));
		piece->setPos(+c+3*r, -c+i*2*c/8);
		piecesB << piece;
	}
}

void Board::mousePressEvent(QGraphicsSceneMouseEvent* mouse)
{
	int imin = 0;
	int dmin = c;
	for (int i = 0; i < 24; ++i) {
		int dx = mouse->scenePos().x() - positions[i][0];
		int dy = mouse->scenePos().y() - positions[i][1];
		int d = sqrt(dx*dx + dy*dy);

		if (d < dmin) {
			dmin = d;
			imin = i;
		}
	}

	qDebug("imin=%d dmin=%d", imin, dmin);

	if (dmin <= r) {
		int x = positions[imin][0];
		int y = positions[imin][1];

		piecesA.first()->setPos(x, y);
	}
}
