
CC = g++

CFLAGS += -std=c++11
CFLAGS += -I./base -I./src -I./net

LDFLAGS += -lpthread 

LDFLAGS +=

TARGET = main.out
TARGET_SRCS = $(wildcard net/*.cpp src/*.cpp base/*.cpp *.cpp)
TARGET_OBJS = $(notdir $(patsubst %.cpp, %.o, $(TARGET_SRCS)))

all:$(TARGET)

$(TARGET):$(TARGET_SRCS)
	rm -rf *.log
	$(CC) -g -o   $@ $^ $(CFLAGS) $(LDFLAGS)
	#mv *.o ./output
	#mv $(TARGET) ./output
	#./$(TARGET)

.PHONY: clean

clean:
	rm -rf ./output/* *.o *.out *.log
