ROOT = $(shell pwd)

OBJDIR = $(ROOT)/obj
CC = g++

CXFLAGS = -Wall -Wconversion -std=c++11 -g

# Generate dependency information
CXFLAGS += -MD -MP -MF .dep/$(@F).d

LIBDIR = 
LIB = event pthread
SRC = 
VPATH = . 
INC = 
DEBU = CURR_LEVEL=3
#*************Including UTILS**************************
include $(ROOT)/code/utils/utils.mak
#******************************************************

#*************Including UTILS**************************
include $(ROOT)/code/socket/socket.mak
#******************************************************

#*************Including UTILS**************************
include $(ROOT)/code/algo/algo.mak
#******************************************************

OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRC)))
LIBDIRS = $(patsubst %,-L%,$(LIBDIR))
LIBS = $(patsubst %,-l%,$(LIB))
INCS  = $(patsubst %,-I%,$(INC))
DEBUG = $(patsubst %,-D%,$(DEBU))

.PHONY: clean

all: edgeMain SNMain

edgeMain: $(OBJS) $(OBJDIR)/edgeMain.o 
	$(CC) $(CXFLAGS) $(OBJS) $(DEBUG) $(OBJDIR)/edgeMain.o $(INCS) $(LIBDIRS) -o edgeMain $(LIBS)

SNMain: $(OBJS) $(OBJDIR)/SNMain.o 
	$(CC) $(CXFLAGS) $(OBJS) $(DEBUG) $(OBJDIR)/SNMain.o $(INCS) $(LIBDIRS) -o SNMain $(LIBS)
	
debug:
	@echo $(VPATH)
clean:
	rm $(ROOT)/obj/* SNMain edgeMain

$(OBJDIR)/%.o: %.cpp
	@echo Compiling $@ $< ...
	mkdir -p $(OBJDIR)
	$(CC) -c $(CXFLAGS) $(INCS) $(DEBUG) $(LIBDIRS) $< -o $@ $(LIBS)

# Include the dependency files
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
