# Makefile for compiing saber library for data acquisition with polaris

CC = g++ -pg # -g

NAME = nidaq
LIBNAME = lib$(NAME).so


CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))


CFLAGS = -Wall -std=c++0x -fPIC -I./include

# polaris files
CFLAGS += -I/usr/local/include/polaris
LD_PLRSFLAGS = -L/usr/local/lib -lpolaris

# ROOT files
ROOT_FLAGS = #`root-config --cflags --glibs`

# National Instruments
CFLAGS += -D_POSIX_C_SOURCE=200809L
LD_NIFLAGS += -L/usr/lib/x86_64-linux-gnu -lm -lnidaqmx


all : lib

lib : ./lib/$(LIBNAME)

./lib/$(LIBNAME) : $(OBJ_FILES)
	@echo "linking $@"
	@mkdir -p ./lib
	$(CC) -fPIC -shared -Wl,-soname,$(LIBNAME) -o $@ $^ ${LD_PLRSFLAGS} ${LD_NIFLAGS} ${ROOT_FLAGS}

%.o : %.cpp
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $^ -o $@ ${ROOT_FLAGS}

clean:
	@echo "cleaning..."
	@-rm ${OBJ_FILES} > /dev/null 2>&1
	@-rm ./lib/${LIBNAME} > /dev/null 2>&1
