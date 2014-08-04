/****************************************************************************
**
** Copyright (c) 2013 Samuel Aaron
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
**
****************************************************************************/

#include <QMap>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QDockWidget>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextStream>
#include <QTextEdit>
#include <QToolBar>
#include <QProcess>
#include <QFont>
#include <QTabWidget>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerruby.h>
#include <QLabel>
#include <QGridLayout>

#include "mainwindow.h"
#include "lexer.h"

#define RHS_WIDTH     250

/******************* Styling *******************
 *                                             *
 * TODO: Move away from using the preprocessor *
 *                                             *
 ***********************************************/

/* General App Style */
#define APP_STYLING  " \
QWidget { \
  background-color: #ffffff; \
}"

/* The tabs which contain the main workspace */
#define TABS_STYLING  " \
QTabWidget::pane { /* The tab widget frame */ \
    border-top: 2px solid #C2C7CB; \
} \
QTabWidget::tab-bar { \
    left: 5px; /* move to the right by 5px */ \
} \
QTabBar { \
    outline: none; \
} \
\
/* Style the tab using the tab sub-control. Note that it reads QTabBar _not_ QTabWidget */ \
QTabBar::tab { \
    background: #e7e7e8; \
    border: 0px solid #C4C4C3; \
    border-bottom-color: #C2C7CB; /* same as the pane color */ \
    border-top-left-radius: 1px; \
    border-top-right-radius: 1px; \
    min-width: 8ex; \
    height: 34px; \
    width: 100px; \
    margin-left: 5px; \
    font-family: 'Bariol'; \
    font-size: 18px; \
    color: #636466; \
} \
QTabBar::tab:selected, QTabBar::tab:hover { \
    background: #f0f0f1; \
} \
QTabBar::tab:selected { \
    height: 36px; \
    border-color: #9B9B9B; \
    border-bottom-color: #C2C7CB; /* same as pane color */ \
} \
QTabBar::tab:!selected { \
    margin-top: 2px; /* make non-selected tabs look smaller */ \
}"

/* The main workspace */
#define WORKSPACE_STYLING " \
QSciScintilla { \
    padding: 30px; \
}"

#define MARGIN_WIDTH  '0'

#define MARGIN_BG_COLOUR "#e7e7e8"

#define MARGIN_FG_COLOUR "#636466"

#define STYLE_WORKSPACE(workspace) \
  workspace->setUtf8(true); \
  workspace->setAutoIndent(true); \
  workspace->setMarginsFont(marginFont); \
  workspace->setMarginWidth(0, MARGIN_WIDTH); \
  workspace->setMarginLineNumbers(0, true); \
  workspace->setMarginsBackgroundColor(QColor(MARGIN_BG_COLOUR)); \
  workspace->setMarginsForegroundColor(QColor(MARGIN_FG_COLOUR)); \
  workspace->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); \
  workspace->setIndentationWidth(4); \
  workspace->setTabWidth(4); \
  workspace->setLexer(lexer); \
  workspace->zoomIn(13); \
  //workspace->autoCompleteFromAll(); \
  //workspace->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); \


/* The label describing the output pane */
#define OUTPUT_LABEL_STYLING  " \
QLabel { \
    background-color: #e7e7e8; \
    color: #636466; \
    margin-top: 2px; \
    margin-bottom: 0; \
    padding: 8px; \
    font-family: 'Bariol'; \
    font-size: 18px; \
    color: #636466; \
}"

/* The pane showing the output */
#define OUTPUT_PANE_STYLING  " \
QTextEdit { \
    background-color: #ffffff; \
    margin: 0; \
    font-family: 'Bariol'; \
    font-size: 30px; \
}"

/* The label describing the error pane */
#define ERROR_LABEL_STYLING  " \
QLabel { \
    background-color: #e7e7e8; \
    color: #636466; \
    margin-top: 5px; \
    margin-bottom: 0; \
    padding: 9px; \
    font-family: 'Bariol'; \
    font-size: 18px; \
    color: #636466; \
}"

/* The pane showing any error messages */
#define ERROR_PANE_HEIGHT  150
#define ERROR_PANE_STYLING  " \
QsciScintilla { \
    font-family: 'Bariol'; \
    margin: 0; \
}"

