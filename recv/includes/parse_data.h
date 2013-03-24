/*
 * 解析数据
 */

#ifndef __PARSE_DATA_H__
#define __PARSE_DATA_H__

#include "common.h"
#include <pthread.h>
#include <mysql/mysql.h>

#define TEMPERATURE  "温    度"
#define HUMIDITY     "湿    度"
#define ILIUMINATION "照    度"
#define SOIL_TEMP    "土壤温度"
#define SOIL_WATER   "土壤水分"
#define ELECTRIC     "电    量"

#define DB_NAME "db_serial"

char table_name[MAX_NAME_LEN];
unsigned char recv[MAX_BUF_LEN];

int chtoint( char c );
uint16_t strtoint(char *p, int len);
uint16_t crc(unsigned char *p, int len);
uint64_t hextoint(unsigned char *p, int len);
//void strtohex(const char *src, unsigned char *dest, int len);
void print( data_frame *frame );
void *parse_data( void *arg );

void cal_results(data_frame *frame);
void get_name(data_type *type, res_arg *res);
void cal_values(data_type *type, res_arg *res);

/* db_switch */
/* 数据库 */
#define db_name "db_serial"
char table_name[MAX_NAME_LEN];

void init_mysql( MYSQL *pdata );
void get_table_name();
int get_cur_time( char *str_time, int type );
void *db_switch_thread( void *arg );
//end

#endif
