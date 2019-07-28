CC = gcc
C++ = g++

CFLAGS = $(COMPILER_FLAGS) -c -g -fPIC
CXXFLAGS = $(COMPILER_FLAGS) -c -g -fPIC



SOURCEDIR = src/
COMMONEDIR      =       common/
COLLECTORDIR    =       collector/
AGENTDIR        =       agent/
CPP_DIR         =       cpp_dir/

OBJFILE = $(CFILES:.c=.o) $(C++FILES:.cpp=.o)

CFILES = $(wildcard $(COMMONEDIR)*.c $(COLLECTORDIR)*.c $(AGENTDIR)*.c)
C++FILES = $(wildcard $(CPP_DIR)*.cpp)


CC_OBJS := $(patsubst %.c,%.o,$(CFILES))

CPP_OBJS := $(patsubst %.cpp,%.o,$(C++FILES))

OBJFILE = $(CC_OBJS) $(CPP_OBJS)
TESTSOURCEDIR   = src/
TESTSOURCES = $(wildcard $(TESTSOURCEDIR)*.c)
TESTOBJECTS = $(patsubst %.c,%.o,$(TESTSOURCES))

DEBUG ?= 1
ifeq ($(DEBUG), 1)
        BUILD_TYPE=debug
        PROG_FLAGS=-g -ggdb -Wall -Werror -DDEBUG -D_GNU_SOURCE -std=gnu99 -lpthread -Wno-unknown-pragmas
        PROG_LDFLAGS= -O0
else
        BUILD_TYPE=release
        PROG_FLAGS=-D_GNU_SOURCE -Wall -Werror -lpthread
        PROG_LDFLAGS= -O0 -s
endif

CFLAGS := $(CFLAGS) $(PROG_FLAGS) -pthread
CXXFLAGS := $(CXXFLAGS)
LDFLAGS :=  $(PROG_LDFLAGS)

all:clean $(TARGET) $(AR_TARGET) $(SO_TARGET)
	@echo build success!!!

WINDOWS ?= 0
ifeq ($(WINDOWS), 1)
$(TARGET) :  $(TESTOBJECTS) $(OBJECTS)
        $(CC) $(LDFLAGS) -o $@ $(OBJFILE) $(TESTOBJECTS) -lws2_32
else
$(TARGET) :  $(TESTOBJECTS) $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJFILE) $(TESTOBJECTS) -lpthread -lz -lstdc++ -lm
endif


$(SO_TARGET): $(OBJFILE)
#	$(CC) $^  -std=c99 -g -Wall -fPIC -shared -o $@  $^
	$(CC) -o $@ -shared -fPIC $(OBJFILE)
$(AR_TARGET) : $(OBJFILE)
	$(AR) -r $@ $(OBJFILE)

#$(CC_OBJS) : %.o : $(CFILES)
#	$(CC) $(CCFLAGS) $(INCS) -c $<

#$(CPP_OBJS) : %.o : $(C++FILES)
#	$(C++) $(C++FLAGS) $(INCS) -c $<

#install:
#	tsxs -i -o $(TARGET)

.PHONY : clean
clean:
	rm -rf $(TARGET) $(SO_TARGET) $(AR_TARGET) $(OBJFILE) $(TESTOBJECTS)
launch:
        $(TARGET)
bulid-common:
	@cd $(COMMONEDIR) && $(MAKE) -f common.mk

clean-common:
	@cd $(COMMONEDIR) && $(MAKE) -f common.mk clean

