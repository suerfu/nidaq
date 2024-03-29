# Makefile for compiing saber library for data acquisition with polaris

CC = g++ 

NAME = nidaq
LIBNAME = lib$(NAME).so


CPP_FILES = $(wildcard src/*.cpp h5manager/src/*.cpp) 
OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))

CFLAGS = -Wall -std=c++0x -fPIC -I./include -I/usr/local/include

# polaris files
CFLAGS += -I/usr/include/polaris
LDFLAGS += -L/usr/lib -lpolaris

# National Instruments
CFLAGS += -D_POSIX_C_SOURCE=200809L
LDFLAGS += -L/usr/lib/x86_64-linux-gnu -lm -lnidaqmx

# HDF5 flags
CFLAGS += -I/usr/local/hdf5/include
LDFLAGS += -L/usr/local/hdf5/lib -lhdf5 -lhdf5_hl -lhdf5_cpp

# HDF5 manager
CFLAGS += -I./h5manager/include

# below is for C version
# CFLAGS += -I/usr/local/hdf5/include/ -I/usr/include
# LDFLAGS += -L/usr/local/hdf5/lib -lhdf5 -lhdf5_hl -lhdf5_cpp

all : lib test


test : h5write


h5write : test/h5write.o $(OBJ_FILES)
	$(CC) -o $@ $^ ${LDFLAGS}


#test_type : test/test_type.o $(OBJ_FILES)
#	$(CC) -o $@ $^ ${LDFLAGS}


h5write.o : test/h5write.cpp
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
