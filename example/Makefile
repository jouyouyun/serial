# $@  目标文件名
# $<  第一个依赖的文件名
# $^  所有依赖的文件名


CC =  gcc
PROG_NAME = my_serial

#头文件所在目录
#INCS = ../include/

#当前目录
PWD = $(shell pwd)/

#CFLAGS = -Wall -g -lpthread -I$(INCS)
#LDFLAGS = -Wall -g -lpthread -I$(INCS)
CFLAGS = -Wall -g
LDFLAGS = -Wall -g

OBJS := ex_main.o example.o

all : ${PROG_NAME}

${PROG_NAME} : ${OBJS}
	${CC} ${CFLAGS} $^ -o $@
	
#$(COMMON)common.o:
#	cd ${COMMON}; ${CC} ${LDFLAGS} -c common.c; cd ${PWD}
	
%.o : %.c
	${CC} ${CFLAGS} -c $<
	
clean :
	rm -f *.o ${PROG_NAME}
	
rebuild : clean all
