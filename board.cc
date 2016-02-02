#include "board.hh"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include <QApplication>

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
	m_timeline.setDuration(200);
	connect(&m_timeline, SIGNAL(valueChanged(qreal)), SLOT(drawMigration(qreal)));
}

void Board::setState(const MillState& state)
{
	while (m_timeline.state() == QTimeLine::Running) {
		QApplication::instance()->processEvents(QEventLoop::AllEvents, 10);
	}

	m_selected = nullptr;
	m_choosetoremove = false;

	for (int p : {0, 1}) {
		for (auto pce : m_pieces[p]) {
			pce->setBrush(m_color[p]);
		}
	}


	QList<int> orig[2];
	QList<int> dest[2];

	for (int k = 0; k < 24; ++k) {
		int p0 = m_state.getPlayerAt(k);
		int p1 = state.getPlayerAt(k);

		if (p0 != -1) orig[p0] << k;
		if (p1 != -1) dest[p1] << k;
	}

	for (int p : {0, 1}) {
		for (int i = 0; i < state.getNotPlaced(p); ++i) {
			dest[p] << -9+i;
		}
		for (int i = 0; i < m_state.getNotPlaced(p); ++i) {
			orig[p] << -9+i;
		}

		for (int i = 0; i < state.getKilled(p); ++i) {
			dest[p] << -1-i;
		}
		for (int i = 0; i < m_state.getKilled(p); ++i) {
			orig[p] << -1-i;
		}
	}


	for (int p : {0, 1}) {
		Q_ASSERT(orig[p].size() == dest[p].size());
		Q_ASSERT(orig[p].size() == 9);

		qSort(orig[p]);
		qSort(dest[p]);
		int end = 9;
		for (int i = 0; i < end; ++i) {
			if (orig[p][i] < dest[p][i]) {
				orig[p].move(i, 8);
				i--;
				end--;
			} else if (dest[p][i] < orig[p][i]) {
				dest[p].move(i, 8);
				i--;
				end--;
			}
		}


		for (int i = 0; i < 9; ++i) {
			int k0 = orig[p][i];
			int k1 = dest[p][i];
			QPointF posi0, posi1;

			if (k0 >= 0) posi0 = QPointF(positions[k0][0], positions[k0][1]);
			else posi0 = QPointF((2*p-1)*(c+3*r), c+c*(k0+1.0)/4.0);

			if (k1 >= 0) posi1 = QPointF(positions[k1][0], positions[k1][1]);
			else posi1 = QPointF((2*p-1)*(c+3*r), c+c*(k1+1.0)/4.0);

			auto pce = m_pieces[p][i];
			pce->setData(INITIALPOS, posi0);
			pce->setData(FINALPOS, posi1);
			pce->setData(CELLPOS, k1);
		}
	}

	this->m_state = state;

	m_timeline.start();
}

void Board::acceptHumanEntry()
{
	m_waitHuman = true;
}

void Board::drawMigration(qreal value)
{
	for (int p : {0, 1}) {
		for (auto pce : m_pieces[p]) {
			QPointF p0 = pce->data(INITIALPOS).toPointF();
			QPointF p1 = pce->data(FINALPOS).toPointF();
			pce->setPos((1.0 - value) * p0 + value * p1);
		}
	}

	update();
}

void Board::finishMigration()
{
	for (int p : {0, 1}) {
		for (auto pce : m_pieces[p]) {
			QPointF p = pce->data(FINALPOS).toPointF();
			pce->setData(INITIALPOS, p);
			pce->setPos(p);
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
				MillState state = m_state;
				state.remove(idthere);
				setState(state);

				m_turn++;
			}
		} else if (m_state.getNotPlaced(1) > 0) {
			if (there == nullptr) {
				MillState state = m_state;
				state.add(m_turn%2, idthere);
				setState(state);

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
					MillState state = m_state;
					state.move(idfrom, idthere);
					setState(state);

					if (m_state.ismill(idthere) && !m_state.eatable(1 - m_turn%2).isEmpty())
						m_choosetoremove = true;
					else
						m_turn++;
				}
			}
		}

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



