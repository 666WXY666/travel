#include "mapwidget.h"

#include <QApplication>
#include <QStateMachine>
#include <QPushButton>
#include <QSignalTransition>
#include <QPropertyAnimation>
#include <QPainter>
#include <QState>
#include <QLabel>

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent), state(-1)
{
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Background, QBrush(QPixmap("://resource/map.png").scaled(941,698,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    this->setPalette(palette);

    left_timer = new QTimer;
    left_timer->start(1000/60);
    connect(left_timer, SIGNAL(timeout()), this, SLOT(update()));
}

//重写绘图函数，实现图标移动和画线
void MapWidget::paintEvent(QPaintEvent *)
{
    QPointF ico_position;
    QPen pen;
    QPainter painter(this);
    Widget *parent_ptr = dynamic_cast<Widget*>(parentWidget());
    if (parent_ptr->current_traveler != -1)
    {
        std::vector<Attribute>path=parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].get_plan();
        painter.drawPixmap((set_pos()), set_ico());
        pen.setWidth(3);
        pen.setStyle(Qt::DotLine);
        painter.setPen(pen);
        ico_position=set_pos();
        if(path.size()!=0)
        {
            if(get_city_pos(path[0].from)!=ico_position)
            {
                parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].flag=true;
            }
        }

        for (unsigned i=0;i<path.size();i++)
        {
            painter.drawLine(get_city_pos(path[unsigned(i)].from)+QPointF(15,15),get_city_pos(path[unsigned(i)].to)+QPointF(15,15));
        }
    }
}

//根据当前状态、交通方式选择图标
QPixmap MapWidget::set_ico()
{
    QPixmap pointGraph;
    switch(state)
    {
    case -2:
        pointGraph = QPixmap("://resource/arrive.png").scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        break;
    case -1:
        pointGraph = QPixmap("://resource/pause.png").scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        break;
    case 0:
        pointGraph = QPixmap("://resource/car.png").scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        break;
    case 1:
        pointGraph = QPixmap("://resource/train.png").scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        break;
    case 2:
        pointGraph = QPixmap("://resource/plane.png").scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        break;
    }

    return pointGraph;
}

//设置当前图标所处位置
QPointF MapWidget::set_pos()
{
    Widget *parent_ptr = dynamic_cast<Widget*>(parentWidget());
    static QPointF position;
    std::vector<Attribute> path = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].get_plan();
    QDateTime spent_time = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].used_time;
    QDateTime start_time = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].start_time;

    //已用时间不小于总时间，当前位置为目的地
    if(spent_time >= parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].total_time)
    {
         if(spent_time == parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].total_time)
         {
              qDebug() << "到达终点：" << path[path.size()-1].to;
         }
         position = get_city_pos(path[path.size()-1].to);
         state = -2;
    }
    else
    {
        for (std::vector<Attribute>::size_type index = 0;index != path.size(); index++)
        {
            QDateTime departure_datetime = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].get_city_departure_datetime(path[index].from);
            QDateTime city_arrive_time = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].get_city_arrival_dateTime(path[index].to);
            QDateTime start_next_begin = get_time_differ(start_time, departure_datetime);
            QDateTime start_next_end = get_time_differ(start_time, city_arrive_time);
            //等待状态
            if (spent_time <= start_next_begin)
            {
                position = get_city_pos(path[index].from);
                state = -1;
                break;
            }
            //运行中状态
            else if (spent_time <= start_next_end)
            {
                position = get_city_pos(path[index].from);
                position += get_distance(spent_time, start_next_begin, start_next_end, path[index].from, path[index].to);
                state = path[index].vehicle;
                if (spent_time == start_next_end)
                {
                    qDebug() << "到达：" << path[index].to;
                }
                break;
            }
        }
    }

    return position;
}

