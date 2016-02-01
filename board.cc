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

	for (int p : {0, 1}) {
		for (int i = 0; i < 9; ++i) {
			auto piece = addEllipse(-r, -r, 2*r, 2*r, QPen(), QBrush(color[p]));
			piece->setData(PLAYERID, p);
			pieces[p] << piece;
		}
	}

	turn = 0;
	setState(MillState());
}

void Board::setState(const MillState& state)
{
	this->state = state;
	selected = nullptr;
	choosetoremove = false;

	for (int p : {0, 1}) {
		for (auto pce : pieces[p]) {
			pce->setBrush(color[p]);
		}
	}

	int i[2] = {0, 0};

	for (int k = 0; k < 24; ++k) {
		int p = state.getPlayerAt(k);
		if (p != -1) {
			auto pce = pieces[p][i[p]];
			i[p]++;

			pce->setPos(positions[k][0], positions[k][1]);
			pce->setData(CELLPOS, k);
			pce->show();
		}
	}
	for (int p : {0, 1}) {
		while (i[p] < 9 - state.getNotPlaced(p)) {
			auto pce = pieces[p][i[p]];

			pce->setPos(2*c, 0); // out of screen
			pce->setData(CELLPOS, -1);
			pce->hide();

			i[p]++;
		}
		while (i[p] < 9) {
			auto pce = pieces[p][i[p]];

			pce->setPos((2*p-1)*(c+3*r), -c+i[p]*2*c/8);
			pce->setData(CELLPOS, -1);
			pce->show();

			i[p]++;
		}
	}
}

void Board::setTurn(int turn)
{
	this->turn = turn;
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
			if (there && there->data(PLAYERID).toInt() != turn%2 && !state.ismill(idthere)) {
				there->setData(CELLPOS, -1);
				there->setPos(2*c, 0); // out of screen
				there->hide();
				state.remove(idthere);

				turn++;
				choosetoremove = false;
			}
		} else if (turn < 2*9) {
			if (there == nullptr) {
				auto piece = pieces[turn%2][turn/2];
				piece->setPos(x, y);
				piece->setData(CELLPOS, idthere);
				state.add(turn%2, idthere);
				if (state.ismill(idthere) && !state.eatable(1 - turn%2).isEmpty())
					choosetoremove = true;
				else
					turn++;
			}
		} else {
			if (there != nullptr) {
				if (pieces[turn % 2].contains(there)) {
					if (selected) selected->setBrush(color[selected->data(PLAYERID).toInt()]);
					selected = there;
					selected->setBrush(Qt::black);
				}
			} else if (selected != nullptr && there == nullptr) {
				int idfrom = selected->data(CELLPOS).toInt();
				if (state.getOnBoard(turn%2) <= 3 || connectedto[idfrom].contains(idthere)) {
					state.move(idfrom, idthere);
					selected->setPos(x, y);
					selected->setData(CELLPOS, idthere);
					selected->setBrush(color[selected->data(PLAYERID).toInt()]);
					selected = nullptr;

					if (state.ismill(idthere) && !state.eatable(1 - turn%2).isEmpty())
						choosetoremove = true;
					else
						turn++;
				}
			}
		}
	} else {
		if (selected) {
			selected->setBrush(color[selected->data(PLAYERID).toInt()]);
			selected = nullptr;
		}
	}

	if (turn%2 == 1) {
		setState(bot.play(state, 1));
		turn++;
	}
}

