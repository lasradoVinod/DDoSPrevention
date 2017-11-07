ROOT = $(shell pwd)

OBJDIR = $(ROOT)/obj
CC = g++

CXFLAGS = -Wall -Wconversion -O3 -std=c++11

# Generate dependency information
CXFLAGS += -MD -MP -MF .dep/$(@F).d

LIBDIR = 
LIB = 
SRC = ClientSocket.cpp \
	ServerSocket.cpp \
	Socket.cpp \
	exception.cpp \
	log.cpp 

INC = .

OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRC)))
LIBDIRS = $(patsubst %,-L%,$(LIBDIR))
LIBS = $(patsubst %,-l%,$(LIB))
INCS  = $(patsubst %,-I%,$(INC))

.PHONY: clean

all: sink bulk onOff random

sink: $(OBJS) $(OBJDIR)/sink.o 
	$(CC) $(CXFLAGS) $(OBJS) $(OBJDIR)/sink.o $(INCS) $(LIBDIRS) -o sink $(LIBS)

bulk: $(OBJS) $(OBJDIR)/bulkSend.o 
	$(CC) $(CXFLAGS) $(OBJS) $(OBJDIR)/bulkSend.o $(INCS) $(LIBDIRS) -o bulk $(LIBS)

onOff: $(OBJS) $(OBJDIR)/onOffSend.o 
	$(CC) $(CXFLAGS) $(OBJS) $(OBJDIR)/onOffSend.o $(INCS) $(LIBDIRS) -o onOff $(LIBS)

random: $(OBJS) $(OBJDIR)/randomSend.o 
	$(CC) $(CXFLAGS) $(OBJS) $(OBJDIR)/randomSend.o $(INCS) $(LIBDIRS) -o random $(LIBS)
		
debug:
	@echo $(VPATH)
clean:
	rm $(ROOT)/obj/* sink bulk onOff random
	rm -rf .dep

$(OBJDIR)/%.o: %.cpp
	@echo Compiling $@ $< ...
	mkdir -p $(OBJDIR)
	$(CC) -c $(CXFLAGS) $(INCS) $(LIBDIRS) $< -o $@ $(LIBS)

# Include the dependency files
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
