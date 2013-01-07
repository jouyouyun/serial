/*
 * 数据库自动切换
 * 1、连接 MySQL 数据库
 * 2、获得当前日期，并与当前操作的数据表比较，判断是否存在变化
 * 3、若存在变化，则开始执行数据库的自动切换功能，一当前日期创建新表
 * 新表示例：
 * create table xx_xx_xx (id int auto_increment not null primary key, 
 *					CollectTime datetime, DestID varchar(15) not null, 
 *					temperature int not null);
 */

#include "my_serial.h"
#include <time.h>

void
init_mysql ( MYSQL *pdata )
{
	if ( !mysql_init(pdata) ) {
		perror( "mysql_init" );
		return;
	}
	
	/*
	if ( !mysql_real_connect( pdata, IP, "root", "root",
				"db_serial", Port, NULL, 0 ) )
	*/
	if ( !mysql_real_connect( pdata, "localhost", "root", "root",
				db_name, 0, NULL, 0 ) ) {
		//mysql_close(pdata);
		perror( "mysql_real_connect" );
		return;
	}
	fprintf( stdout, "Init MySQL Successfully!\n" );
	if ( mysql_select_db(pdata, db_name) != 0 ) {
		//mysql_close(pdata);
		perror( "mysql_select_db" );
		return;
	}
}

void
get_table_name ()
{
	MYSQL pdata;
	int i, j;
	char table[30] = "0";
	char str_sql[] = "select TableName from manager_table where id in \
					  ( select max(id) from manager_table )";
	MYSQL_RES *res;
	MYSQL_ROW row;

	init_mysql(&pdata);
	
	i = mysql_real_query( &pdata, str_sql, strlen(str_sql) );
	if ( i ) {
		printf( "Query Err: %s\n", mysql_error(&pdata) );
		mysql_close(&pdata);
		return;
	}
	printf( "Query made......\n" );

	res = mysql_use_result( &pdata );
	if ( !res ) {
		printf( "Result Err: %s\n", mysql_error(&pdata) );
		mysql_close(&pdata);
		return;
	}

	/*
	 * mysql_field_count 返回查询的列的数量
	 */
	for ( i = 0; i < mysql_field_count(&pdata); i++ ) {
		/*
		 * mysql_fetch_row 检索一个结果集合的下一行
		 */
		row = mysql_fetch_row( res );
		if ( row == NULL ) {
			break;
		}

		/*
		 * mysql_num_fields 返回集合中行的数量
		 */
		for ( j = 0; j < mysql_num_fields(res); j++ ) {
			sprintf( table, "%s", row[j] );
		}
	}
	memset( table_name, 0, MAX_NAME_LEN );
	memcpy( table_name, table, strlen(table) );
	printf( "table_name: %s\n", table_name );
	
	mysql_free_result(res);
	mysql_close(&pdata);
}

void *
db_switch_thread( void *arg )
{
	char str_time[20];
	char str_sql[300];
	char str_create_time[30];
	MYSQL pdata;
	
	init_mysql(&pdata);

	fprintf( stdout, "数据库切换已开始...\n" );
	while (1) {
		get_cur_time( str_time, 2 );
		printf( "str_time: %s\n", str_time );
		if ( strncmp( str_time, table_name, strlen(str_time) ) != 0 ) {
			get_cur_time( str_create_time, 1 );
			//写入数据库的主控表，同时创建新表
			sprintf( str_sql, "insert into manager_table (CreateTime, TableName) \
					values ( '%s', '%s' )", str_create_time, str_time );

			if ( mysql_real_query(&pdata, str_sql, strlen(str_sql)) != 0 ) {
				fprintf( stderr, "Error : %s\n", str_sql );
				//mysql_close(pdata);
				//return;
				break;
			}
			fprintf( stdout, "创建新记录成功 (%s)\n", str_time );

			sprintf( str_sql, "create table %s (id int auto_increment not null \
				primary key, CollectTime datetime, ", str_time );
			sprintf( str_sql, "%sDestID varchar(15) not null, temperature int \
					not null)", str_sql );

			if ( mysql_real_query( &pdata, str_sql, strlen(str_sql) ) != 0 ) {
				fprintf( stderr, "Error : %s\n", str_sql );
				//mysql_close(pdata);
				//return;
				break;
			}
			fprintf( stdout, "创建新表成功 (%s)\n", str_time );

			strcpy( table_name, str_time );
		} else {
			fprintf( stdout, "数据表已创建...\n" );
		}
		sleep(5);
	}

	mysql_close(&pdata);
}

int
get_cur_time( char *str_time, int type )
{
	time_t t;
	struct tm *tm = NULL;

	time(&t);
	if ( t == -1 ) {
		return -1;
	}

	tm = localtime( &t );
	if ( tm == NULL ) {
		return -1;
	}

	switch (type) {
		case 0:
			sprintf( str_time, "系统运行时间位：%d-%d-%d %d:%d:%d\n", 
					tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, 
					tm->tm_hour, tm->tm_min, tm->tm_sec );
			break;
		case 1:
			sprintf( str_time, "%d-%d-%d %d:%d:%d", 
					tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, 
					tm->tm_hour, tm->tm_min, tm->tm_sec );
			break;
		case 2:
			sprintf( str_time, "%d_%d_%d", tm->tm_year + 1900, 
					tm->tm_mon + 1, tm->tm_mday );
			break;
		default :
			return -1;
	}

	return 0;
}

/*
int
main( int argc, char *argv[] )
{
	char str[30] = "0";

	get_table_name();
	if ( get_cur_time( str, 0 ) == -1 ) {
		fprintf( stderr, "get_cur_time err\n" );
		return -1;
	}

	db_switch();
	fprintf( stdout, "数据库切换已结束...\n" );

	return 0;
}
*/
