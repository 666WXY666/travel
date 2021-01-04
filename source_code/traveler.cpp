#include "traveler.h"
#include "log.h"

Traveler::Traveler(bool is_start,int id,QDateTime used_time, QDateTime start_time, QDateTime deadline_time, int strategy, int origin,
                   int destination, bool isChecked, std::vector<bool> throughCity) :
    is_continue(0),is_start(is_start),isChecked(isChecked), id(id), strategy(strategy), origin(origin), destination(destination),
    start_time(start_time), deadline_time(deadline_time), used_time(used_time),
    throughCity(throughCity), time(10, QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59))),
    min(0x7FFFFFFF), min_time(QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59)))
{
    flag=false;
    std::vector<bool> known(10, false);  //标记每个点是否被访问过
    //根据策略选择算法
    if(strategy !=1 || isChecked)
    {
        std::vector<QDateTime> tempTime(10, QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59)));//临时记录每个点的到达时间
        std::vector<int> tempValue(10);      //临时记录
        std::vector<Attribute> path;         //记录每个点的移动路径
        tempTime[unsigned(origin)] = start_time;

        deep_first(origin, path, known, tempTime, tempValue);
        qDebug()<<"使用深度优先搜索";
    }
    else
    {
        plan = dijkstra(known, time);
        qDebug()<<"使用dijkstra";
    }
    total_time = total_datetime();//计算行程总时间
}

//根据行程起始时间和终止时间计算总时间
QDateTime Traveler::total_datetime()
{
    QDateTime end_time = time[unsigned(destination)];

    int min = end_time.time().minute() - start_time.time().minute();
    int hour = end_time.time().hour() - start_time.time().hour();
    if(min < 0)
    {
        hour--;
    }
    int day = end_time.date().day() - start_time.date().day()+ start_time.date().daysInMonth();
    if(hour < 0)
    {
        day--;
    }

    min = (min + 60) % 60;
    hour = (hour + 24) % 24;
    day=day% start_time.date().daysInMonth();
    return QDateTime(QDate(1, 1, day+1), QTime(hour, min, 0));
}

//获得计划
std::vector<Attribute> Traveler::get_plan()
{
    return this->plan;
}

//改变计划
std::vector<Attribute> Traveler::change_plan(int city, int strategy, int destination, QDateTime deadline_time,bool isChecked, std::vector<bool> throughCity)
{
    min = 0x7FFFFFFF;
    min_time = QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59));

    //对旅客信息进行更改
    qDebug() << "更改旅行计划：" << city << strategy << destination << deadline_time;
    std::vector<bool> known(10, false);  //标记每个点是否被访问过
    std::vector<Attribute> oldPlan = plan;
    std::vector<Attribute> tempPlan = plan;
    int oldOrigin = origin;
    QDateTime oldstart_time = start_time;
    origin = -1;

    //将原来的旅行计划中未走的路径删除
    for(std::vector<Attribute>::iterator iter = oldPlan.begin(); iter != oldPlan.end(); iter++)
    {
        if (iter->from == city)
        {
            {
                origin = iter->from;
                if (iter == oldPlan.begin())
                {
                    int Min = used_time.time().minute() + start_time.time().minute();
                    int Hour = used_time.time().hour() + start_time.time().hour() + Min / 60;
                    int Day = used_time.date().day()-1 + start_time.date().day() + Hour / 24;
                    int Month = used_time.date().month()-1 + start_time.date().month() + Day / start_time.date().daysInMonth();
                    Min %= 60;
                    Hour %= 24;
                    Day %= start_time.date().daysInMonth();
                    start_time = QDateTime(QDate(start_time.date().year(), Month, Day), QTime(Hour, Min, start_time.time().second()));
                }
                else
                    start_time = time[unsigned(iter->from)];
                oldPlan.erase(iter, oldPlan.end());
            }
            break;
        }
        known[unsigned(iter->from)] = true;
        throughCity[unsigned(iter->from)] = false;
    }
    //如果原计划未变化，即为终点城市，说明在最后一条路径，此时不能改变计划
    if (origin == -1)
    {
        std::vector<Attribute> nullPlan; //其实就是得到一个空路径
        plan = tempPlan;
        origin = oldOrigin;
        total_time = total_datetime();
        return nullPlan;
    }
    plan.clear();
    this->strategy = strategy;
    this->destination = destination;
    this->deadline_time = deadline_time;
    this->isChecked = isChecked;
    this->throughCity = throughCity;

    //任何策略都是用深度优先改变计划
    std::vector<QDateTime> tempTime(10, QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59)));
    std::vector<int> tempValue(10);
    std::vector<Attribute> path;     //记录每个点的移动路径
    for (std::vector<Attribute>::iterator iter = oldPlan.begin(); iter != oldPlan.end(); iter++)
    {
        tempTime[unsigned(iter->from)] = time[unsigned(iter->from)];
    }
    tempTime[unsigned(origin)] = start_time;
    deep_first(origin, path, known, tempTime, tempValue);

    //新旧计划组合
    oldPlan.insert(oldPlan.end(), plan.begin(), plan.end());
    start_time = oldstart_time;
    //判断新计划为空且不为目的地城市，则说明无有效路径
    if (plan.size() == 0 && city != destination)
    {
        std::vector<Attribute> nullPlan; //得到一个空路径
        plan = tempPlan;
        origin = oldOrigin;
        total_time = total_datetime();
        return nullPlan;
    }
    else 
    {
        plan = oldPlan;
        origin = oldOrigin;
        total_time = total_datetime();
        return plan;
    }

}

