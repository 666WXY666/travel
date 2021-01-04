#include "widget.h"
#include "ui_widget.h"
#include "log.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),current_traveler(-1), ui(new Ui::Widget),
    throughcity(10, false),add_traveler_times(0),start_clicked_times(0)
{
    ui->setupUi(this);
    int button1;
    button1 = QMessageBox::question(this, tr("提示"), QString(tr("是否恢复上一次保留的旅客信息?")), QMessageBox::Yes | QMessageBox::No);
    if (button1 == QMessageBox::Yes)
    {
        read_traveler();//读入文件中保存的旅客信息
    }
    init_UI();
    init_time_thread();
    init_connect();
}

//初始化图形界面
void Widget::init_UI()
{
    qDebug() << "UI初始化成功";
    this->setFixedSize(1280, 720);
    this->setWindowTitle("旅行模拟系统");
    QIcon exeIcon(":/exe.ico");
    this->setWindowIcon(exeIcon);
    if(travelers.size()==0)
    {
        qDebug()<<"没有原来的旅客信息";
        ui->StartDateTimeEdit->setDateTime(QDateTime::currentDateTime());
        ui->DeadlineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
        ui->StrategyComboBox->setEnabled(false);
        ui->StartComboBox->setEnabled(false);
        ui->DestinationComboBox->setEnabled(false);
        ui->DestinationComboBox->setCurrentIndex(1);
        ui->TravelerComboBox->setEnabled(false);
        ui->ThroughCityCheckBox->setEnabled(false);
        ui->city0cbox->setEnabled(false);
        ui->city1cbox->setEnabled(false);
        ui->city2cbox->setEnabled(false);
        ui->city3cbox->setEnabled(false);
        ui->city4cbox->setEnabled(false);
        ui->city5cbox->setEnabled(false);
        ui->city6cbox->setEnabled(false);
        ui->city7cbox->setEnabled(false);
        ui->city8cbox->setEnabled(false);
        ui->city9cbox->setEnabled(false);
        ui->StartDateTimeEdit->setEnabled(false);
        ui->DeadlineDateTimeEdit->setEnabled(false);
        ui->StartButton->setEnabled(false);
    }
    else
    {
        for (unsigned i=0;i<travelers.size();i++)
        {
            qDebug()<<"有保存的旅客信息";
            if(travelers[i].is_start==true)
            {
                start_clicked.push_back(false);
            }
            else
            {
                start_clicked.push_back(false);
            }
            travelers[i].is_continue=1;
            add_traveler_times += 1;
            start_clicked_times = 0;
            ui->TravelerComboBox->addItem(QString::number(add_traveler_times));
            ui->TravelerComboBox->setCurrentText(QString::number(add_traveler_times));
            traveler_changed();
            display_spent_time();
            ui->StartButton->setText(QString::fromWCharArray(L"开始"));

            if(travelers[i].is_start==true)
            {
                ui->StrategyComboBox->setEnabled(false);
                ui->StartComboBox->setEnabled(false);
                ui->DestinationComboBox->setEnabled(false);
                ui->ThroughCityCheckBox->setEnabled(false);
                ui->city0cbox->setEnabled(false);
                ui->city1cbox->setEnabled(false);
                ui->city2cbox->setEnabled(false);
                ui->city3cbox->setEnabled(false);
                ui->city4cbox->setEnabled(false);
                ui->city5cbox->setEnabled(false);
                ui->city6cbox->setEnabled(false);
                ui->city7cbox->setEnabled(false);
                ui->city8cbox->setEnabled(false);
                ui->city9cbox->setEnabled(false);
                ui->StartDateTimeEdit->setEnabled(false);
                ui->DeadlineDateTimeEdit->setEnabled(false);
            }
        }
    }
}

//处理信号与槽
void Widget::init_connect()
{
    connect(ui->addTravelerButton, SIGNAL(clicked()), this, SLOT(addtravelerbutton_clicked()));
    connect(ui->TravelerComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(traveler_changed()));
    connect(ui->ThroughCityCheckBox, SIGNAL(toggled(bool)), this, SLOT(active_throughcity()));
    connect(ui->city0cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity0()));
    connect(ui->city1cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity1()));
    connect(ui->city2cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity2()));
    connect(ui->city3cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity3()));
    connect(ui->city4cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity4()));
    connect(ui->city5cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity5()));
    connect(ui->city6cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity6()));
    connect(ui->city7cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity7()));
    connect(ui->city8cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity8()));
    connect(ui->city9cbox, SIGNAL(toggled(bool)), this, SLOT(set_throungcity9()));
    connect(ui->StartButton, SIGNAL(clicked()), this, SLOT(startbutton_clicked()));
    connect(ui->StrategyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(set_deadtime_edit(int)));
    connect(right_timer, SIGNAL(timeout()), this, SLOT(display_spent_time()));
    connect(ui->StartButton, SIGNAL(clicked()), this, SLOT(right_timer_start()));
    connect(this, SIGNAL(start_timer()), right_timer, SLOT(start()));
}

