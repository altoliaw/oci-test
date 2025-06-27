# ===== Makefile for OCILib Model =====

CXX = g++
CXXFLAGS = -std=c++17 \
    -I../../Headers \
    -I../Headers \
    -I/usr/local/include \
    -I/usr/include/oracle/19.10/client64

LDFLAGS =
LDLIBS =

SRC = Source/OcilibModel.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = OcilibModel.o

all: $(OBJ)

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
