#include "mainwindow.hh"
#include <QMenuBar>
#include <QGraphicsView>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	m_board = new Board(this);
	statusBar();

	auto view = new QGraphicsView(m_board, this);
	view->setRenderHint(QPainter::Antialiasing, false);
	setCentralWidget(view);

	QMenu* play = menuBar()->addMenu("Play");
	QMenu* newgame = play->addMenu("New game");
	newgame->addAction("I begin", this, SLOT(newgame_human()));
	newgame->addAction("You begin", this, SLOT(newgame_bot()));

	QMenu* history = menuBar()->addMenu("History");
	m_action_undo = history->addAction("Undo action", this, SLOT(undo()), QKeySequence::Undo);
	m_action_redo = history->addAction("Redo action", this, SLOT(redo()), QKeySequence::Redo);
	m_action_undo->setDisabled(true);
	m_action_redo->setDisabled(true);

	connect(m_board, SIGNAL(humanPlayed()), this, SLOT(humanPlayed()));
	connect(&m_bot, SIGNAL(finished()), this, SLOT(botFinished()));

	m_history_position = 0;
}

MainWindow::~MainWindow()
{

}

void MainWindow::newgame_human()
{
	m_board->setState(MillState());
	m_board->setTurn(0);

	m_board->acceptHumanEntry();
	statusBar()->showMessage("Ok, You begin");

	m_history.clear();
	m_history << m_board->state();
	m_history_position = 0;
	m_action_undo->setDisabled(true);
	m_action_redo->setDisabled(true);
}

void MainWindow::newgame_bot()
{
	m_board->setState(MillState());
	m_board->setTurn(0);

	m_bot.play(m_board->state(), 0);
	statusBar()->showMessage("I am thinking...");

	m_history.clear();
	m_action_undo->setDisabled(true);
	m_action_redo->setDisabled(true);
}

void MainWindow::humanPlayed()
{
	m_bot.play(m_board->state(), m_board->whoPlays());
	statusBar()->showMessage("I am thinking...");

	while (m_history_position < m_history.size()-1) m_history.removeLast();
	m_action_redo->setDisabled(true);
}

void MainWindow::botFinished()
{
	m_board->setState(m_bot.getResult());
	m_board->nextTurn();
	if (m_board->state().possibilities(m_board->whoPlays()).isEmpty()) {
		m_board->nextTurn(); // turn of human passes
		statusBar()->showMessage("You cannot play, you pass your turn");
		m_bot.play(m_board->state(), m_board->whoPlays());
	}

	statusBar()->showMessage("Ok, Now it's your turn");
	m_board->acceptHumanEntry();

	m_history << m_board->state();
	m_history_position = m_history.size() - 1;
	if (m_history_position > 0) m_action_undo->setEnabled(true);
}

void MainWindow::undo()
{
	if (m_history_position > 0) {
		m_history_position--;
		m_board->setState(m_history[m_history_position]);
		m_action_redo->setEnabled(true);
		if (m_history_position == 0) m_action_undo->setDisabled(true);
	}
}

void MainWindow::redo()
{
	if (m_history_position+1 < m_history.size()) {
		m_history_position++;
		m_board->setState(m_history[m_history_position]);
		m_action_undo->setEnabled(true);
		if (m_history_position == m_history.size() - 1) m_action_redo->setDisabled(true);
	}
}