//初始化计时器线程
void Widget::init_time_thread()
{
    right_timer = new QTimer;
    right_timer->setInterval(1);
    right_timer_thread = new QThread();
    right_timer_thread->start();
    right_timer->moveToThread(right_timer_thread);
}

//开始计时器
void Widget::right_timer_start()
{
    qDebug() << "右侧计时器开始";
    if (right_timer->isActive())
        return;
    emit start_timer();
}

Widget::~Widget()
{
    //安全退出时间线程
    qDebug() << "程序安全退出";
    qDebug() << "---------------------------------------------------------------------------------------------------------------------------------------------------------";
    right_timer_thread->terminate();
    right_timer_thread->wait();
    delete ui;
    delete right_timer_thread;
}

void Widget::read_traveler()
{
    QFile traveler_file("traveler.txt");
    if(!traveler_file.open(QIODevice::ReadOnly))
    {
        qCritical() << "打开旅客信息文件失败！";
        return;
    }

    QTextStream traveler_in(&traveler_file);
    QString is_start,id, strategy,origin, destination,usedDate,used_time,startDate,start_time, deadlineDate,deadline_time,isChecked,bool_throughcity;
    bool bool_isChecked,bool_is_start;
    while(!traveler_in.atEnd())
    {
        std::vector<bool> throughCity;
        traveler_in>>is_start >> id>>strategy>>origin>>destination>>usedDate>>used_time>>startDate>>start_time>>deadlineDate>>deadline_time>>isChecked;
        if(isChecked.toInt()==0)
        {
            bool_isChecked=false;
        }
        else
        {
            bool_isChecked=true;
        }
        if(is_start.toInt()==0)
        {
            bool_is_start=false;
        }
        else
        {
            bool_is_start=true;
        }
        for (unsigned i=0;i<10;i++)
        {
            traveler_in >>bool_throughcity;
            if(bool_throughcity.toInt()==0)
            {
                throughCity.push_back(false);
            }
            else
            {
                throughCity.push_back(true);
            }
        }
        traveler_in>>endl;
        if(id != "")
        {
            qDebug() <<bool_is_start<<id<<strategy<<origin<<destination<<usedDate<<used_time<<startDate<<start_time<<deadlineDate<<deadline_time<<bool_isChecked<<throughCity;
            travelers.push_back(Traveler(bool_is_start,id.toInt(),QDateTime(QDate::fromString(usedDate,"yyyy-MM-dd"),QTime::fromString(used_time,"hh:mm")),
                                         QDateTime(QDate::fromString(startDate,"yyyy-MM-dd"),QTime::fromString(start_time,"hh:mm")),
                                         QDateTime(QDate::fromString(deadlineDate,"yyyy-MM-dd"),QTime::fromString(deadline_time,"hh:mm")),
                                         strategy.toInt(), origin.toInt(),destination.toInt(),bool_isChecked, throughCity));
        }
    }
    qDebug() << "导入旅客信息成功";
    traveler_file.close();
}

void Widget::write_traveler()
{
    QFile traveler_file("traveler.txt");
    if(!traveler_file.open(QIODevice::WriteOnly))
    {
        qCritical() << "打开旅客信息文件失败！";
        return;
    }

    QTextStream traveler_in(&traveler_file);
    QString is_start, id, strategy,origin, destination,usedDate,used_time,startDate,start_time, deadlineDate,deadline_time,isChecked,bool_throughcity;
    bool bool_isChecked,bool_is_start;
    for (unsigned i=0;i<travelers.size();i++)
    {
        id.setNum(travelers[i].id);
        strategy.setNum(travelers[i].strategy);
        origin.setNum(travelers[i].origin);
        destination.setNum(travelers[i].destination);
        usedDate=travelers[i].used_time.date().toString("yyyy-MM-dd");
        used_time=travelers[i].used_time.time().toString("hh:mm");
        startDate=travelers[i].start_time.date().toString("yyyy-MM-dd");
        start_time=travelers[i].start_time.time().toString("hh:mm");
        deadlineDate=travelers[i].deadline_time.date().toString("yyyy-MM-dd");
        deadline_time=travelers[i].deadline_time.time().toString("hh:mm");
        bool_isChecked=travelers[i].isChecked;
        if(bool_isChecked==false)
        {
            isChecked.setNum(0);
        }
        else
        {
            isChecked.setNum(1);
        }
        bool_is_start=travelers[i].is_start;
        if(bool_is_start==false)
        {
            is_start.setNum(0);
        }
        else
        {
            is_start.setNum(1);
        }
        traveler_in <<is_start<<" "<< id<<" "<<strategy<<" "<<origin<<" "<<destination<<" "<<usedDate<<" "<<used_time<<" "<<startDate<<" "<<start_time<<" "<<deadlineDate<<" "<<deadline_time<<" "<<isChecked;
        for (unsigned j=0;j<10;j++)
        {
            if(travelers[i].throughCity[j]== false)
            {
                traveler_in<<QString(" 0");
            }
            else
            {
                traveler_in<<QString(" 1");
            }
        }
        traveler_in<<"\n";
        qDebug() <<is_start<<id<<strategy<<origin<<destination<<usedDate<<used_time<<startDate<<start_time<<deadlineDate<<deadline_time<<bool_isChecked<<travelers[i].throughCity;
    }
    traveler_file.flush();
    traveler_file.close();
    qDebug() << "写入旅客信息成功";
}

