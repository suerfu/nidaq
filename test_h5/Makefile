
CC = g++ 

NAME = grp_write
NAME = ds_write
#NAME = h5_write
#NAME = bin_write

CPP_FILES = $(wildcard *.cpp)
OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))


CFLAGS = -Wall -std=c++0x -fPIC -I./include

# polaris files
CFLAGS += -I/usr/local/include/polaris
LDFLAGS = -L/usr/local/lib -lpolaris

# National Instruments
CFLAGS += -D_POSIX_C_SOURCE=200809L
LDFLAGS += -L/usr/lib/x86_64-linux-gnu -lm -lnidaqmx


# National Instruments
CFLAGS += -I/usr/local/hdf5/include/
LDFLAGS += -L/usr/local/hdf5/lib -lhdf5 -lhdf5_hl


all : $(NAME)

$(NAME) : $(NAME).o
	$(CC) -o $@ $^ ${LDFLAGS}

%.o : %.cpp
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $^ -o $@

clean:
	@echo "cleaning..."
	@-rm ${OBJ_FILES} > /dev/null 2>&1
	@-rm ${NAME} > /dev/null 2>&1

