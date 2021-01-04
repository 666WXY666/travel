#ifndef TRAVELER_H
#define TRAVELER_H

#include "attribute.h"
#include "schedule.h"

#include <vector>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <cmath>


class Traveler
{
public:
    bool flag;
    int is_continue;//该旅客本次运行是否第一次按开始,0代表新旅客，1代表旧旅客该次没开始，2代表旧旅客该次已开始
    bool is_start;//该旅客是否点击开始
    bool isChecked;//该旅客是否点击途经城市勾选框
    int id, strategy;
    int origin, destination;
    QDateTime start_time, deadline_time, used_time;
    QDateTime total_time;
    std::vector<bool> throughCity;

    Traveler(bool,int, QDateTime,QDateTime, QDateTime, int, int, int, bool, std::vector<bool>);
    std::vector<Attribute> get_plan();//得到旅行计划
    std::vector<Attribute> change_plan(int city, int strategy, int destination, QDateTime deadline_time,
                                      bool isChecked, std::vector<bool> throughCity);//改变旅行计划
    QDateTime get_city_arrival_dateTime(int);//到达城市时间
    QDateTime get_city_departure_datetime(int);//离开城市时间

    std::vector<Attribute> plan; //记录最优路径
    std::vector<QDateTime> time; //记录每个城市的到达时间
private:
    std::vector<Attribute> dijkstra(std::vector<bool> &, std::vector<QDateTime> &);//迪杰斯特拉算法求策略0 1的最优路径
    void deep_first(int city, std::vector<Attribute>&, std::vector<bool>&,std::vector<QDateTime>&, std::vector<int> &);//递归查找路径
    void make_plan(int, const std::vector<Attribute>&, std::vector<Attribute> &);//生成旅行计划
    void update_adjacent(int, std::vector<int>&, std::vector<bool>&, std::vector<Attribute>&); //更新相邻节点的值
    QDateTime calculate_time(const std::multimap<int, Attribute>::iterator&,std::vector<QDateTime>&);//计算时间
    QDateTime total_datetime();//总用时

    int min;//最小花费
    QDateTime min_time;//最短时间
};

#endif // TRAVELER_H
