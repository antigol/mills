#include "mainwindow.hh"
#include <QMenuBar>
#include <QGraphicsView>
#include <QStatusBar>
#include <QInputDialog>

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

	QMenu* options = menuBar()->addMenu("Options");
	options->addAction("Set bot time", this, SLOT(action_setbottime()));

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
	m_board->setWhoplay(0);

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
	m_board->setWhoplay(0);

	m_bot.play(m_board->state(), 0);
	statusBar()->showMessage("I am thinking...");
	menuBar()->setDisabled(true);

	m_history.clear();
	m_action_undo->setDisabled(true);
	m_action_redo->setDisabled(true);
}

void MainWindow::humanPlayed()
{
	if (m_board->state().getRemoved(m_board->whoPlays()) > 6) {
		statusBar()->showMessage("Congratulations, You win !");
	} else {
		m_bot.play(m_board->state(), m_board->whoPlays());
		statusBar()->showMessage("I am thinking...");
		menuBar()->setDisabled(true);
	}
	while (m_history_position < m_history.size()-1) m_history.removeLast();
	m_action_redo->setDisabled(true);
}

void MainWindow::botFinished()
{
	menuBar()->setEnabled(true);

	m_board->setState(m_bot.getResult());
	m_board->nextTurn();
	if (m_board->state().possibilities(m_board->whoPlays()).isEmpty()) {
		m_board->nextTurn(); // turn of human passes
		statusBar()->showMessage("You cannot play, you pass your turn");
		m_bot.play(m_board->state(), m_board->whoPlays());
	}
	if (m_board->state().getRemoved(m_board->whoPlays()) > 6) {
		statusBar()->showMessage("I won that match.");
	} else {
		if (m_bot.whowin() == 1 - m_board->whoPlays())
			statusBar()->showMessage("I already won, but it's your turn");
		else
			statusBar()->showMessage("Ok, Now it's your turn");
		m_board->acceptHumanEntry();
	}
	m_history << m_board->state();
	m_history_position = m_history.size() - 1;
	if (m_history_position > 0) m_action_undo->setEnabled(true);
}

void MainWindow::undo()
{
	if (m_bot.isRunning()) return;
	if (m_history_position > 0) {
		m_history_position--;
		m_board->setState(m_history[m_history_position]);
		m_board->acceptHumanEntry();
		m_action_redo->setEnabled(true);
		if (m_history_position == 0) m_action_undo->setDisabled(true);
	} else {
		m_action_undo->setDisabled(true);
	}
}

void MainWindow::redo()
{
	if (m_bot.isRunning()) return;
	if (m_history_position+1 < m_history.size()) {
		m_history_position++;
		m_board->setState(m_history[m_history_position]);
		m_action_undo->setEnabled(true);
		if (m_history_position == m_history.size() - 1) m_action_redo->setDisabled(true);
	} else {
		m_action_redo->setDisabled(true);
	}
}

void MainWindow::action_setbottime()
{
	int ans = QInputDialog::getInt(this, "Bot Max Time", "The time allowed for the bot to think about the next move. Value in milliseconds.", m_bot.maxTime(), 0);
	m_bot.setMaxTime(ans);
}