//获得城市的到达时间
QDateTime Traveler::get_city_arrival_dateTime(int index)
{
    return time[unsigned(index)];
}

//获得城市的离开时间
QDateTime Traveler::get_city_departure_datetime(int index)
{
    std::vector<Attribute>::size_type id = 0;
    QTime tempTime;

    for(; id != plan.size(); id++)
        if(plan[id].from == index)
        {
            tempTime = plan[id].begin; //获得当前城市的出发时间
            break;
        }

    QDateTime DepartureDateTime = time[unsigned(index)];
    if(DepartureDateTime.time() <= tempTime)
        return QDateTime(DepartureDateTime.date(), tempTime);
    else
        return QDateTime(DepartureDateTime.date().addDays(1), tempTime);
}

//递归求路径
void Traveler::deep_first(int city, std::vector<Attribute>& path, std::vector<bool>& known,
                   std::vector<QDateTime>& tempTime, std::vector<int>& tempValue)
{
    //分别各个策略进行剪枝
    if (strategy == 2 && (tempTime[unsigned(city)] > deadline_time || tempValue[unsigned(city)] > min)) //总时间大于截至时间或者花费大于已有路径花费，不满足约束条件
        return;
    if (strategy == 1 && tempTime[unsigned(city)] > min_time)
        return;
    if (strategy == 0 && tempValue[unsigned(city)] > min)
        return;

    known[unsigned(city)] = true; //标记此城市已访问过

    //已经递归到目的地城市
    if (city == destination)
    {
        int ok = true;
        std::vector<bool> mark = throughCity;
        //将路径上的所有城市取消标志
        for (std::vector<Attribute>::size_type ix = 0; ix != path.size(); ix++)
                mark[unsigned(path[ix].to)] = false;
        mark[unsigned(origin)] = false;

        //必经城市还有点未取消标志
        if (isChecked)
        {
            for (std::vector<bool>::size_type ix = 0; ix != mark.size(); ix++)
            {
                if (mark[ix] == true)
                {
                    ok = false;
                    break;
                }
            }
        }
        //若满足约束条件，则更新最小值并记录路径
        if(strategy != 1 && tempValue[unsigned(city)] < min && ok)
        {
            min = tempValue[unsigned(city)];
            time = tempTime;
            plan = path;
        }
        else if (strategy == 1 && tempTime[unsigned(city)] < min_time && ok)
        {
            min_time = tempTime[unsigned(city)];
            time = tempTime;
            plan = path;
        }
    }
    //未到达目的地城市，继续向下一层递归
    else
    {
        typedef std::multimap<int, Attribute>::size_type sz_type;
        sz_type entries = Schedule::database.count(city);

        std::multimap<int, Attribute>::iterator iter = Schedule::database.find(city);
        std::multimap<int, Attribute>::iterator min = iter;
        bool start = true;
        for(sz_type cnt = 0; cnt != entries; cnt++, iter++)
        {
            if (known[unsigned(iter->second.to)] == true) //如果去往城市已经访问过，则忽略该路径
            {
                if (start)
                    min++;
                continue;
            }
            start = false;
            if (strategy != 2)
            {
                //找到一条通往新的城市的路径
                if (iter->second.to != min->second.to)
                {
                    if (!known[unsigned(min->second.to)])
                    {
                        path.push_back(min->second);

                        tempTime[unsigned(min->second.to)] = calculate_time(min, tempTime);
                        tempValue[unsigned(min->second.to)] = tempValue[unsigned(city)] + min->second.cost;

                        deep_first(min->second.to, path, known, tempTime, tempValue);

                        //回溯状态
                        known[unsigned(min->second.to)] = false;
                        path.erase(path.end());

                        //更新min
                        min = iter;
                    }
                }
                //此路径的去往城市与之前相同，则寻找最小值
                else
                {
                    if (strategy == 0 && iter->second.cost < min->second.cost)
                        min = iter;
                    if (strategy == 1 && calculate_time(iter, tempTime) < calculate_time(min, tempTime))
                        min = iter;
                }
            }
            //策略三不能局部最优剪枝，所以每条路径都需要尝试
            else
            {
                path.push_back(iter->second);

                //更新时间和花费
                tempTime[unsigned(iter->second.to)] = calculate_time(iter, tempTime);
                tempValue[unsigned(iter->second.to)] = tempValue[unsigned(city)] + iter->second.cost;

                deep_first(iter->second.to, path, known, tempTime, tempValue);

                known[unsigned(iter->second.to)] = false;
                path.erase(path.end());
            }
        }
        //循环结束后还需要处理一次min
        if (strategy != 2)
        {
            if (!known[unsigned(min->second.to)])
            {
                path.push_back(min->second);

                tempTime[unsigned(min->second.to)] = calculate_time(min, tempTime);
                tempValue[unsigned(min->second.to)] = tempValue[unsigned(city)] + min->second.cost;

                deep_first(min->second.to, path, known, tempTime, tempValue);

                known[unsigned(min->second.to)] = false;
                path.erase(path.end());
            }
        }
    }
}

