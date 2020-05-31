#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("设置初始内存模型"));
    connect(this->ui->pushButton,SIGNAL(clicked()),this,SLOT(start()));  //连接槽函数
}

Widget::~Widget()
{
    delete ui;
    delete win;  //释放资源
}

void Widget :: start (){
    this->hide();
    win=new Form();
    win->show();
    win->init(this->ui->spinBox->value(),this->ui->spinBox_4->value(),this->ui->spinBox_3->value(),this->ui->spinBox_2->value());
}