/* The dockable toolbar containing the play, pause, etc buttons */
#define FILE_TOOLBAR_STYLING  " \
QToolBar { \
    border: 0px solid black; /* Needed to make the background persistent when the bar is docked at the top or bottom */\
    background: #ffffff;\
    spacing: 3px; /* spacing between items in the tool bar */ \
}"

/***** End *****
 *      of     *
 *** styling ***/


/***********************************************************
 ***********************************************************
 *********************** Constructor ***********************
 ***********************************************************
 ***********************************************************/

MainWindow::MainWindow(QApplication &app)
{
  this->setWindowIcon(QIcon(":app_icon"));

  ensureWorkspaces();

  runProcess =  NULL;
  groupName = "default";
  QFile file("/tmp/sonic-pi/group-name");
  bool ret = file.open(QIODevice::ReadOnly | QIODevice::Text);
  if( ret )
    {
      QTextStream stream(&file);
      groupName = stream.readAll().trimmed();
    }

  QMap<QString, QString> map;
  tabs = new QTabWidget();
  tabs->setStyleSheet(TABS_STYLING);
  tabs->setTabsClosable(false);
  tabs->setMovable(false);
  //setCentralWidget(tabs);

  workspace1 = new QsciScintilla;
  workspace2 = new QsciScintilla;
  workspace3 = new QsciScintilla;
  workspace4 = new QsciScintilla;
  workspace5 = new QsciScintilla;
  workspace6 = new QsciScintilla;
  //workspace7 = new QsciScintilla;
  //workspace8 = new QsciScintilla;

  QString w1 = "Song 1";
  QString w2 = "Song 2";
  QString w3 = "Song 3";
  QString w4 = "Song 4";
  QString w5 = "Song 5";
  QString w6 = "Song 6";
  //QString w7 = "Song 7";
  //QString w8 = "Song 8";

  tabs->addTab(workspace1, w1);
  tabs->addTab(workspace2, w2);
  tabs->addTab(workspace3, w3);
  tabs->addTab(workspace4, w4);
  tabs->addTab(workspace5, w5);
  tabs->addTab(workspace6, w6);
  //tabs->addTab(workspace7, w7);
  //tabs->addTab(workspace8, w8);

  QString one = "one";
  QString two = "two";
  QString three = "three";
  QString four = "four";
  QString five = "five";
  QString six = "six";
  //QString seven = "seven";
  //QString eight = "eight";

  map.insert(w1, one);
  map.insert(w2, two);
  map.insert(w3, three);
  map.insert(w4, four);
  map.insert(w5, five);
  map.insert(w6, six);
  //map.insert(w7, seven);
  //map.insert(w8, eight);

  QFont marginFont("Bariol");
  marginFont.setStyleHint(QFont::SansSerif);
  marginFont.setPointSize(10);

  lexer = new Lexer;

  STYLE_WORKSPACE(workspace1)
  STYLE_WORKSPACE(workspace2)
  STYLE_WORKSPACE(workspace3)
  STYLE_WORKSPACE(workspace4)
  STYLE_WORKSPACE(workspace5)
  STYLE_WORKSPACE(workspace6)
  //STYLE_WORKSPACE(workspace7)
  //STYLE_WORKSPACE(workspace8)

  /*************************************************************
   * Application Asset Declaration and Arrangement             *
   *************************************************************
   * Looks something like this                                 *
   *************************************************************
   *  | Toolbar                                             |  *
   *  -------------------------------------------------------  *
   *   ________  ________  ________             ____________   *
   *  | Song 1 || Song 2 || Song 3 | ...       | Output     |  *
   *  -----------------------------------------|-------------  *
   *  | Main workspace                         |            |  *
   *  |                                        |            |  *
   *  |                                        |            |  *
   *  |                                        |            |  *
   *  |                                        |            |  *
   *  |                                        |            |  *
   *  |                                        |            |  *
   *  |                                        |____________|  *
   *  |                                        |Errors      |  *
   *  |                                        |------------|  *
   *  |                                        |            |  *
   *  |                                        |            |  *
   *  |                                        |            |  *
   *  |                                        |            |  *
   *  -------------------------------------------------------  *
   *************************************************************
   * Setup with QGridLayout elements like this                 *
   *************************************************************
   *  | Toolbar                                             |  *
   *  -------------------------------------------------------  *
   *  _______________________________________________________  *
   * | mainContentLayout                                     | *
   * | _______________________________________ _____________ | *
   * || tabs Widget                           | rhsLayout   || *
   * ||                                       | ___________ || *
   * ||                                       ||outputLabel||| *
   * ||                                       ||-----------||| *
   * ||                                       ||outputPane ||| *
   * ||                                       ||           ||| *
   * ||                                       ||           ||| *
   * ||                                       ||           ||| *
   * ||                                       ||           ||| *
   * ||                                       ||___________||| *
   * ||                                       ||errorLabel ||| *
   * ||                                       ||-----------||| *
   * ||                                       ||errorPane  ||| *
   * ||                                       ||           ||| *
   * ||                                       ||           ||| *
   * ||                                       |-------------|| *
   * |-------------------------------------------------------| *
   * --------------------------------------------------------- *
   *************************************************************
   * mainContentLayout and rhsLayout : QGridLayout   elements  *
   * tabs                            : QTabWidget    element   *
   * outputLabel and errorLabel      : QLabel        elements  *
   * outputPane                      : QTextEdit     element   *
   * errorPane                       : QSciScintilla element   *
   *************************************************************/


/*******
 * RHS *
 *******/

  /* Output */
  QLabel * outputLabel = new QLabel(this);
  outputLabel->setFixedWidth(RHS_WIDTH);
  outputLabel->setText(tr("Output"));
  outputLabel->setStyleSheet(OUTPUT_LABEL_STYLING);

  outputPane = new QTextEdit;
  outputPane->setFixedWidth(RHS_WIDTH);
  outputPane->setStyleSheet(OUTPUT_PANE_STYLING);
  outputPane->setReadOnly(true);
  //outputPane->zoomIn(7);

  /* Error */
  QLabel * errorLabel = new QLabel(tr("Errors"), this);
  errorLabel->setFixedWidth(RHS_WIDTH);
  errorLabel->setStyleSheet(ERROR_LABEL_STYLING);

  errorPane = new QsciScintilla;
  errorPane->setFixedWidth(RHS_WIDTH);
  errorPane->setFixedHeight(ERROR_PANE_HEIGHT);
  errorPane->setStyleSheet(ERROR_PANE_STYLING);
  errorPane->setMarginWidth(1, 0);
  errorPane->setReadOnly(true);
  errorPane->zoomIn(3);

  /* RHS Grid Layout */
  QGridLayout * rhsLayout = new QGridLayout;
  rhsLayout->addWidget(outputLabel, 0, 0);
  rhsLayout->addWidget(outputPane, 1, 0);
  rhsLayout->addWidget(errorLabel, 2, 0);
  rhsLayout->addWidget(errorPane, 3, 0);
  rhsLayout->setVerticalSpacing(0);


/***********************
 * Main Content Layout *
 ***********************/

  QGridLayout * mainContentLayout = new QGridLayout;
  mainContentLayout->addWidget(tabs, 0, 0);
  mainContentLayout->addLayout(rhsLayout, 0, 1);
  mainContentLayout->setVerticalSpacing(0);

  QWidget * mainWidget = new QWidget(this);
  mainWidget->setLayout(mainContentLayout);

  setCentralWidget(mainWidget);

  setStyleSheet(APP_STYLING);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  readSettings();

  // connect(textEdit, SIGNAL(textChanged()),
  //         this, SLOT(documentWasModified()));

  //  setWindowTitle(tr("Sonic Pi"));
  setWindowTitle("Make Music - " + groupName);
  callInitScript();
  loadWorkspaces();

  // connect(runProcess, SIGNAL(readyReadStandardOutput()),
  //         this, SLOT(updateOutput()));

  connect(&app, SIGNAL( aboutToQuit() ), this, SLOT( onExitCleanup() ) );
}



