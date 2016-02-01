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
constexpr int INITIALPOS = 2;
constexpr int FINALPOS = 3;

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
			auto pce = addEllipse(-r, -r, 2*r, 2*r, QPen(), QBrush(m_color[p]));
			pce->setData(PLAYERID, p);
			pce->setData(INITIALPOS, pce->pos());
			m_pieces[p] << pce;
		}
	}

	m_turn = 0;
	setState(MillState());

	m_waitHuman = false;
	m_timeline.setDuration(500);
	connect(&m_timeline, SIGNAL(valueChanged(qreal)), SLOT(drawMovement(qreal)));
}

void Board::setState(const MillState& state)
{
	this->m_state = state;
	m_selected = nullptr;
	m_choosetoremove = false;

	for (int p : {0, 1}) {
		for (auto pce : m_pieces[p]) {
			pce->setBrush(m_color[p]);
		}
	}

	int i[2] = {0, 0};

	for (int k = 0; k < 24; ++k) {
		int p = state.getPlayerAt(k);
		if (p != -1) {
			auto pce = m_pieces[p][i[p]];
			i[p]++;

			//pce->setPos(positions[k][0], positions[k][1]);
			pce->setData(INITIALPOS, pce->pos());
			pce->setData(FINALPOS, QPointF(positions[k][0], positions[k][1]));
			pce->setData(CELLPOS, k);
			//pce->show();
		}
	}
	for (int p : {0, 1}) {
		while (i[p] < 9 - state.getNotPlaced(p)) {
			auto pce = m_pieces[p][i[p]];

			//pce->setPos(2*c, 0); // out of screen
			pce->setData(INITIALPOS, pce->pos());
			pce->setData(FINALPOS, QPointF((2*p-1)*2*c, 0));
			pce->setData(CELLPOS, -1);
			//pce->show();

			i[p]++;
		}
		int inc = 0;
		while (i[p] < 9) {
			auto pce = m_pieces[p][i[p]];

			//pce->setPos((2*p-1)*(c+3*r), -c+(inc++)*2*c/8);
			pce->setData(INITIALPOS, pce->pos());
			pce->setData(FINALPOS, QPointF((2*p-1)*(c+3*r), -c+(inc++)*2*c/8));
			pce->setData(CELLPOS, -1);
			//pce->show();

			i[p]++;
		}
	}

	m_timeline.start();
}

void Board::acceptHumanEntry()
{
	m_waitHuman = true;
}

void Board::drawMovement(qreal value)
{
	for (int p : {0, 1}) {
		for (auto pce : m_pieces[p]) {
			QPointF p0 = pce->data(INITIALPOS).toPointF();
			QPointF p1 = pce->data(FINALPOS).toPointF();
			if (value == 1.0) {
				pce->setData(INITIALPOS, p1);
				pce->setPos(p1);
			} else {
				pce->setPos((1.0 - value) * p0 + value * p1);
			}
		}
	}
	update();
}

void Board::mousePressEvent(QGraphicsSceneMouseEvent* mouse)
{
	if (!m_waitHuman) return;

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

	int oldturn = m_turn;

	if (dmin <= r) {
		int x = positions[idthere][0];
		int y = positions[idthere][1];

		QGraphicsEllipseItem* there = qgraphicsitem_cast<QGraphicsEllipseItem*>(itemAt(x, y, QTransform()));

		if (m_choosetoremove) {
			if (there && there->data(PLAYERID).toInt() != m_turn%2 && !m_state.ismill(idthere)) {
				there->setData(CELLPOS, -1);
				there->setData(INITIALPOS, there->pos());
				there->setData(FINALPOS, QPointF((2*(m_turn%2)-1)*2*c, 0));
				m_state.remove(idthere);

				m_turn++;
				m_choosetoremove = false;
			}
		} else if (m_state.getNotPlaced(1) > 0) {
			if (there == nullptr) {
				auto piece = m_pieces[m_turn%2].last();
				//piece->setPos(x, y);
				piece->setData(INITIALPOS, piece->pos());
				piece->setData(FINALPOS, QPointF(x, y));
				piece->setData(CELLPOS, idthere);
				m_state.add(m_turn%2, idthere);
				if (m_state.ismill(idthere) && !m_state.eatable(1 - m_turn%2).isEmpty())
					m_choosetoremove = true;
				else
					m_turn++;
			}
		} else {
			if (there != nullptr) {
				if (m_pieces[m_turn%2].contains(there)) {
					if (m_selected) m_selected->setBrush(m_color[m_selected->data(PLAYERID).toInt()]);
					m_selected = there;
					m_selected->setBrush(Qt::black);
				}
			} else if (m_selected != nullptr && there == nullptr) {
				int idfrom = m_selected->data(CELLPOS).toInt();
				if (m_state.getOnBoard(m_turn%2) <= 3 || connectedto[idfrom].contains(idthere)) {
					m_state.move(idfrom, idthere);
					//m_selected->setPos(x, y);
					m_selected->setData(INITIALPOS, m_selected->pos());
					m_selected->setData(FINALPOS, QPointF(x, y));
					m_selected->setData(CELLPOS, idthere);
					m_selected->setBrush(m_color[m_selected->data(PLAYERID).toInt()]);
					m_selected = nullptr;

					if (m_state.ismill(idthere) && !m_state.eatable(1 - m_turn%2).isEmpty())
						m_choosetoremove = true;
					else
						m_turn++;
				}
			}
		}
		m_timeline.start();

	} else {
		if (m_selected) {
			m_selected->setBrush(m_color[m_selected->data(PLAYERID).toInt()]);
			m_selected = nullptr;
		}
	}

	if (oldturn != m_turn) {
		emit humanPlayed();
		m_waitHuman = false;
	}
}



