#ifndef MAINWINDOW_CONTEXT_H
#define MAINWINDOW_CONTEXT_H

#include <QString>
#include "settingwin.h"

class LogWin;
class A2L_Project;

struct MainWindowContext
{
    Proj *project = nullptr;
    A2L_Project *currentA2lProject = nullptr;
    LogWin *logWin = nullptr;

    QString *mdfFileName = nullptr;
    bool *recordOn = nullptr;
    int *recordRateMs = nullptr;
};

#endif // MAINWINDOW_CONTEXT_H
