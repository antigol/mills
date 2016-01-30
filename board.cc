#include "board.hh"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>

constexpr double a = 30, b = 65, c = 100, r = 10;
constexpr double positions[3*8][2] = {
	{-c,-c}, {0,-c}, {c,-c}, {c,0}, {c,c}, {0,c}, {-c,c}, {-c,0},
	{-b,-b}, {0,-b}, {b,-b}, {b,0}, {b,b}, {0,b}, {-b,b}, {-b,0},
	{-a,-a}, {0,-a}, {a,-a}, {a,0}, {a,a}, {0,a}, {-a,a}, {-a,0},
};
const QColor colorA(Qt::yellow);
const QColor colorB(Qt::red);

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
		auto piece = addEllipse(-r, -r, 2*r, 2*r, QPen(), QBrush(colorA));
		piece->setPos(-c-3*r, -c+i*2*c/8);
		piece->setData(0, 1);
		piecesA << piece;
	}
	for (int i = 0; i < 9; ++i) {
		auto piece = addEllipse(-r, -r, 2*r, 2*r, QPen(), QBrush(colorB));
		piece->setPos(+c+3*r, -c+i*2*c/8);
		piece->setData(0, 2);
		piecesB << piece;
	}

	selected = nullptr;
	turn = 0;
}

void Board::mousePressEvent(QGraphicsSceneMouseEvent* mouse)
{
	int idthere = 0;
	int dmin = c;
	for (int i = 0; i < 24; ++i) {
		int dx = mouse->scenePos().x() - positions[i][0];
		int dy = mouse->scenePos().y() - positions[i][1];
		int d = sqrt(dx*dx + dy*dy);

		if (d < dmin) {
			dmin = d;
			idthere = i;
		}
	}

	qDebug("imin=%d dmin=%d", idthere, dmin);

	if (dmin <= r) {
		int x = positions[idthere][0];
		int y = positions[idthere][1];

		QGraphicsEllipseItem* there = qgraphicsitem_cast<QGraphicsEllipseItem*>(itemAt(x, y, QTransform()));


		if (turn < 2*9) {
			if (there == nullptr) {
				if (turn % 2 == 0) {
					piecesA[turn / 2]->setPos(x, y);
				} else {
					piecesB[turn / 2]->setPos(x, y);
				}
				turn++;
			}
		} else {
			if (there != nullptr) {
				if ((turn % 2 == 0 && piecesA.contains(there)) || (turn % 2 == 1 && piecesB.contains(there))) {
					if (selected) selected->setBrush(selected->data(0) == 1 ? colorA : colorB);
					selected = there;
					selected->setBrush(Qt::black);
					selected->setData(1, idthere);
				}
			} else if (selected != nullptr && there == nullptr) {
				if (/* allowed */true) {
					selected->setPos(x, y);
					selected->setBrush(selected->data(0) == 1 ? colorA : colorB);
					selected = nullptr;
					turn++;
				}
			}
		}
	} else {
		if (selected) {
			selected->setBrush(selected->data(0) == 1 ? colorA : colorB);
			selected = nullptr;
		}
	}
}
