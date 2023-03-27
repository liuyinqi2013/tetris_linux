RM=rm -rf
CP=cp -rf
CC=g++
CFLAGS= -c -Wall -g -std=c++11

TARGET=tetris
BIN_PATH=../bin
INSTALL=
BOOST_HOME=/home/bulande/boost

SUBDIRS=$(shell find ./* -type d)
INC_SUBDIRS=$(shell find ./* -type d | awk -F' ' '{print "-I"$$0}')
INC_PATH=$(INC_SUBDIRS) -I$(BOOST_HOME)/include/
LIB_PATH=
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