//中途更改计划，获得新计划的始发地
int MapWidget::next_city()
{
    Widget *parent_ptr = dynamic_cast<Widget*>(parentWidget());
    std::vector<Attribute> path = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].get_plan();
    int nextcity_arrive=-1;
    QDateTime spent_time = parent_ptr->get_spend_time();

    //已经到达目的地
    if(spent_time >= parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].total_time)
    {
         nextcity_arrive = -1;
         state = -2;
    }
    else
        for (std::vector<Attribute>::size_type index = 0;
            index != path.size(); index++)
        {
            QDateTime start_time = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].start_time;
            QDateTime departuredatetime = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].get_city_departure_datetime(path[index].from);
            QDateTime cityarrivaltime = parent_ptr->travelers[unsigned(parent_ptr->current_traveler)].get_city_arrival_dateTime(path[index].to);
            //等待状态，新计划始发地为当前所处地点
            if (spent_time <= get_time_differ(start_time, departuredatetime))
            {
                nextcity_arrive = path[index].from;
                state = -1;
                break;
            }
            //运行状态，新计划为即将到达的城市
            else if (spent_time <=get_time_differ(start_time, cityarrivaltime))
            {
                nextcity_arrive = path[index].to;
                state = path[index].vehicle;
                break;
            }
        }
    qDebug() << "下一个要到达的城市为：" << nextcity_arrive;
    return nextcity_arrive;
}

//返回两时间点之间的时间差
QDateTime MapWidget::get_time_differ(QDateTime shorter, QDateTime longer)
{
    int sec = (longer.time().second() - shorter.time().second());
    int min = longer.time().minute() - shorter.time().minute();
    if(sec < 0)
    {
        min--;
    }
    int hour = longer.time().hour() - shorter.time().hour();
    if(min < 0)
    {
        hour--;
    }
    int day = longer.date().day() - shorter.date().day()+ shorter.date().daysInMonth();
    if(hour < 0)
    {
        day--;
    }

    sec = (sec + 60) % 60;
    min = (min + 60) % 60;
    hour = (hour + 24) % 24;
    day=day%shorter.date().daysInMonth();

    return QDateTime(QDate(1, 1, day+1), QTime(hour, min, sec, 999));
}

//返回两个时间段的时间差
double MapWidget::get_time_difference(QDateTime shorter, QDateTime longer)
{
    int sec = (longer.time().second() - shorter.time().second());
    int min = longer.time().minute() - shorter.time().minute();
    if(sec < 0)
    {
        min--;
    }
    int hour = longer.time().hour() - shorter.time().hour();
    if(min < 0)
    {
        hour--;
    }
    int day = longer.date().day() - shorter.date().day()+ shorter.date().daysInMonth();
    if(hour < 0)
    {
        day--;
    }

    sec = (sec + 60) % 60;
    min = (min + 60) % 60;
    hour = (hour + 24) % 24;
    day=day%shorter.date().daysInMonth();

    return static_cast<double>(day * 86400 + hour * 3600 + min * 60 + sec);
}

//计算坐标增量
QPointF MapWidget::get_distance(QDateTime spent_time, QDateTime start_next_begin, QDateTime start_next_End,int from, int to)
{
    double rate = get_time_difference(start_next_begin, spent_time)/get_time_difference(start_next_begin, start_next_End);
    double x_increase = (get_city_pos(to) - get_city_pos(from)).x() * rate;
    double y_increase = (get_city_pos(to) - get_city_pos(from)).y() * rate;

    return QPointF(x_increase, y_increase);
}

//获得图形中各个城市的位置
QPointF MapWidget::get_city_pos(int city)
{
    int x=0, y=0;
    switch (city)
    {
    case 0:
        x=716;
        y=276;
        break;
    case 1:
        x=859;
        y=150;
        break;
    case 2:
        x=829;
        y=443;
        break;
    case 3:
        x=554;
        y=499;
        break;
    case 4:
        x=734;
        y=342;
        break;
    case 5:
        x=226;
        y=205;
        break;
    case 6:
        x=282;
        y=493;
        break;
    case 7:
        x=710;
        y=621;
        break;
    case 8:
        x=846;
        y=565;
        break;
    case 9:
        x=682;
        y=388;
        break;
    }
    return QPointF(x-25,y-25);
}
