# ===== Makefile for OcilibController =====

CXX = g++
CXXFLAGS = -std=c++17 \
    -I../../../../Headers \
    -I/usr/local/include \
    -I/usr/include/oracle/19.10/client64

LDFLAGS =
LDLIBS =

SRC = OcilibController.cpp
OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
