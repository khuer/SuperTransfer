.PHONY:all clean
CC     :=g++
AT     :=@
OUTPATH:=out
TARGET :=$(OUTPATH)/main.exe
OBJS   :=$(OUTPATH)/main.o
INCLUDE:=-I./
VPATH  :=src
OFLAG=-lwsock32
SRC=	main.cpp	\
		log.cpp		\
		network.cpp	\
		networkProtocol.cpp	\


all:$(TARGET)

$(TARGET):$(SRC:.cpp=.o)
	$(CC) $^ -o $@ $(OFLAG)

clean:
	-rm *.o *.d $(TARGET)

