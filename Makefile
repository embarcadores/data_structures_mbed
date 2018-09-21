#GCC_TARGET_PREFIX=~/bin/gcc_arm/bin/arm-none-eabi-
GCC_TARGET_PREFIX=

CC  = $(GCC_TARGET_PREFIX)gcc
CPP = $(GCC_TARGET_PREFIX)g++
AR  = $(GCC_TARGET_PREFIX)ar
SIZE= $(GCC_TARGET_PREFIX)size
DUMP= $(GCC_TARGET_PREFIX)objdump
COPY= $(GCC_TARGET_PREFIX)objcopy

CFLAGS   = -g3 -O0 -std=c99 -Wall 
CPPFLAGS = -g3 -O0 -std=c99 -Wall 
LDFLAGS = 
LIBS =
LIBDIR = -L.
BLDDIR = ./output
SRCDIR = . 
INCDIR = -I.
APP_NAME = $(notdir $(shell pwd))
EXEC:=$(BLDDIR)/$(APP_NAME)

C_SRCS = util_queue.c util_cbuf.c util_queuehdr.c util_queuecp.c util_mpool.c main.c
CPP_SRCS = 

C_OBJS=$(C_SRCS:%.c=output/%.o)
CPP_OBJS=$(CPP_SRCS:%.cpp=output/%.o)

all: $(BLDDIR) $(EXEC) 

# building generic c files
$(BLDDIR)/%.o: %.c
	$(CC) $(INCDIR) $(CFLAGS) -c $< -o $@

# building generic cpp files
$(BLDDIR)/%.o: %.cpp
	$(CPP) $(INCDIR) $(CPPFLAGS) -c $< -o $@

# creating output directory
$(BLDDIR):
	-mkdir -p ../$(BLDDIR)
	-mkdir -p $(BLDDIR)

# generating executable
$(EXEC): $(C_OBJS) $(CPP_OBJS) 
	$(CC) $(LDFLAGS) $(C_OBJS) $(CPP_OBJS) $(LIBDIR) $(LIBS) -o $(EXEC) 
	$(SIZE) $(EXEC)
    
clean:
	-rm -fR $(BLDDIR)