void Widget::closeEvent(QCloseEvent *)
{
    int button;
    button = QMessageBox::question(this, tr("退出程序"), QString(tr("是否保留这次旅客信息?")), QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::Yes)
    {
        qDebug()<<"保留旅客信息中…………";
        write_traveler();
    }
    else
    {
        qDebug()<<"清空旅客信息文件中…………";
        QFile traveler_file("traveler.txt");
        if(!traveler_file.open(QIODevice::WriteOnly))
        {
            qCritical() << "打开旅客信息文件失败！";
            return;
        }
    }
}


//开始按钮按下
void Widget::startbutton_clicked()
{
    QDateTime start_datetime;
    qDebug() << "开始按钮按下";

    if ((travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==0&&travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start== false)||(travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==1&&travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start==false))    //初次点击开始按钮
    {
        strategy = get_strategy();
        start = get_start();
        destination = get_destination();
        //始发地和目的地相同
        if (start == destination)
        {
            qWarning()<<"始发地和目的地相同";
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"出发地和目的地相同"));
            return;
        }
        //截止时间不晚于当前时间
        if ((ui->StartDateTimeEdit->dateTime() >= ui->DeadlineDateTimeEdit->dateTime())&&strategy==2)
        {
            qWarning()<<"截止时间不晚于当前时间";
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"截止时间不晚于当前时间"));
            return;
        }
        qDebug() << "旅客开始运行！";
        start_datetime = get_start_time();

        travelers[unsigned(ui->TravelerComboBox->currentIndex())] = (Traveler(true,add_traveler_times,QDateTime(QDate(1, 1, 1), QTime(0, 0, 0, 0)),start_datetime,
                                                                    get_dead_time(), strategy, start, destination, ui->ThroughCityCheckBox->isChecked(),throughcity));
        std::vector<Attribute> path = travelers[unsigned(ui->TravelerComboBox->currentIndex())].get_plan();
        if (path.size() == 0)
        {
            qWarning()<<"无有效路径";
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"无有效路径"));
            start_clicked[unsigned(ui->TravelerComboBox->currentIndex())] = false;
            return;
        }
        start_clicked[unsigned(ui->TravelerComboBox->currentIndex())] = true;
        current_traveler = ui->TravelerComboBox->currentIndex();
        display_total_time();
        display_money(path);
        display_path(path);
        ui->StartButton->setText(QString::fromWCharArray(L"更改"));
        ui->StartComboBox->setEnabled(false);
        ui->StartDateTimeEdit->setEnabled(false);
        ui->TravelerComboBox->setEnabled(true);
        ui->StrategyComboBox->setEnabled(true);
        ui->DestinationComboBox->setEnabled(true);
        ui->ThroughCityCheckBox->setEnabled(true);
        if(travelers[unsigned(ui->TravelerComboBox->currentIndex())].strategy!=2)
        {
            ui->DeadlineDateTimeEdit->setEnabled(false);
        }
        else
        {
            ui->DeadlineDateTimeEdit->setEnabled(true);
        }
        ui->StartButton->setEnabled(true);
        start_clicked_times += 1;
        start_clicked[unsigned(ui->TravelerComboBox->currentIndex())] = true;
        travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start=true;
        return;
    } 
    else if ((travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==0&&travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start == true)||travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==2)   //更改计划
    {
        strategy = get_strategy();
        start = get_start();
        destination = get_destination();

        //始发地和目的地相同
        if (start == destination)
        {
            qWarning()<<"始发地和目的地相同";
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"出发地和目的地相同"));
            return;
        }

        //截止时间早于当前时间报错
        if ((ui->StartDateTimeEdit->dateTime() >= ui->DeadlineDateTimeEdit->dateTime())&&strategy==2)
        {
            qWarning()<<"截止时间不晚于当前时间";
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"截止时间不晚于当前时间"));
            ui->DeadlineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
            return;
        }

        //获得新计划的始发地
        int nextCity2Arrive = ui->LeftWidget->next_city();
        if (nextCity2Arrive != -1)
        {
            std::vector<Attribute> path = travelers[unsigned(ui->TravelerComboBox->currentIndex())].change_plan
                 (nextCity2Arrive, strategy, destination, get_dead_time(),ui->ThroughCityCheckBox->isChecked(),throughcity);
            if (path.size() == 0)
            {
                qWarning()<<"无有效路径";
                QMessageBox::information(this, "Error", QString::fromWCharArray(L"无有效路径"));
                return;
            }
            qDebug() << "更改计划成功！";
            current_traveler = ui->TravelerComboBox->currentIndex();
            display_total_time();
            display_money(path);
            display_path(path);
        }
        ui->StartButton->setText(QString::fromWCharArray(L"更改"));
        ui->StartComboBox->setEnabled(false);
        ui->StartDateTimeEdit->setEnabled(false);
        ui->TravelerComboBox->setEnabled(true);
        ui->StrategyComboBox->setEnabled(true);
        ui->DestinationComboBox->setEnabled(true);
        ui->ThroughCityCheckBox->setEnabled(true);
        if(travelers[unsigned(ui->TravelerComboBox->currentIndex())].strategy!=2)
        {
            ui->DeadlineDateTimeEdit->setEnabled(false);
        }
        else
        {
            ui->DeadlineDateTimeEdit->setEnabled(true);
        }
        ui->StartButton->setEnabled(true);
        start_clicked_times += 1;
        start_clicked[unsigned(ui->TravelerComboBox->currentIndex())] = true;
    }
    else if (travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==1&&travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start==true)
    {
        travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue=2;
        strategy = get_strategy();
        start = get_start();
        destination = get_destination();
        //始发地和目的地相同
        if (start == destination)
        {
            qWarning()<<"始发地和目的地相同";
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"出发地和目的地相同"));
            return;
        }
        //截止时间不晚于当前时间
        if ((ui->StartDateTimeEdit->dateTime() >= ui->DeadlineDateTimeEdit->dateTime())&&strategy==2)
        {
            qWarning()<<"截止时间不晚于当前时间";
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"截止时间不晚于当前时间"));
            return;
        }
        qDebug() << "旅客开始运行！";
        start_datetime = get_start_time();

        std::vector<Attribute> path = travelers[unsigned(ui->TravelerComboBox->currentIndex())].get_plan();
        if (path.size() == 0)
        {
            qWarning()<<"无有效路径";
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"无有效路径"));
            start_clicked[unsigned(ui->TravelerComboBox->currentIndex())] = false;
            return;
        }
        start_clicked[unsigned(ui->TravelerComboBox->currentIndex())] = true;
        current_traveler = ui->TravelerComboBox->currentIndex();
        display_total_time();
        display_money(path);
        display_path(path);
        ui->StartButton->setText(QString::fromWCharArray(L"更改"));
        ui->StartComboBox->setEnabled(false);
        ui->StartDateTimeEdit->setEnabled(false);
        ui->TravelerComboBox->setEnabled(true);
        ui->StrategyComboBox->setEnabled(true);
        ui->DestinationComboBox->setEnabled(true);
        ui->ThroughCityCheckBox->setEnabled(true);
        if(travelers[unsigned(ui->TravelerComboBox->currentIndex())].strategy!=2)
        {
            ui->DeadlineDateTimeEdit->setEnabled(false);
        }
        else
        {
            ui->DeadlineDateTimeEdit->setEnabled(true);
        }
        ui->StartButton->setEnabled(true);
        start_clicked_times += 1;
        start_clicked[unsigned(ui->TravelerComboBox->currentIndex())] = true;
        travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start=true;
        return;
    }
    active_throughcity();
}