void MainWindow::callInitScript()
{
  QString program = QCoreApplication::applicationDirPath() + "/../../app/scripts/boot.rb";
  QStringList arguments;
  QObject *parent;
  QProcess *myProcess = new QProcess(parent);
  myProcess->start(program, arguments);
  myProcess->waitForFinished();
}


/**************************************************************************
 **************************************************************************
 *********************** Creation of menus/toolbars ***********************
 **************************************************************************
 **************************************************************************/


/**************************************************************************
 * Create the actions and link the signals to the slots                   *
 **************************************************************************/
void MainWindow::createActions()
{
  runAct = new QAction(QIcon(":/images/play.png"), tr("&Run"), this);
  runAct->setShortcut(tr("Ctrl+R"));
  runAct->setStatusTip(tr("Run code"));
  connect(runAct, SIGNAL(triggered()), this, SLOT(runCode()));

  stopAct = new QAction(QIcon(":/images/stop.png"), tr("&Stop"), this);
  stopAct->setShortcut(tr("Ctrl+Q"));
  stopAct->setStatusTip(tr("Stop code"));
  connect(stopAct, SIGNAL(triggered()), this, SLOT(stopCode()));

    // newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    // newAct->setShortcut(tr("Ctrl+N"));
    // newAct->setStatusTip(tr("Create a new file"));
    // connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAsAct = new QAction(QIcon(":/images/save.png"), tr("&Save &As..."), this);
  saveAsAct->setShortcut(tr("Ctrl+S"));
  saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  exitAct = new QAction(QIcon(":/images/close.png"), tr("E&xit"), this);
  exitAct->setShortcut(tr("Alt+F4"));
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    // cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    // cutAct->setShortcut(tr("Ctrl+X"));
    // cutAct->setStatusTip(tr("Cut the current selection's contents to the "
    //                         "clipboard"));
    // connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    // copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    // copyAct->setShortcut(tr("Ctrl+C"));
    // copyAct->setStatusTip(tr("Copy the current selection's contents to the "
    //                          "clipboard"));
    // connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    // pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    // pasteAct->setShortcut(tr("Ctrl+V"));
    // pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
    //                           "selection"));
    // connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // cutAct->setEnabled(false);
    // copyAct->setEnabled(false);
    // connect(textEdit, SIGNAL(copyAvailable(bool)),
    //         cutAct, SLOT(setEnabled(bool)));
    // connect(textEdit, SIGNAL(copyAvailable(bool)),
    //         copyAct, SLOT(setEnabled(bool)));
}

