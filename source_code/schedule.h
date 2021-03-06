﻿#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "attribute.h"

#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <QString>
#include <QObject>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class Schedule
{
public:
    Schedule();
    static int cityname_to_citynum(QString city);
    static std::multimap<int, Attribute> database;

};

#endif // SCHEDULE_H
