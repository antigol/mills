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
	void setState(const MillState& state);
	const MillState& state() const { return m_state; }
	inline void setTurn(int turn) { m_turn = turn; }
	inline int turn() const { return m_turn; }
	void nextTurn() { m_turn++; }
	inline int whoPlays() const { return m_turn%2; }

public slots:
	void acceptHumanEntry();

signals:
	void humanPlayed();

private:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouse) override;

	int m_turn;
	QList<QGraphicsEllipseItem*> m_pieces[2];
	QGraphicsEllipseItem* m_selected;
	bool m_choosetoremove;

	MillState m_state;
	bool m_waitHuman;

	QColor m_color[2] = {Qt::yellow, Qt::red};
};

#endif // BOARD_HH
