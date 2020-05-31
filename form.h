#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include<QTableWidget>
#include<vector>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGridLayout>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>
QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE
namespace Ui {
class Form;
}
QT_CHARTS_USE_NAMESPACE
struct free_block{         //内存空闲分区
    int num;               //区号
    int size;             //分区大小
    int address;          //起始地址
    free_block * next;   //指向下一节点指针
};
struct busy_block{
    int size;              //分区大小
    int address;         //起始地址
    QString pid;         //占用线程
    busy_block * next;  //指向下一节点指针
};

class Form : public QWidget
{
    Q_OBJECT
public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    void setlayout();             //初始化布局
    void init(int sum, int os ,int pnum, int page_size);  //内存模型初始化
    void  update_table();         //更新内存使用情况

private slots:
    void  request_memory();      //申请内存
    void  recycle_memory();      //回收内存
    void  create_thread();       //创建进程

private:
   void  connectslots();        //连接槽函数
   void  sort_usedmemo();       //排序已用内存
   void  darw_chart();          //饼状图呈现内存使用情况
   void  FF();    //最先匹配算法
   void  BF();    //最佳匹配算法
   void  WF();    //最坏匹配算法
   free_block *Free_list;      //空闲内存链表
   busy_block *Busy_list;      //已用内存链表
   std::vector<int> pid_memo;  //记录各线程使用内存
   int  pagesize;              //物理页面大小
   int  osmemo;                //操作系统占用内存大小
   int summemo;                //总内存大小
   int freememo;               //空闲内存大小
   int  sumpid=1;              //线程总数
   int  freenum;
   int  fenqudaxiao;
    Ui::Form *ui;              //ui
    QList<QChartView *> m_charts;  //内存使用情况饼状图
};

#endif // FORM_H
