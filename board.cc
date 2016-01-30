#include "board.hh"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>

constexpr double a = 30, b = 65, c = 100, r = 10;
constexpr double positions[3*8][2] = {
	{-c,-c}, {0,-c}, {c,-c}, {c,0}, {c,c}, {0,c}, {-c,c}, {-c,0},
	{-b,-b}, {0,-b}, {b,-b}, {b,0}, {b,b}, {0,b}, {-b,b}, {-b,0},
	{-a,-a}, {0,-a}, {a,-a}, {a,0}, {a,a}, {0,a}, {-a,a}, {-a,0},
};
constexpr int PLAYERID = 0;
constexpr int CELLPOS = 1;

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
		piece->setData(PLAYERID, 1);
		piece->setData(CELLPOS, -1);
		piecesA << piece;
	}
	for (int i = 0; i < 9; ++i) {
		auto piece = addEllipse(-r, -r, 2*r, 2*r, QPen(), QBrush(colorB));
		piece->setPos(+c+3*r, -c+i*2*c/8);
		piece->setData(PLAYERID, 2);
		piece->setData(CELLPOS, -1);
		piecesB << piece;
	}

	selected = nullptr;
	turn = 0;
	choosetoremove = false;
	amountInA = 9;
	amountInB = 9;
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

	//qDebug("imin=%d dmin=%d", idthere, dmin);

	if (dmin <= r) {
		int x = positions[idthere][0];
		int y = positions[idthere][1];

		QGraphicsEllipseItem* there = qgraphicsitem_cast<QGraphicsEllipseItem*>(itemAt(x, y, QTransform()));

		if (choosetoremove) {
			if (there && there->data(PLAYERID).toInt() != (turn % 2) + 1) {
				there->setData(CELLPOS, -1);
				there->setPos(2*c, 0); // out of screen
				there->hide();
				if (turn % 2 == 0) {
					amountInB--;
				} else {
					amountInA--;
				}
				turn++;
				choosetoremove = false;
			}
		} else if (turn < 2*9) {
			if (there == nullptr) {
				auto piece = turn % 2 == 0 ? piecesA[turn / 2] : piecesB[turn / 2];
				piece->setPos(x, y);
				piece->setData(CELLPOS, idthere);
				if (ismill(idthere))
					choosetoremove = true;
				else
					turn++;
			}
		} else {
			if (there != nullptr) {
				if ((turn % 2 == 0 && piecesA.contains(there)) || (turn % 2 == 1 && piecesB.contains(there))) {
					if (selected) selected->setBrush(selected->data(PLAYERID) == 1 ? colorA : colorB);
					selected = there;
					selected->setBrush(Qt::black);
				}
			} else if (selected != nullptr && there == nullptr) {
				if ((turn%2==0 ? amountInA <= 3 : amountInB <= 3) || connectedto[selected->data(CELLPOS).toInt()].contains(idthere)) {
					selected->setPos(x, y);
					selected->setData(CELLPOS, idthere);
					selected->setBrush(selected->data(PLAYERID) == 1 ? colorA : colorB);
					selected = nullptr;

					if (ismill(idthere))
						choosetoremove = true;
					else
						turn++;
				}
			}
		}
	} else {
		if (selected) {
			selected->setBrush(selected->data(PLAYERID) == 1 ? colorA : colorB);
			selected = nullptr;
		}
	}
}

bool Board::ismill(int cell)
{
	for (int i = 0; i < 16; ++i) {
		if (mills[i][0] == cell || mills[i][1] == cell || mills[i][2] == cell) {
			int n = 0;
			for (auto pce : piecesA) {
				int x = pce->data(CELLPOS).toInt();
				if (mills[i][0] == x || mills[i][1] == x || mills[i][2] == x) n++;
			}
			if (n == 3) return true;
			n = 0;
			for (auto pce : piecesB) {
				int x = pce->data(CELLPOS).toInt();
				if (mills[i][0] == x || mills[i][1] == x || mills[i][2] == x) n++;
			}
			if (n == 3) return true;
		}
	}
	return false;
}
