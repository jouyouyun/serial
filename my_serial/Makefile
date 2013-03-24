# $@  目标文件名
# $<  第一个依赖的文件名
# $^  所有依赖的文件名


CC =  gcc
PROG_SEND = send_serial
PROG_RECV = recv_serial

#头文件所在目录
#INCS = ../include/

#当前目录
PWD = $(shell pwd)/

#CFLAGS = -Wall -g -lpthread -I$(INCS)
#LDFLAGS = -Wall -g -lpthread -I$(INCS)
CFLAGS  = -Wall -g -lpthread -lm
LDFLAGS = -Wall -g -lpthread -lm

OBJS := my_serial.o set_port.o parse.o
OBJS_SEND := send_main.o
OBJS_RECV := recv_main.o

all : ${PROG_SEND} ${PROG_RECV}

${PROG_SEND} : ${OBJS} ${OBJS_SEND}
	${CC} ${CFLAGS} $^ -o $@

${PROG_RECV} : ${OBJS} ${OBJS_RECV}
	${CC} ${CFLAGS} $^ -o $@
	
%.o : %.c
	${CC} ${CFLAGS} -c $<
	
clean :
	rm -f *.o ${PROG_SEND} ${PROG_RECV}
	
rebuild : clean all
