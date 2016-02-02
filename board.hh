#ifndef BOARD_HH
#define BOARD_HH

#include <QWidget>
#include <QGraphicsScene>
#include <QTimeLine>
#include "millstate.hh"

class Board : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit Board(QObject *parent = 0);
	void setState(const MillState& state);
	const MillState& state() const { return m_state; }
	inline void setWhoplay(int who) { m_whoplay = who; }
	void nextTurn() { m_whoplay = 1 - m_whoplay; }
	inline int whoPlays() const { return m_whoplay; }

public slots:
	void acceptHumanEntry();

signals:
	void humanPlayed();

private slots:
	void drawMigration(qreal value);
	void finishMigration();

private:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouse) override;

	int m_whoplay;
	QList<QGraphicsEllipseItem*> m_pieces[2];
	QGraphicsEllipseItem* m_selected;
	bool m_choosetoremove;

	MillState m_state;
	bool m_waitHuman;

	QColor m_color[2] = {Qt::yellow, Qt::red};
	QTimeLine m_timeline;
};

#endif // BOARD_HH
