#include "logindialog.h"
#include "ui_logindialog.h"
QString userName;
QString password;
bool isAdmin;
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->password->setEchoMode(QLineEdit::Password);//输入时显示数字，失去焦点时显示圆点
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButton_clicked() //管理员登录
{
    userName = ui->userName->text();
    password = ui->password->text();
    isAdmin = true;
}

void LoginDialog::on_pushButton_visitor_clicked()  //游客直接访问
{
    isAdmin=false;
}
