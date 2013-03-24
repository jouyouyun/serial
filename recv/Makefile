# $@  目标文件名
# $<  第一个依赖的文件名
# $^  所有依赖的文件名


CC =  gcc
PROG_NAME = main

#当前目录
PWD = $(shell pwd)/

#头文件所在目录
INCS := ${PWD}includes/

LIBS := /usr/lib/mysql/

SRC_PATH := ${PWD}src/

vpath %.c ${SRC_PATH}

CFLAGS  = -Wall -g -I${INCS} -L${LIBS} -lpthread -lm -lmysqlclient
LDFLAGS = -Wall -g -I${INCS} -L${LIBS} -lpthread -lm -lmysqlclient

OBJS :=  main.o my_serial.o set_port.o parse.o switch_sql.o

all : ${PROG_NAME}

${PROG_NAME} : ${OBJS}
	${CC} ${CFLAGS} $^ -o $@

%.o : %.c
	${CC} ${CFLAGS} -c $<
	
clean :
	rm -f *.o ${PROG_NAME}
	
rebuild : clean all
