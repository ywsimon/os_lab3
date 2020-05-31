#include "form.h"
#include "ui_form.h"
#include<QHeaderView>
#include<QTableWidget>
#include<QDebug>
Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    setlayout();
    connectslots();

}

Form::~Form()
{
    delete ui;
}

void Form:: connectslots(){
    connect(this->ui->pushButton_2,SIGNAL(clicked()),this,SLOT(request_memory()));
    connect(this->ui->pushButton,SIGNAL(clicked()),this,SLOT(create_thread()));
    connect(this->ui->pushButton_3,SIGNAL(clicked()),this,SLOT(recycle_memory()));
}


void Form::  setlayout(){
    this->setWindowTitle(QStringLiteral("内存管理模型"));
    this->setFixedSize(this->size());
    pid_memo.clear();
    pid_memo.push_back(0);
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdNumber->setStyleSheet("border: 1px solid green; color: red; background: silver;");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->ui->tableWidget->setColumnCount(3); //设置列数
    QHeaderView* headerView =  ui->tableWidget->verticalHeader();
    headerView->setHidden(true);
    QStringList header;
    header<<QStringLiteral("区号")<<QStringLiteral("大小")<<QStringLiteral("起始地址");
    this->ui->tableWidget->setHorizontalHeaderLabels(header);


    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->ui->tableWidget_2->setColumnCount(3); //设置列数
    //QHeaderView* headerView_2 =  ui->tableWidget_2->verticalHeader();
    //headerView_2->setHidden(true);
    QStringList header2;
    header2<<QStringLiteral("大小")<<QStringLiteral("起始地址")<<QStringLiteral("进程号");
    this->ui->tableWidget_2->setHorizontalHeaderLabels(header2);

//    ui->tableWidget_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableWidget_3->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    this->ui->tableWidget_3->setColumnCount(1); //设置列数
//    QHeaderView* headerView_3 =  ui->tableWidget_3->verticalHeader();
//    headerView_3->setHidden(true);
//    QStringList header3;
//    header3<<QStringLiteral("内存图");
//    this->ui->tableWidget_3->setHorizontalHeaderLabels(header3);
}

void Form:: init(int sum, int os ,int pnum, int page_size){

    osmemo=os;
    summemo=sum;
    freememo=summemo-osmemo;
    freenum=pnum;
    fenqudaxiao=sum/pnum;
    pid_memo[0]=0;
    Busy_list = new busy_block();
    Free_list = new free_block();

    Busy_list->pid=QStringLiteral("操作系统");
    Busy_list->address=0;
    Busy_list->size=os;
    Busy_list->next = nullptr;  //初始化占用地址块链表

    free_block *f=new free_block;
    Free_list->next=f;
    free_block *prev;
    prev=f;
    for(int i=0;i<pnum;i++)
    {
        f->size=sum/pnum;
        f->address=os+(i)*(sum/pnum);
        f->num=i+1;
        f->next=new free_block();
        prev=f;
        f=f->next;
    }
    Free_list->next->size-=os;
    prev->next=nullptr;
     delete f;

//    this->ui->tableWidget_3->insertRow(0);
//    QTableWidgetItem *item = new QTableWidgetItem();
//    item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
//    item->setBackgroundColor(QColor(255,0,0));
//    item->setText(QStringLiteral("系统占用"));
//    this->ui->tableWidget_3->setItem(0, 0, item);
//     item->setTextColor(QColor(0,0,0));
//    ui->tableWidget_3->item(0,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    pagesize=page_size;
    this->ui->label_8->setText("("+QString::number(page_size)+"KB"+")");
    update_table();
    darw_chart();
    this->ui->textBrowser->append("<font color=\"#00FF00\">"+QStringLiteral("欢迎使用内存管理模型"));
}