/***********************************************
 * Create the menus and fill them with actions *
 ***********************************************/
void MainWindow::createMenus()
{
    // fileMenu = menuBar()->addMenu(tr("&File"));
    // fileMenu->addAction(newAct);
    // fileMenu->addAction(openAct);
    // fileMenu->addAction(saveAct);
    // fileMenu->addAction(saveAsAct);
    // fileMenu->addSeparator();
    // fileMenu->addAction(exitAct);

    // editMenu = menuBar()->addMenu(tr("&Edit"));
    // editMenu->addAction(cutAct);
    // editMenu->addAction(copyAct);
    // editMenu->addAction(pasteAct);

    // menuBar()->addSeparator();

    // helpMenu = menuBar()->addMenu(tr("&Help"));
    // helpMenu->addAction(aboutAct);
    // helpMenu->addAction(aboutQtAct);f
}

/**********************************************
 * Setup the toolbar and fill it with actions *
 **********************************************/
void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Run"));
    fileToolBar->setMovable(false);
    fileToolBar->setIconSize(QSize(69,30));
    fileToolBar->setStyleSheet(FILE_TOOLBAR_STYLING);
    fileToolBar->addAction(runAct);
    fileToolBar->addAction(stopAct);
    // fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAsAct);

    // Move the close button to the far end of the toolbar
    QWidget * spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible(true);
    fileToolBar->addWidget(spacerWidget);
    fileToolBar->addAction(exitAct);
}

/************************************
 * Create and format the status bar *
 ************************************/
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    /* For now, don't show the status bar */
    statusBar()->hide();
}


/***************************************************************
 ***************************************************************
 *************************** File IO ***************************
 ***************************************************************
 ***************************************************************/


/***************************************************************
 *********************** Creating a file ***********************
 ***************************************************************/
void MainWindow::newFile()
{

}


/*************************************************************
 *********************** Saving a file ***********************
 *************************************************************/

/********************************************
 * Save over a previous save without prompt *
 ********************************************/
bool MainWindow::save()
{
  // TODO Check for a previous save and overwrite it.
  return true;
}

/********************************************************************
 * This is the function which is connected with the signal for save *
 ********************************************************************/
