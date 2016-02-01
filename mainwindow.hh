#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include "board.hh"
#include "botmills.hh"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void newgame_human();
	void newgame_bot();
	void humanPlayed();
	void botFinished();
	void undo();
	void redo();

private:
	Board* m_board;
	BotMills m_bot;
	QList<MillState> m_history;
	int m_history_position;
	QAction* m_action_undo;
	QAction* m_action_redo;
};

#endif // MAINWINDOW_HH
