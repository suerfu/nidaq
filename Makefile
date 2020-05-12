# Makefile for compiing saber library for data acquisition with polaris

CC = g++ 

NAME = nidaq
LIBNAME = lib$(NAME).so


CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))


CFLAGS = -Wall -std=c++0x -fPIC -I./include 

# polaris files
CFLAGS += -I/usr/local/include/polaris
LDFLAGS += -L/usr/local/lib -lpolaris

# ROOT files
#ROOT_FLAGS = #`root-config --cflags --glibs`

# National Instruments
CFLAGS += -D_POSIX_C_SOURCE=200809L
LDFLAGS += -L/usr/lib/x86_64-linux-gnu -lm -lnidaqmx

# HDF5 flags
CFLAGS += -I/usr/include
LDFLAGS += -L/usr/lib64 -lhdf5 -lhdf5_hl -lhdf5_cpp
# below is for C version
# CFLAGS += -I/usr/local/hdf5/include/ -I/usr/include
# LDFLAGS += -L/usr/local/hdf5/lib -lhdf5 -lhdf5_hl -lhdf5_cpp

all : lib test_h5

test_h5 : test/test_hdf5.o $(OBJ_FILES)
	$(CC) -o $@ $^ ${LDFLAGS}

hdf5_test.o : test/test_hdf5.cpp
	@$(CC) $(CFLAGS) -c $^ -o $@

lib : ./lib/$(LIBNAME)

./lib/$(LIBNAME) : $(OBJ_FILES)
	@echo "linking $@"
	@mkdir -p ./lib
	$(CC) -fPIC -shared -Wl,-soname,$(LIBNAME) -o $@ $^ ${LDFLAGS}

%.o : %.cpp
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $^ -o $@

clean:
	@echo "cleaning..."
	@-rm ${OBJ_FILES} > /dev/null 2>&1
	@-rm test/*.o > /dev/null 2>&1
	@-rm ./lib/${LIBNAME} > /dev/null 2>&1
