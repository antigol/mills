#include "mainwindow.hh"
#include <QMenuBar>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	m_board = new Board(this);

	auto view = new QGraphicsView(m_board, this);
	view->setRenderHint(QPainter::Antialiasing, false);
	setCentralWidget(view);

	QMenu* play = menuBar()->addMenu("Play");
	play->addAction("New game", this, SLOT(newgame()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::newgame()
{

}
