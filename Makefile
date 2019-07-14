
CC = g++

CFLAGS += -std=c++11
CFLAGS += -I./base -I./src

LDFLAGS = 

LDFLAGS +=

TARGET = main
TARGET_SRCS = $(wildcard src/*.cpp base/*.cpp *.cpp)
TARGET_OBJS = $(notdir $(patsubst %.cpp, %.o, $(TARGET_SRCS)))

all:$(TARGET)

$(TARGET):$(TARGET_SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	#mv *.o ./output
	#mv $(TARGET) ./output
	./$(TARGET)

.PHONY: clean

clean:
	rm -rf ./output/* *.o main




