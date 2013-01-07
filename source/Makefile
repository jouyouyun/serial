# $@  目标文件名
# $<  第一个依赖的文件名
# $^  所有依赖的文件名


CC =  gcc
PROG_NAME = modbus

CFLAGS  = -Wall -g
LDFLAGS = -Wall -g

OBJS := modbus.o modbus_crc.o

all : ${PROG_NAME}

${PROG_NAME} : ${OBJS}
	${CC} ${CFLAGS} $^ -o $@
	
%.o : %.c
	${CC} ${CFLAGS} -c $<

clean :
	rm -f *.o ${PROG_NAME}
	
rebuild : clean all
