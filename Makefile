SRC_DIR="./src"
OBJ_DIR="./obj"
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

INC="$(SRC_DIR)/inc"
UTIL="$(SRC_DIR)/utils"

INCFLAGS=-I$(INC)
UTILFLAGS=-I$(UTIL)
GLFLAGS=-lGL -lglfw -lGLEW -lGLU
CC=g++
NVCPP=nvc++

all: Serial Cuda
Serial: $(SRC_DIR)/serial.cpp $()
	$(NVCPP) $(INCFLAGS) $(UTILFLAGS) $(GLFLAGS) src/serial.cpp -o bin/Serial

$(OBJ_DIR)/%.o: $(UTIL)/%.cpp
   $(NVCPP) $(INCFLAGS) $(UTILFLAGS) $(GLFLAGS) -c -o $@ $<


Cuda: cuda.cu
	$(NVCC) $(INCFLAGS)  cuda.cu -o bin/Cuda

clean:
	rm bin/Serial  bin/Cuda
