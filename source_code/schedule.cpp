#include "schedule.h"
#include "log.h"
std::multimap<int, Attribute> Schedule::database;

Schedule::Schedule()
{
    QFile file(":/database.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "打开时刻表文件失败！";
        return;
    }

    QTextStream in(&file);
    QString from, to, number, begin, end, cost, vechile; //分别为出发地、目的地、班次、开始时间、结束时间、费用、交通方式

    //将时刻表信息读入database数据结构
    while(!in.atEnd())
    {
        in >> from >> to >> number >> begin >> end >> cost >> vechile >> endl;
        if(from != "")
        {
            qDebug() << from << to << number << begin<<end << cost << vechile;
            Attribute line(cityname_to_citynum(from), cityname_to_citynum(to), number, QTime::fromString(begin),
                           QTime::fromString(end), cost.toInt(), vechile.toInt());
            database.insert(std::make_pair(cityname_to_citynum(from), line));
        }
    }
    qDebug() << "导入时刻表成功";
    file.close();
}

//根据字符串转换为相应的序号
int Schedule::cityname_to_citynum(QString city)
{
    int num = -1;

    if(city.toUtf8() == "北京")
        num = 0;
    else if(city.toUtf8() == "哈尔滨")
        num = 1;
    else if(city.toUtf8() == "上海")
        num = 2;
    else if(city.toUtf8() == "重庆")
        num = 3;
    else if(city.toUtf8() == "济南")
        num = 4;
    else if(city.toUtf8() == "乌鲁木齐")
        num = 5;
    else if(city.toUtf8() == "拉萨")
        num = 6;
    else if(city.toUtf8() == "香港")
        num = 7;
    else if(city.toUtf8() == "台北")
        num = 8;
    else if(city.toUtf8() == "郑州")
        num = 9;
    return num;
}
