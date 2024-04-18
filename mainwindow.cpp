#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "paintarea.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    paintArea = new PaintArea;
    setCentralWidget(paintArea);
    createActions();
    createMenus();
    setWindowTitle(tr("Paint"));
    resize(1000, 700);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event) {
    if(isMaybeSave()){
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::open() {
    if(isMaybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
        if(!fileName.isEmpty()) {
            paintArea->openImage(fileName);
        }
    }
}

void MainWindow::save() {
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::penColor() {
    QColor newColor = QColorDialog::getColor(paintArea->penColor());
    if(newColor.isValid()) {
        paintArea-> setPenColor(newColor);
    }
}

void MainWindow::penWidth() {
    bool ok;
    int newWidth = QInputDialog::getInt(this,
                                        tr("Paint"),
                                        tr("Select pen width: "),
                                        paintArea->penWidth(),
                                        1, 50, 1, &ok);
    if(ok) {
        paintArea->setPenWidth(newWidth);
    }
}

void MainWindow::about() {
    QMessageBox::about(this,
                       tr("About Paint"),
                       tr("<p>The <b>Paint</b> example is awesome<p>"));
}

void MainWindow::createActions() {
    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    foreach(QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());
        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }
    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), paintArea, SLOT(print()));
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    penColorAct = new  QAction(tr("&Pen Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));
    penWidthAct = new  QAction(tr("&Pen Width..."), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    clearScreenAct = new  QAction(tr("&Clear Screen..."), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),paintArea, SLOT(clearImage()));

    aboutAct = new  QAction(tr("&About..."), this);
    connect(aboutAct, SIGNAL(triggered()), SLOT(about()));

    aboutQtAct = new  QAction(tr("&About &Qt..."), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

void MainWindow::createMenus() {
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach(QAction *action, saveAsActs) {
        saveAsMenu->addAction(action);
    }
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu-> addAction(penColorAct);
    optionMenu-> addAction(penWidthAct);
    optionMenu->addSeparator();
    optionMenu-> addAction(clearScreenAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

bool MainWindow::isMaybeSave() {
    if(paintArea->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Paint"),
                                   tr("The image has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if(ret == QMessageBox::Save) {
            return saveFile("png");
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat) {
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    initialPath,
                                                    tr("%1 Files (*.%2);;All Files (*)")
                                                        .arg(QString::fromLatin1(fileFormat.toUpper()), QString::fromLatin1(fileFormat)));

    if(fileName.isEmpty()) {
        return false;
    } else {
        return paintArea->saveImage(fileName, fileFormat.constData());
    }
}