QDateTime Traveler::calculate_time(const std::multimap<int, Attribute>::iterator& iter,
                                  std::vector<QDateTime>& tempTime) //计算时间
{
    QDateTime temp;
    //判断是否时间跨天
    bool span;
    if(iter->second.begin <= iter->second.end)
        span = false;
    else
        span = true;
    if(!span && tempTime[unsigned(iter->second.from)].time() <= iter->second.begin)
        temp = QDateTime(tempTime[unsigned(iter->second.from)].date(), iter->second.end);
    else if(!span && tempTime[unsigned(iter->second.from)].time() > iter->second.begin)
        temp = QDateTime(tempTime[unsigned(iter->second.from)].date().addDays(1), iter->second.end);
    else if(span && tempTime[unsigned(iter->second.from)].time() <= iter->second.begin)
        temp = QDateTime(tempTime[unsigned(iter->second.from)].date().addDays(1), iter->second.end);
    else if(span && tempTime[unsigned(iter->second.from)].time() > iter->second.begin)
        temp = QDateTime(tempTime[unsigned(iter->second.from)].date().addDays(2), iter->second.end);
    return temp;
}

std::vector<Attribute> Traveler::dijkstra(std::vector<bool> &known, std::vector<QDateTime>& time)
{
    std::vector<int> value(10, INT_MAX); //记录原点到每个点的权值之和
    std::vector<Attribute> path(10);     //记录每个点的移动路径

    QDateTime currentTime = start_time;
    time[unsigned(origin)] = currentTime;
    known[unsigned(origin)] = true;
    value[unsigned(origin)] = 0;
    int city = origin;
    while(1)
    {
        //更新相邻节点的值
        update_adjacent(city, value, known, path);
        city = -1;

        if(strategy == 0)
        {
            int min = INT_MAX;
            //寻找未访问点中最小的权值
            for(std::vector<int>::size_type ix = 0;
                ix != value.size(); ix++)
            {
                if(!known[ix] && min > value[ix])
                {
                    min = value[ix];
                    city = int(ix);
                }
            }
        }
        else if(strategy == 1)
        {
            QDateTime minn(QDate(7999, 12, 31), QTime(23, 59, 59));

            for(std::vector<QDateTime>::size_type ix = 0;
                ix != time.size(); ix++)
            {
                if(!known[ix] && minn > time[ix])
                {
                    minn = time[ix];
                    city = int(ix);
                }
            }
        }

        if(city == -1)
            break;

        known[unsigned(city)] = true;
    }
    std::vector<Attribute> plan;
    make_plan(destination, path, plan);
    qDebug() << "旅行计划计算完成！";
    return plan;
}