//根据策略改变截止日期栏状态
void Widget::set_deadtime_edit(int current_strategy)
{
    qDebug() << "策略改变至：" << current_strategy;
    if (current_strategy != 2)
        ui->DeadlineDateTimeEdit->setEnabled(false);
    else
        ui->DeadlineDateTimeEdit->setEnabled(true);
}

//添加旅客按钮按下
void Widget::addtravelerbutton_clicked()
{
    qDebug() << "添加新旅客";
    std::vector<bool> temp(10, false);
    throughcity = temp;
    start_clicked.push_back(false);
    add_traveler_times += 1;
    start_clicked_times = 0;

    travelers.push_back(Traveler(false,add_traveler_times,QDateTime(QDate(1, 1, 1), QTime(0, 0, 0, 0)), get_start_time(), get_dead_time(),get_strategy(), get_start(),
                                 get_destination(),false,throughcity));

    ui->TravelerComboBox->addItem(QString::number(add_traveler_times));
    ui->TravelerComboBox->setCurrentText(QString::number(add_traveler_times));
    ui->StartButton->setText(QString::fromWCharArray(L"开始"));
    ui->TravelerComboBox->setEnabled(true);
    ui->StartComboBox->setEnabled(true);
    ui->StrategyComboBox->setEnabled(true);
    ui->DestinationComboBox->setEnabled(true);
    ui->StrategyComboBox->setCurrentIndex(0);
    ui->StartComboBox->setCurrentIndex(0);
    ui->DestinationComboBox->setCurrentIndex(1);
    ui->ThroughCityCheckBox->setEnabled(true);
    ui->DeadlineDateTimeEdit->setEnabled(false);
    ui->StartDateTimeEdit->setEnabled(true);
    ui->StartButton->setEnabled(true);
    ui->StartDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->DeadlineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

//更改旅客
void Widget::traveler_changed()
{
    qDebug() << "更改旅客";
    ui->StartButton->setEnabled(true);
    //当前旅客已经开始
    if (travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start)
    {
        qDebug() << "当前旅客已经开始";
        ui->StartDateTimeEdit->setDateTime(travelers[unsigned(ui->TravelerComboBox->currentIndex())].start_time);
        ui->DeadlineDateTimeEdit->setDateTime(travelers[unsigned(ui->TravelerComboBox->currentIndex())].deadline_time);
        ui->StrategyComboBox->setCurrentIndex(travelers[unsigned(ui->TravelerComboBox->currentIndex())].strategy);
        ui->StartComboBox->setCurrentIndex(travelers[unsigned(ui->TravelerComboBox->currentIndex())].origin);
        ui->DestinationComboBox->setCurrentIndex(travelers[unsigned(ui->TravelerComboBox->currentIndex())].destination);
        display_money(travelers[unsigned(ui->TravelerComboBox->currentIndex())].get_plan());
        display_total_time();
        display_path(travelers[unsigned(ui->TravelerComboBox->currentIndex())].get_plan());
        display_spent_time();
        ui->ThroughCityCheckBox->setChecked(travelers[unsigned(ui->TravelerComboBox->currentIndex())].isChecked);
        throughcity = travelers[unsigned(ui->TravelerComboBox->currentIndex())].throughCity;
        active_throughcity();
        if(travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==0||travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==2)
        {
            ui->StartButton->setText(QString::fromWCharArray(L"更改"));
            if(travelers[unsigned(ui->TravelerComboBox->currentIndex())].strategy!=2)
            {
                ui->DeadlineDateTimeEdit->setEnabled(false);
            }
            else
            {
                ui->DeadlineDateTimeEdit->setEnabled(true);
            }
            ui->StartComboBox->setEnabled(false);
            ui->StartDateTimeEdit->setEnabled(false);
            ui->StrategyComboBox->setEnabled(true);
            ui->DestinationComboBox->setEnabled(true);
            ui->ThroughCityCheckBox->setEnabled(true);
        }
        else
        {
            ui->StartButton->setText(QString::fromWCharArray(L"开始"));
            ui->StrategyComboBox->setEnabled(false);
            ui->StartComboBox->setEnabled(false);
            ui->DestinationComboBox->setEnabled(false);
            ui->ThroughCityCheckBox->setEnabled(false);
            ui->city0cbox->setEnabled(false);
            ui->city1cbox->setEnabled(false);
            ui->city2cbox->setEnabled(false);
            ui->city3cbox->setEnabled(false);
            ui->city4cbox->setEnabled(false);
            ui->city5cbox->setEnabled(false);
            ui->city6cbox->setEnabled(false);
            ui->city7cbox->setEnabled(false);
            ui->city8cbox->setEnabled(false);
            ui->city9cbox->setEnabled(false);
            ui->StartDateTimeEdit->setEnabled(false);
            ui->DeadlineDateTimeEdit->setEnabled(false);
        }
        current_traveler = ui->TravelerComboBox->currentIndex();
    }
    //当前旅客未开始
    else
    {
        qDebug() << "当前旅客未开始";
        ui->StartButton->setText(QString::fromWCharArray(L"开始"));
        ui->StrategyComboBox->setEnabled(true);
        ui->StartComboBox->setEnabled(true);
        ui->DestinationComboBox->setEnabled(true);
        ui->StartDateTimeEdit->setEnabled(true);
        ui->ThroughCityCheckBox->setEnabled(true);
        ui->ThroughCityCheckBox->setChecked(travelers[unsigned(ui->TravelerComboBox->currentIndex())].isChecked);
        throughcity = travelers[unsigned(ui->TravelerComboBox->currentIndex())].throughCity;
        active_throughcity();

        ui->StartDateTimeEdit->setDateTime(travelers[unsigned(ui->TravelerComboBox->currentIndex())].start_time);
        ui->DeadlineDateTimeEdit->setDateTime(travelers[unsigned(ui->TravelerComboBox->currentIndex())].deadline_time);
        if(travelers[unsigned(ui->TravelerComboBox->currentIndex())].strategy!=2)
        {
            ui->DeadlineDateTimeEdit->setEnabled(false);
        }
        ui->StrategyComboBox->setCurrentIndex(travelers[unsigned(ui->TravelerComboBox->currentIndex())].strategy);
        ui->StartComboBox->setCurrentIndex(travelers[unsigned(ui->TravelerComboBox->currentIndex())].origin);
        ui->DestinationComboBox->setCurrentIndex(travelers[unsigned(ui->TravelerComboBox->currentIndex())].destination);
        QVBoxLayout *listlayout = new QVBoxLayout;
        QWidget *containwidget = new QWidget(ui->PathList);
        containwidget->setLayout(listlayout);
        ui->PathList->setWidget(containwidget);
        ui->FareEdit->clear();
        ui->TotalTimeEdit->clear();
        ui->DurationText->clear();
        current_traveler = -1;
    }
}

//获取策略
int Widget::get_strategy()
{
    qDebug() << "获取策略成功！";
    return ui->StrategyComboBox->currentIndex();
}

//获取始发地
int Widget::get_start()
{
    qDebug() << "获取始发地成功！";
    return ui->StartComboBox->currentIndex();
}

//获取目的地
int Widget::get_destination()
{
    qDebug() << "获取目的地成功！";
    return ui->DestinationComboBox->currentIndex();
}

//获取截止时间
QDateTime Widget::get_dead_time()
{
    qDebug() << "获取截止时间成功！";
    return ui->DeadlineDateTimeEdit->dateTime();
}

//获取开始时间
QDateTime Widget::get_start_time()
{
    qDebug() << "获取开始时间成功！";
    return ui->StartDateTimeEdit->dateTime();
}

//获取已用时间，按照10s = 1h的比率即1:360得到旅客经过时间
QDateTime Widget::get_spend_time()
{    
    QDateTime used_datetime = travelers[unsigned(ui->TravelerComboBox->currentIndex())].used_time;

    int day = used_datetime.date().day();
    int hour = used_datetime.time().hour();
    int min = used_datetime.time().minute();
    int sec = used_datetime.time().second();
    int msec = used_datetime.time().msec();

    msec += 360;

    sec += msec / 1000;
    msec = msec % 1000;
    min += sec / 60;
    sec = sec % 60;
    hour += min / 60;
    min = min % 60;
    day += hour /24;
    hour = hour % 24;
    day = day % 30;
    travelers[unsigned(ui->TravelerComboBox->currentIndex())].used_time = QDateTime(QDate(1, 1, day), QTime(hour, min, sec, msec));
    return travelers[unsigned(ui->TravelerComboBox->currentIndex())].used_time;
}

//显示已用时间
void Widget::display_spent_time()
{
    if(ui->StartButton->text()==QString::fromWCharArray(L"更改"))
    {
        ui->StartButton->setEnabled(travelers[unsigned(ui->TravelerComboBox->currentIndex())].flag);
    }
    if(ui->StartButton->text()==QString::fromWCharArray(L"开始"))
    {
        ui->StartButton->setEnabled(true);
    }
    //当前用户已经开始
    if (travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start==true&&(travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==0||travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_continue==2))
    {
        QDateTime spentTime = get_spend_time();
        //已用时间不超过计划用总时间
        if (travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time >= spentTime)
        {
            ui->DurationText->setText(QString::number(spentTime.date().day()-1) + QString::fromWCharArray(L"天 ")
                    + QString::number(spentTime.time().hour()) + QString::fromWCharArray(L"小时 ")
                    + QString::number(spentTime.time().minute()) + QString::fromWCharArray(L"分钟"));
        }
        //已用时间超过计划用总时间
        else if (ui->StartButton->isEnabled())
        {
            ui->DurationText->setText(QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.date().day()-1)
                    + QString::fromWCharArray(L"天 ") +
                    QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.time().hour())
                    + QString::fromWCharArray(L"小时 ") +
                    QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.time().minute())
                    + QString::fromWCharArray(L"分钟"));
            ui->StartButton->setEnabled(false);
            qDebug() << "该旅客已经到达目的地，花费总时间: " << QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.date().day()-1)
                    + QString::fromWCharArray(L"天 ") +
                    QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.time().hour())
                    + QString::fromWCharArray(L"小时 ") +
                    QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.time().minute())
                    + QString::fromWCharArray(L"分钟");
        }
    }
    else if(travelers[unsigned(ui->TravelerComboBox->currentIndex())].is_start==false)
    {
        ui->DurationText->clear();
    }
    else
    {
        QDateTime spentTime = travelers[unsigned(ui->TravelerComboBox->currentIndex())].used_time;
        //已用时间不超过计划用总时间
        if (travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time >= spentTime)
        {
            ui->DurationText->setText(QString::number(spentTime.date().day()-1) + QString::fromWCharArray(L"天 ")
                    + QString::number(spentTime.time().hour()) + QString::fromWCharArray(L"小时 ")
                    + QString::number(spentTime.time().minute()) + QString::fromWCharArray(L"分钟"));
        }
        //已用时间超过计划用总时间
        else if (ui->StartButton->isEnabled())
        {
            ui->DurationText->setText(QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.date().day()-1)
                    + QString::fromWCharArray(L"天 ") +
                    QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.time().hour())
                    + QString::fromWCharArray(L"小时 ") +
                    QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.time().minute())
                    + QString::fromWCharArray(L"分钟"));
            ui->StartButton->setEnabled(false);
            qDebug() << "该旅客已经到达目的地，花费总时间: " << QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.date().day()-1)
                    + QString::fromWCharArray(L"天 ") +
                    QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.time().hour())
                    + QString::fromWCharArray(L"小时 ") +
                    QString::number(travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time.time().minute())
                    + QString::fromWCharArray(L"分钟");
        }
    }
}