bool MainWindow::saveAs()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Current Workspace"),
                                                  QDir::homePath() + "/Music-content",
                                                  tr("Make Music Projects (*.spi)"));
  if (!fileName.isEmpty())
  {
    if(!fileName.endsWith(".spi"))
      fileName.append(".spi");
    return saveFile(fileName, (QsciScintilla*)tabs->currentWidget());
  } else {
    return false;
  }
}

/***************************************
 * Perform the work of saving the file *
 ***************************************/
bool MainWindow::saveFile(const QString &fileName, QsciScintilla* text)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Make Music"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << text->text();
    QApplication::restoreOverrideCursor();
    //    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}


/**************************************************************
 *********************** Loading a file ***********************
 **************************************************************/

/********************************************************************
 * This is the function which is connected with the signal for open *
 ********************************************************************/
void MainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  QDir::homePath() + "/Music-content",
                                                  tr("Make Music Projects (*.spi)"));
  if (!fileName.isEmpty())
  {
    QsciScintilla * currentWidget = (QsciScintilla *)tabs->currentWidget();
    // Send the filename to loadFile for loading
    loadFile(fileName, currentWidget);
  }
  return;
}

/**************************************
 * Perform the work of loading a file *
 **************************************/
void MainWindow::loadFile(const QString &fileName, QsciScintilla* &text)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Make Music"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    text->setText(in.readAll());
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("File loaded"), 2000);
}


/*************************************************************
 *************************************************************
 *********************** Music Control ***********************
 *************************************************************
 *************************************************************/

void MainWindow::runCode()
{
  //  printf("running code");
  killSynths();
  saveWorkspace( (QsciScintilla*)tabs->currentWidget());
  saveFile("/tmp/sonic-pi-current-code.rb", (QsciScintilla*)tabs->currentWidget());
  outputPane->clear();
  errorPane->clear();
  lexer->setPaper(Qt::lightGray);
  QString emptyText = "";
  statusBar()->showMessage(tr("Running..."), 2000);

  //  clearOutputPanels();

  //  printf((QCoreApplication::applicationDirPath() + "/../../app/scripts/run-code.rb").toAscii().data());
  QString program = QCoreApplication::applicationDirPath() + "/../../app/scripts/run-code.rb";
  QStringList arguments;
  arguments << "/tmp/sonic-pi-current-code.rb";
  QObject *parent;
  runProcess = new QProcess(parent);

  connect(runProcess, SIGNAL(readyReadStandardOutput()),
          this, SLOT(updateOutput()));

  connect(runProcess, SIGNAL(readyReadStandardError()),
          this, SLOT(updateError()));



  runProcess->start(program, arguments);
  //runProcess->write(tabs->currentWidget()->text().toAscii());
  //  runProcess->waitForFinished();
  lexer->setPaper(Qt::white);
}

void MainWindow::killSynths()
{
  if (runProcess)
    {
      runProcess->kill();
    }
  stopRunningSynths();
}

void MainWindow::stopCode()
{
  outputPane->clear();
  errorPane->clear();
  lexer->setPaper(Qt::red);
  statusBar()->showMessage(tr("Stopping..."), 2000);
  killSynths();
  //  clearOutputPanels();
  // QString program = QCoreApplication::applicationDirPath() + "/../../app/scripts/stop-code.rb";
  // QStringList arguments;
  // arguments << "/tmp/sonic-pi";
  // QObject *parent;
  // runProcess = new QProcess(parent);

  // connect(runProcess, SIGNAL(readyReadStandardOutput()),
  //         this, SLOT(updateOutput()));

  // connect(runProcess, SIGNAL(readyReadStandardError()),
  //         this, SLOT(updateError()));

  // runProcess->start(program, arguments);
  //runProcess->write(currentTextArea()->text().toAscii());
  //  runProcess->waitForFinished();
  lexer->setPaper(Qt::white);
}

void MainWindow::stopRunningSynths()
{
  QString program = QCoreApplication::applicationDirPath() + "/../../app/scripts/stop-running-synths.rb";
  QStringList arguments;
  QObject *parent;
  QProcess *myProcess = new QProcess(parent);
  myProcess->start(program, arguments);
  myProcess->waitForFinished();
}

void MainWindow::clearOutputPanels()
{
    outputPane->clear();
    errorPane->clear();
}

void MainWindow::updateError()
{
  QByteArray output = runProcess->readAllStandardError();
  errorPane->append(output);
}

