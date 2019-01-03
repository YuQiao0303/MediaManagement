#ifndef CONNECTION_H
#define CONNECTION_H
#include <QSqlDatabase>
#include <QSqlQuery>

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("database.db");
    if(!db.open()) return false;
    QSqlQuery query;

    //query.exec("create table media (id int primary key, name vchar ,path vchar)");
    //query.exec("insert into media values (1,'test.txt','./')");
    //query.exec("insert into media values (2,'whatisgoingon.txt','./')");
    query.exec("create table media (id int primary key, name vchar ,path vchar,title vchar,artist vchar, album vchar,year vchar)");
    //query.exec("update media set path = name");

    query.exec("create table admin (id int primary key, userName vchar ,password vchar)");
    query.exec("insert into admin values (1,'admin','admin')");
    return true;
}
#endif // CONNECTION_H