//显示方案所需总时间
void Widget::display_total_time()
{
    QDateTime total_time = travelers[unsigned(ui->TravelerComboBox->currentIndex())].total_time;
    qDebug() <<"所需总时间为："<< QString::number(total_time.date().day()-1) + QString::fromWCharArray(L"天 ") +
                QString::number(total_time.time().hour()) + QString::fromWCharArray(L"小时 ") +
                QString::number(total_time.time().minute()) + QString::fromWCharArray(L"分钟");
    ui->TotalTimeEdit->setText(QString::number(total_time.date().day()-1) + QString::fromWCharArray(L"天 ") +
                               QString::number(total_time.time().hour()) + QString::fromWCharArray(L"小时 ") +
                               QString::number(total_time.time().minute()) + QString::fromWCharArray(L"分钟"));
}

//显示方案所需钱
void Widget::display_money(std::vector<Attribute> path)
{
    int totalcost = 0;
    for(std::vector<Attribute>::size_type index = 0;
            index != path.size(); index++)
    {
        totalcost += path[index].cost;
    }
    qDebug() << "总花费为："<<QString::number(totalcost) + QString::fromWCharArray(L"元");
    ui->FareEdit->setText(QString::number(totalcost) + QString::fromWCharArray(L"元"));
}

//将方案中城市编号对应城市名称
QString Widget::citynum_to_cityname(int index){
    QString city;
    switch (index)
    {
    case 0:
        city = QString::fromWCharArray(L"北京");
        break;
    case 1:
        city = QString::fromWCharArray(L"哈尔滨");
        break;
    case 2:
        city = QString::fromWCharArray(L"上海");
        break;
    case 3:
        city = QString::fromWCharArray(L"重庆");
        break;
    case 4:
        city = QString::fromWCharArray(L"济南");
        break;
    case 5:
        city = QString::fromWCharArray(L"乌鲁木齐");
        break;
    case 6:
        city = QString::fromWCharArray(L"拉萨");
        break;
    case 7:
        city = QString::fromWCharArray(L"香港");
        break;
    case 8:
        city = QString::fromWCharArray(L"台北");
        break;
    case 9:
        city = QString::fromWCharArray(L"郑州");
        break;
    default:
        qCritical()<<"城市编号错误";
        QMessageBox::warning(this, "Error", QString::fromWCharArray(L"错误"));
        break;
    }
    return city;
}