void MainWindow::updateOutput()
{
  QByteArray output = runProcess->readAllStandardOutput();
  outputPane->append(output);
}


/****************************************************
 ****************************************************
 *********************** Exit ***********************
 ****************************************************
 ****************************************************/

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}


/**********************************************************************************
 * Check if the user wants to quit and if so whether they want to save their work *
 **********************************************************************************/
bool MainWindow::maybeSave()
{
    // if (textEdit->isModified()) {
    //     int ret = QMessageBox::warning(this, tr("Application"),
    //                  tr("The document has been modified.\n"
    //                     "Do you want to save your changes?"),
    //                  QMessageBox::Yes | QMessageBox::Default,
    //                  QMessageBox::No,
    //                  QMessageBox::Cancel | QMessageBox::Escape);
    //     if (ret == QMessageBox::Yes)
    //         return save();
    //     else if (ret == QMessageBox::Cancel)
    //         return false;
    // }
    return true;
}


void MainWindow::documentWasModified()
{
  //    setWindowModified(textEdit->isModified());
}

// void MainWindow::textChanged()
// {
//   printf("changed!");
// }

void MainWindow::onExitCleanup()
{
  QString program = QCoreApplication::applicationDirPath() + "/../../app/scripts/shutdown.rb";
  QStringList arguments;
  QObject *parent;
  runProcess = new QProcess(parent);
  runProcess->start(program, arguments);
}


/********************************************************************
 ********************************************************************
 *********************** Workspace operations ***********************
 ********************************************************************
/********************************************************************/

void MainWindow::ensureWorkspaces()
{
  QString program = QCoreApplication::applicationDirPath() + "/../../app/scripts/ensure-workspaces.rb";
  QStringList arguments;
  QObject *parent;
  runProcess = new QProcess(parent);
  runProcess->start(program, arguments);
  runProcess->waitForFinished();
}

void MainWindow::saveWorkspaces()
{
  saveFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/one/1.spi", workspace1);
  saveFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/two/1.spi", workspace2);
  saveFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/three/1.spi", workspace3);
  saveFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/four/1.spi", workspace4);
  saveFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/five/1.spi", workspace5);
  saveFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/six/1.spi", workspace6);
  //saveFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/seven/1.spi", workspace7);
  //saveFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/eight/1.spi", workspace8);
}

void MainWindow::loadWorkspaces()
{
  loadFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/one/1.spi", workspace1);
  loadFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/two/1.spi", workspace2);
  loadFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/three/1.spi", workspace3);
  loadFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/four/1.spi", workspace4);
  loadFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/five/1.spi", workspace5);
  loadFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/six/1.spi", workspace6);
  //loadFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/seven/1.spi", workspace7);
  //loadFile(QDir::homePath() + "/.make-music/workspaces/" + groupName + "/eight/1.spi", workspace8);
}

QString MainWindow::workspaceFilename(QsciScintilla* text)
{
  if(text == workspace1) {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/one/1.spi";}
  else if(text == workspace2) {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/two/1.spi";}
  else if(text == workspace3) {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/three/1.spi";}
  else if(text == workspace4) {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/four/1.spi";}
  else if(text == workspace5) {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/five/1.spi";}
  else if(text == workspace6) {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/six/1.spi";}
  //else if(text == workspace7) {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/seven/1.spi";}
  //else if(text == workspace8) {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/eight/1.spi";}
 else {return QDir::homePath() + "/.make-music/workspaces/" + groupName + "/one/1.spi";}
}

bool MainWindow::saveWorkspace(QsciScintilla* text)
{
  QString label = currentTabLabel();
  return saveFile(workspaceFilename(text), text);
}

// void MainWindow::switchWorkspace(const QString &fileName)
// {
//   //curWorkspace = filename;
// }

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    //    textEdit->setModified(false);
    setWindowModified(false);

}

QString MainWindow::currentTabLabel()
{
  return tabs->tabText(tabs->currentIndex());
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


/*******************************************************************
 *******************************************************************
 *********************** About/Help/Settings ***********************
 *******************************************************************
/*******************************************************************/

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Sonic Pi"),
            tr("<b>Sonic Pi</b> is an experimental language and application"
               "for using creative processes to engage students with a"
               "computer science curriculum."));
}

void MainWindow::readSettings()
{
    QSettings settings("Trolltech", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Trolltech", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

/******************************************************************/
