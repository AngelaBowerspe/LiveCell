#ifndef CREATEEXPERIMENTSETTINGS_H
#define CREATEEXPERIMENTSETTINGS_H

#include <QString>

struct CreateExperimentSettings
{
    QString experimentName;
    QString experimentNumber;
    QString experimentDate;
    QString experimentType;
    QString scanChannel;
    QString objective;
    QString scanType;
    QString zStack;
    QString intervalTime;
    QString loopCount;
    QString fieldStitching;
    QString fieldOverlap;
    QString focusInterval;
    QString focusChannel;
    QString plateType;
    QString selectedWells;
    QString selectedGroups;
    QString selectedFields;
};

#endif // CREATEEXPERIMENTSETTINGS_H
