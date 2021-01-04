#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include "widget.h"

#include <QWidget>
#include <QPen>
#include <QDebug>
#include <QRectF>
#include <QDateTime>
#include <QPointF>
#include <QMessageBox>
#include <QTextEdit>
#include <QBrush>
#include <QPalette>
#include <QPixmap>

#include <QTimer>
#include <QThread>

class MapWidget : public QWidget
{
public:
    MapWidget(QWidget *);

    int next_city();//获得新计划的始发地

    QPixmap set_ico();//设置图标
    QPointF set_pos();//设置图标位置

    QDateTime get_time_differ(QDateTime shorter, QDateTime longer);//获取两时间点时间间隔
    QPointF get_city_pos(int city);//获得城市坐标
    QPointF get_distance(QDateTime spent_time, QDateTime start_next_begin, QDateTime start_next_End,
                            int from, int to);//获得中间坐标
    double get_time_difference(QDateTime shorter, QDateTime longer);//获得两时间间隔时间差

private slots:
    void update();//刷新画面

private:
    int state;
    QTimer * left_timer;

protected:
    void paintEvent(QPaintEvent *);
};

#endif // MAPWIDGET_H
