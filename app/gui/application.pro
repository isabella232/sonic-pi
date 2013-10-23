CONFIG       += release

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

HEADERS       = mainwindow.h \
				lexer.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                lexer.cpp
RESOURCES     = application.qrc
LIBS         += -lqscintilla2
