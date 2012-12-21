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
CFLAGS = -Wall -g
LDFLAGS = -Wall -g

OBJS := my_serial.o set_port.o

all : ${PROG_SEND} ${PROG_RECV}

${PROG_SEND} : ${OBJS} send_main.o
	${CC} ${CFLAGS} $^ -o $@

${PROG_RECV} : ${OBJS} recv_main.o
	${CC} ${CFLAGS} $^ -o $@
	
#$(COMMON)common.o:
#	cd ${COMMON}; ${CC} ${LDFLAGS} -c common.c; cd ${PWD}
	
%.o : %.c
	${CC} ${CFLAGS} -c $<

send_main.o : send_main.c
	${CC} ${CFLAGS} -c $<

recv_main.o : recv_main.c
	${CC} ${CFLAGS} -c $<
	
clean :
	rm -f *.o ${PROG_SEND} ${PROG_RECV}
	
rebuild : clean all