//显示路径
void Widget::display_path(std::vector<Attribute> path)
{
    QVBoxLayout *listlayout = new QVBoxLayout;
    QWidget *containwidget = new QWidget(ui->PathList);

    for(std::vector<Attribute>::size_type index = 0;
            index != path.size(); index++)
    {
        QLabel *vehiclelabel = new QLabel;
        QLabel *textlabel = new QLabel;
        if (path[index].vehicle == 0)
            vehiclelabel->setPixmap(QPixmap("://resource/car.png").scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        else if (path[index].vehicle == 1)
            vehiclelabel->setPixmap(QPixmap("://resource/train.png").scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        else if (path[index].vehicle == 2)
            vehiclelabel->setPixmap(QPixmap("://resource/plane.png").scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        textlabel->setText(citynum_to_cityname(path[index].from) + "->" + citynum_to_cityname(path[index].to) +
                           QString::fromWCharArray(L" 班次:") + path[index].num + "\n" +
                           QString::fromWCharArray(L"始发站:") + path[index].begin.toString("hh:mm") +
                           QString::fromWCharArray(L" 终点站:") + path[index].end.toString("hh:mm") + "\n" +
                           QString::fromWCharArray(L"花费:") + QString::number(path[index].cost));
        qDebug()<<citynum_to_cityname(path[index].from) + "->" + citynum_to_cityname(path[index].to);
        qDebug()<<QString::fromWCharArray(L"班次:") + path[index].num;
        qDebug()<<QString::fromWCharArray(L"始发站:") + path[index].begin.toString("hh:mm");
        qDebug()<<QString::fromWCharArray(L"终点站:") + path[index].end.toString("hh:mm");
        qDebug()<<QString::fromWCharArray(L"花费:") + QString::number(path[index].cost);
        QHBoxLayout *rowlayout = new QHBoxLayout;
        rowlayout->addStretch(1);
        rowlayout->addWidget(vehiclelabel);
        rowlayout->addWidget(textlabel);
        rowlayout->addStretch(1);
        listlayout->addLayout(rowlayout);
    }
    containwidget->setLayout(listlayout);
    ui->PathList->setWidget(containwidget);
}

//途经城市设置可选并初始化
void Widget::active_throughcity()
{
    ui->city0cbox->setChecked(throughcity[0]);
    ui->city1cbox->setChecked(throughcity[1]);
    ui->city2cbox->setChecked(throughcity[2]);
    ui->city3cbox->setChecked(throughcity[3]);
    ui->city4cbox->setChecked(throughcity[4]);
    ui->city5cbox->setChecked(throughcity[5]);
    ui->city6cbox->setChecked(throughcity[6]);
    ui->city7cbox->setChecked(throughcity[7]);
    ui->city8cbox->setChecked(throughcity[8]);
    ui->city9cbox->setChecked(throughcity[9]);

    if (ui->ThroughCityCheckBox->isChecked())
    {
        ui->city0cbox->setEnabled(true);
        ui->city1cbox->setEnabled(true);
        ui->city2cbox->setEnabled(true);
        ui->city3cbox->setEnabled(true);
        ui->city4cbox->setEnabled(true);
        ui->city5cbox->setEnabled(true);
        ui->city6cbox->setEnabled(true);
        ui->city7cbox->setEnabled(true);
        ui->city8cbox->setEnabled(true);
        ui->city9cbox->setEnabled(true);
    }
    else
    {
        ui->city0cbox->setEnabled(false);
        ui->city1cbox->setEnabled(false);
        ui->city2cbox->setEnabled(false);
        ui->city3cbox->setEnabled(false);
        ui->city4cbox->setEnabled(false);
        ui->city5cbox->setEnabled(false);
        ui->city6cbox->setEnabled(false);
        ui->city7cbox->setEnabled(false);
        ui->city8cbox->setEnabled(false);
        ui->city9cbox->setEnabled(false);
    }
}

//设置10个城市哪些被选中
void Widget::set_throungcity0()
{
   if (ui->city0cbox->isChecked())
       throughcity[0] = true;
   else
       throughcity[0] = false;
}
void Widget::set_throungcity1()
{
    if (ui->city1cbox->isChecked())
        throughcity[1] = true;
    else
        throughcity[1] = false;
}
void Widget::set_throungcity2()
{
    if (ui->city2cbox->isChecked())
        throughcity[2] = true;
    else
        throughcity[2] = false;
}
void Widget::set_throungcity3()
{
    if (ui->city3cbox->isChecked())
        throughcity[3] = true;
    else
        throughcity[3] = false;
}
void Widget::set_throungcity4()
{
   if (ui->city4cbox->isChecked())
       throughcity[4] = true;
   else
       throughcity[4]= false;
}
void Widget::set_throungcity5()
{
    if (ui->city5cbox->isChecked())
        throughcity[5] = true;
    else
        throughcity[5] = false;
}
void Widget::set_throungcity6()
{
    if (ui->city6cbox->isChecked())
        throughcity[6] = true;
    else
        throughcity[6] = false;
}
void Widget::set_throungcity7()
{
    if (ui->city7cbox->isChecked())
        throughcity[7] = true;
    else
        throughcity[7] = false;
}
void Widget::set_throungcity8()
{
    if (ui->city8cbox->isChecked())
        throughcity[8] = true;
    else
        throughcity[8] = false;
}
void Widget::set_throungcity9()
{
    if (ui->city9cbox->isChecked())
        throughcity[9] = true;
    else
        throughcity[9] = false;
}
