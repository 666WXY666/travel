#ifndef WIDGET_H
#define WIDGET_H

#include "schedule.h"
#include "traveler.h"
#include "mapwidget.h"
#include <QWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QEvent>
#include <QPixmap>
#include <QScrollArea>
#include <QPalette>
#include <QLayout>
#include <QPalette>
#include <QCloseEvent>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QString>
#include <QMessageBox>
#include <QPushButton>
#include <QDate>
#include <QTime>
#include <QDateTimeEdit>
#include <QTimer>
#include <QThread>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    int current_traveler;

    Schedule schedule;
    std::vector<Traveler> travelers;

    QDateTime get_start_time();//获取开始时间
    QDateTime get_spend_time();//获取已用时间
    QDateTime get_dead_time();//获取截止时间

private:
    Ui::Widget *ui;

    std::vector<bool> start_clicked;//开始按钮第一次按下
    std::vector<bool> throughcity;//途经城市是否选定

    int strategy;
    int start;
    int destination;
    int add_traveler_times;//添加旅客次数，即旅客编号最大值
    int start_clicked_times;//“开始”按钮点击次数，0次表示首次运行，1表示其他

    QTimer *right_timer;
    QThread *right_timer_thread;

    void init_UI();
    void init_connect();
    void init_time_thread();

    void display_money(std::vector<Attribute> path);//显示方案所需经费
    void display_path(std::vector<Attribute> path);//显示路径
    void display_total_time();//显示方案所需总时间

    void read_traveler();
    void write_traveler();

    int get_strategy();//获取用户所选策略
    int get_start();//获取用户所选始发地
    int get_destination();//获取用户所选目的地

    QString citynum_to_cityname(int index);//将城市编号转为城市名称

private slots:
    void right_timer_start();//开始计时器

    void active_throughcity();//得到途经城市
    //设置各个城市是否被选为途经城市
    void set_throungcity0();
    void set_throungcity1();
    void set_throungcity2();
    void set_throungcity3();
    void set_throungcity4();
    void set_throungcity5();
    void set_throungcity6();
    void set_throungcity7();
    void set_throungcity8();
    void set_throungcity9();

    void startbutton_clicked();//开始按钮按下，开始计算路径图形输出
    void addtravelerbutton_clicked();//增加旅客按钮按下
    void traveler_changed();//切换旅客时更改界面显示
    void display_spent_time();//显示已经花费的时间
    void set_deadtime_edit(int current_strategy);//根据所选策略更改截止时间栏状态

signals:
    void start_timer();//开始计时的信号

protected:
    void closeEvent(QCloseEvent *);
};

#endif // WIDGET_H