//逆向生成计划
void Traveler::make_plan(int city, const std::vector<Attribute>& path, std::vector<Attribute> &plan)
{
    if(path[unsigned(city)].from == -1)
        return;
    if(city != origin)
    {
        make_plan(path[unsigned(city)].from, path, plan);
        plan.push_back(path[unsigned(city)]);
        qDebug() << path[unsigned(city)].from << path[unsigned(city)].num << path[unsigned(city)].to;
    }
}

void Traveler::update_adjacent(int city, std::vector<int>& value, std::vector<bool>& known, std::vector<Attribute>& path)
{
    typedef std::multimap<int, Attribute>::size_type sz_type;
    sz_type entries = Schedule::database.count(city);

    std::multimap<int, Attribute>::iterator iter = Schedule::database.find(city);
    for(sz_type cnt = 0; cnt != entries; cnt++, iter++)
    {
        //判断是否跨天
        bool span;
        if(iter->second.begin <= iter->second.end)
            span = false;
        else
            span = true;

        if(strategy == 0)
        {
            //策略一:花费最少
            if(!known[unsigned(iter->second.to)] && value[unsigned(iter->second.to)] > value[unsigned(city)] + iter->second.cost)
            {
                value[unsigned(iter->second.to)] = value[unsigned(city)] + iter->second.cost;
                path[unsigned(iter->second.to)] = iter->second;
                //更新
                if(!span && time[unsigned(iter->second.from)].time() <= iter->second.begin)
                    time[unsigned(iter->second.to)] = QDateTime(time[unsigned(iter->second.from)].date(), iter->second.end);
                else if(!span && time[unsigned(iter->second.from)].time() > iter->second.begin)
                    time[unsigned(iter->second.to)] = QDateTime(time[unsigned(iter->second.from)].date().addDays(1), iter->second.end);
                else if(span && time[unsigned(iter->second.from)].time() <= iter->second.begin)
                    time[unsigned(iter->second.to)] = QDateTime(time[unsigned(iter->second.from)].date().addDays(1), iter->second.end);
                else if(span && time[unsigned(iter->second.from)].time() > iter->second.begin)
                    time[unsigned(iter->second.to)] = QDateTime(time[unsigned(iter->second.from)].date().addDays(2), iter->second.end);
            }
        }
        else if(strategy == 1)
        {
            //策略二:时间最短
            if(!known[unsigned(iter->second.to)])
            {
                if(!span && time[unsigned(iter->second.from)].time() <= iter->second.begin &&
                        time[unsigned(iter->second.to)] > QDateTime(time[unsigned(iter->second.from)].date(), iter->second.end))
                {
                    time[unsigned(iter->second.to)] = QDateTime(time[unsigned(iter->second.from)].date(), iter->second.end);
                    path[unsigned(iter->second.to)] = iter->second;
                }
                else if(!span && time[unsigned(iter->second.from)].time() > iter->second.begin &&
                        time[unsigned(iter->second.to)] > QDateTime(time[unsigned(iter->second.from)].addDays(1).date(), iter->second.end))
                {
                    time[unsigned(iter->second.to)] = QDateTime(time[unsigned(iter->second.from)].addDays(1).date(), iter->second.end);
                    path[unsigned(iter->second.to)] = iter->second;
                }
                else if(span && time[unsigned(iter->second.from)].time() <= iter->second.begin &&
                        time[unsigned(iter->second.to)] > QDateTime(time[unsigned(iter->second.from)].addDays(1).date(), iter->second.end))
                {
                    time[unsigned(iter->second.to)] = QDateTime(time[unsigned(iter->second.from)].addDays(1).date(), iter->second.end);
                    path[unsigned(iter->second.to)] = iter->second;
                }
                else if(span && time[unsigned(iter->second.from)].time() > iter->second.begin &&
                        time[unsigned(iter->second.to)] > QDateTime(time[unsigned(iter->second.from)].addDays(2).date(), iter->second.end))
                {
                    time[unsigned(iter->second.to)] = QDateTime(time[unsigned(iter->second.from)].addDays(2).date(), iter->second.end);
                    path[unsigned(iter->second.to)] = iter->second;
                }
            }
        }
    }
}

