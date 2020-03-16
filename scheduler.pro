TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS = mainwindow.h \
          scheduler.h \
          ganttwidget.h

SOURCES = main.cpp \
          mainwindow.cpp \
          scheduler.cpp \
          ganttwidget.cpp

FORMS = mainwindow.ui

INCLUDEPATH += $$PWD
         

