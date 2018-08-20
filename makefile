RM=rm -rf
CP=cp -rf
CC=g++
CFLAGS= -c -Wall -g

TARGET=tetris
BIN_PATH=../bin
INSTALL=

SUBDIRS=$(shell find . -type d)
INC_SUBDIRS=$(shell find . -type d | awk -F' ' '{print "-I"$$0}')
INC_PATH=$(INC_SUBDIRS) -I/usr/local/boost/include/ -I/usr/local/log4cpp/include/
LIB_PATH=/usr/lib64/mysql/libmysqlclient.a /usr/local/boost/lib/libboost_thread.a /usr/local/boost/lib/libboost_system.a /usr/local/log4cpp/lib/liblog4cpp.a
LIB= -lpthread -ldl -lz

C_SRC=$(shell find . -type f | grep "\.c$$" | grep -v grep)
#C_SRC=$(notdir $(C_SRC_NAME)) 
C_OBJ=$(patsubst %.c, %.o, $(C_SRC))
CPP_SRC=$(shell find . -type f | grep "\.cpp$$" | grep -v grep)
#CPP_SRC=$(notdir $(CPP_SRC_NAME)) 
CPP_OBJ=$(patsubst %.cpp, %.o, $(CPP_SRC))
OBJS=$(C_OBJ) $(CPP_OBJ)

VPATH=$(SUBDIRS)
#---------------------------------------------------------------------------------------------

.PHONY:all
	
all:$(TARGET)
	
$(TARGET):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LIB_PATH) $(LIB)
$(CPP_OBJ):%.o:%.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $< -o $@
$(C_OBJ):%.o:%.c
	$(CC) $(CFLAGS) $(INC_PATH) $< -o $@

install:
	
clean:
	$(RM) $(TARGET) $(TARGET1) $(OBJS)