void Form:: update_table(){

    int temp=this->ui->tableWidget->rowCount();
    for(int i=0;i<temp;i++)
        this->ui->tableWidget->removeRow(0);
     temp=this->ui->tableWidget_2->rowCount();
    for(int i=0;i<temp;i++)
        this->ui->tableWidget_2->removeRow(0);
    sort_usedmemo();
    free_block *f=Free_list->next;
    if(f!=nullptr){
        //QStringList header;
        //header<<QStringLiteral("区号")<<QStringLiteral("大小")<<QStringLiteral("起始地址");
        //this->ui->tableWidget->setHorizontalHeaderLabels(header);
        int i=0;
        while(f!=nullptr){
            this->ui->tableWidget->insertRow(i);   //插入新行

            QTableWidgetItem *item = new QTableWidgetItem();
             item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
            QString num=QString::number(f->num);
            item->setText(num);
            item->setTextColor(QColor(0,0,0));
            item->setBackgroundColor(QColor(85,255,127));
            this->ui->tableWidget->setItem(i, 0, item);
            ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            QTableWidgetItem *item2 = new QTableWidgetItem();
             item2->setFlags(item2->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
            QString size=QString::number(f->size);
            item2->setText(size+"KB");
            item2->setTextColor(QColor(0,0,0));
            item2->setBackgroundColor(QColor(85,255,127));
            this->ui->tableWidget->setItem(i, 1, item2);
            ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            QTableWidgetItem *item3 = new QTableWidgetItem();
            item3->setFlags(item3->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
            QString start=QString::number(f->address);
            item3->setText(start+"KB");
            item3->setTextColor(QColor(0,0,0));
            item3->setBackgroundColor(QColor(85,255,127));
            this->ui->tableWidget->setItem(i, 2, item3);
            ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            f=f->next;
            ++i;

         }
    }

    busy_block *b=Busy_list;
    if(b!=nullptr){
        QStringList header;
        header<<QStringLiteral("大小")<<QStringLiteral("起始地址")<<QStringLiteral("进程号");
        this->ui->tableWidget_2->setHorizontalHeaderLabels(header);
        int  i=0;
        while(b!=nullptr){
            this->ui->tableWidget_2->insertRow(i);   //插入新行
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
            QString size=QString::number(b->size);
            item->setText(size+"KB");
            item->setTextColor(QColor(0,0,0));
            item->setBackgroundColor(QColor(255,255,0));
            this->ui->tableWidget_2->setItem(i, 0, item);
            ui->tableWidget_2->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            QTableWidgetItem *item2 = new QTableWidgetItem();
            item2->setFlags(item2->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
            QString start=QString::number(b->address);
            item2->setText(start+"KB");
            item2->setTextColor(QColor(0,0,0));
            item2->setBackgroundColor(QColor(255,255,0));
            this->ui->tableWidget_2->setItem(i, 1, item2);
            ui->tableWidget_2->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            QTableWidgetItem *item3 = new QTableWidgetItem();
            item3->setFlags(item3->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
            QString pid=b->pid;
            item3->setText(pid);
            item3->setTextColor(QColor(0,0,0));
            item3->setBackgroundColor(QColor(255,255,0));
            this->ui->tableWidget_2->setItem(i, 2, item3);
            ui->tableWidget_2->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            b=b->next;
            ++i;
        }
        for(int j=0;j<3;j++)
         this->ui->tableWidget_2->item(0,j)->setBackgroundColor(QColor(0,0,255));

    }



}

void Form:: FF(){
    free_block* slowptr = Free_list->next;
    free_block* fastptr = Free_list->next;
        int size;
        int addr;
        int num;
        while (slowptr !=nullptr && slowptr->next != nullptr) {
                fastptr = slowptr->next;
                while (fastptr != nullptr) {
                    if (slowptr->address > fastptr->address) {
                        addr = slowptr->address;
                        slowptr->address = fastptr->address;
                        fastptr->address = addr;
                        size = slowptr->size;
                        slowptr->size = fastptr->size;
                        fastptr->size = size;
                        num=slowptr->num;
                        slowptr->num=fastptr->num;
                        fastptr->num=num;
                    }
                    fastptr = fastptr->next;
                }
                slowptr = slowptr->next;
            }
}

void Form:: BF(){
    free_block* slowptr = Free_list->next;
    free_block* fastptr = Free_list->next;
        int size;
        int addr;
        int num;
        while (slowptr !=nullptr && slowptr->next != nullptr) {
                fastptr = slowptr->next;
                while (fastptr != nullptr) {
                    if (slowptr->size > fastptr->size) {
                        addr = slowptr->address;
                        slowptr->address = fastptr->address;
                        fastptr->address = addr;
                        size = slowptr->size;
                        slowptr->size = fastptr->size;
                        fastptr->size = size;
                        num=slowptr->num;
                        slowptr->num=fastptr->num;
                        fastptr->num=num;
                    }
                    fastptr = fastptr->next;
                }
                slowptr = slowptr->next;
            }
}

void Form:: WF(){
    free_block* slowptr = Free_list->next;
    free_block* fastptr = Free_list->next;
        int size;
        int addr;
        int num;
        while (slowptr !=nullptr && slowptr->next != nullptr) {
                fastptr = slowptr->next;
                while (fastptr != nullptr) {
                    if (slowptr->size < fastptr->size) {
                        addr = slowptr->address;
                        slowptr->address = fastptr->address;
                        fastptr->address = addr;
                        size = slowptr->size;
                        slowptr->size = fastptr->size;
                        fastptr->size = size;
                        num=slowptr->num;
                        slowptr->num=fastptr->num;
                        fastptr->num=num;
                    }
                    fastptr = fastptr->next;
                }
                slowptr = slowptr->next;
            }
}

void Form :: sort_usedmemo(){
    busy_block* slowptr = Busy_list->next;
    busy_block* fastptr = Busy_list->next;
        int size;
        int addr;
        QString pid;
        while (slowptr !=nullptr && slowptr->next != nullptr) {
                fastptr = slowptr->next;
                while (fastptr != nullptr) {
                    if (slowptr->address > fastptr->address) {
                        addr = slowptr->address;
                        slowptr->address = fastptr->address;
                        fastptr->address = addr;
                        size = slowptr->size;
                        slowptr->size = fastptr->size;
                        fastptr->size = size;
                        pid=slowptr->pid;
                        slowptr->pid=fastptr->pid;
                        fastptr->pid=pid;
                    }
                    fastptr = fastptr->next;
                }
                slowptr = slowptr->next;
            }
}

void Form:: request_memory(){
    int select=this->ui->comboBox->currentIndex();
    switch(select){
    case 0:
        FF();
        break;
    case 1:
        BF();
        break;
    case 2:
        WF();
        break;
    default:
        break;
    }

    int req_pid=this->ui->spinBox->value();
    int req_memo=this->ui->spinBox_2->value()*pagesize;

    busy_block *newnode=new busy_block();

    free_block *p = Free_list->next;
    free_block *prev=Free_list;
        while (p != nullptr)
        {
            if (p->size >= req_memo)
            {
                freememo-=req_memo;
                pid_memo[req_pid-1]+=req_memo;
                newnode->next = Busy_list->next;
                Busy_list->next = newnode;
                newnode->size = req_memo;
                newnode->address = p->address;
                newnode->pid=QString::number(req_pid);

                p->size -= req_memo;
                p->address +=req_memo;
                if(p->size==0)
                {
                    prev->next=p->next;
                    delete p;
                    p=nullptr;
                }
                QString logtext=QStringLiteral("将首地址为%1 KB,大小为%2 KB的内存空间分配给进程%3 \n").arg(newnode->address).arg( newnode->size).arg(newnode->pid);
                this->ui->textBrowser->append("<font color=\"#0000FF\">"+logtext);
                update_table();
                darw_chart();
                return;
            }
            prev=p;
            p = p->next;
        }

       QString logtext=QStringLiteral("没有完整的空闲分区");
        this->ui->textBrowser->append("<font color=\"#FF0000\">"+logtext);
       return;
}

void Form:: create_thread(){
    ++sumpid;
    this->ui->spinBox->setMaximum(sumpid);
    this->ui->spinBox_3->setMaximum(sumpid);
    this->ui->lcdNumber->display(sumpid);
    pid_memo.push_back(0);
      this->ui->textBrowser->append("<font color=\"#FFFF00\">"+QStringLiteral("新创建了一个进程"));
}

void Form:: darw_chart(){

        QPieSeries *series = new QPieSeries();
        series->append(QStringLiteral("操作系统%1KB").arg(osmemo), osmemo)->setColor(QColor(0, 200, 200));	// 构造两个饼状分区
        for(int i=0;i<sumpid;i++)
        {
            if(pid_memo[i]==0)
                continue;
           series->append(QStringLiteral("线程%1 %2KB").arg(i+1).arg(pid_memo[i]),pid_memo[i])->setColor(QColor(255, 255,(5*i%115)+1));//设置每个分区的占比与颜色
       }

        series->append(QStringLiteral("空闲区%1KB").arg(freememo),freememo )->setColor(QColor(0, 255, 0));	// 构造两个饼状分区
        series->setLabelsVisible();  //每个分区label显示
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle(QStringLiteral("内存占用情况"));
        chart->legend()->hide();									//是否显示图例
        chart->setAnimationOptions(QChart::AllAnimations); // 设置显示时的动画效果

        QChartView *chartView;										//构造图表视图
        chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing); //抗锯齿处理
        chartView->setParent(this);
        chartView->move(535,350);
        chartView->setFixedSize(430,320);
        chartView->show();

}

void Form::recycle_memory(){

    int xpid = this->ui->spinBox_3->value();
    pid_memo[xpid-1]=0;
        busy_block *b ;
        busy_block *c ;

        c = Busy_list;
        b = c->next;
        while (b != nullptr)
        {
         if (b->pid == QString::number(xpid)){
              this->ui->textBrowser->append("<font color=\"#00FF00\">"+QStringLiteral("回收地址为%1线程号为%2大小为%3的内存\n").arg(b->address).arg(b->pid).arg(b->size));
                 c->next = b->next;  //回收算法 完成地址块的合并
                 free_block *t = new free_block;
                 t->next = Free_list->next;
                 Free_list->next = t;
                 t->size = b->size;
                 t->address = b->address;
                 t->num=++freenum;
                 b = c->next;
                 continue;
            }
            c = b;
            b = b->next;
        }

        FF();  //地址递增排序
        free_block *p,*q;
        q = Free_list->next;
        p = q->next;
        while (p != nullptr)   //合并内存
        {
            if (q->address + q->size == p->address && (p->size + q->size )<=fenqudaxiao)
            {
              this->ui->textBrowser->append("<font color=\"#0000FF\">"+QStringLiteral("合并地址为%1和%2的空间！\n").arg(q->address).arg(p->address));
                q->size += p->size;
                q->next = p->next;
                free(p);
                p = q->next;
                continue;
            }
            q = p;
            p = p->next;
        }

        update_table();
        darw_chart();
    }


