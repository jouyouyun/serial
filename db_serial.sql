drop database db_serial;
create database db_serial;

use db_serial;
create table manager_table
(
	id int auto_increment not null primary key,
	TableName varchar(20),
	CreateTime datetime
);