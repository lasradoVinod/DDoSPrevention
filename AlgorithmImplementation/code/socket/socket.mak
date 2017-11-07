SOCKET_ROOT = $(ROOT)/code/socket

SRC += ClientSocketTCP.cpp  ServerSocketTCP.cpp ClientSocketUdp.cpp  ServerSocketUdp.cpp SocketTCP.cpp \
	SocketUdp.cpp

VPATH += $(SOCKET_ROOT)/src/ 

INC += $(SOCKET_ROOT)/inc/
