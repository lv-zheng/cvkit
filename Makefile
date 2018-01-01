CXXFLAGS = -g -Wall -std=c++14
LDLIBS = -lstdc++ -lm -lopencv_core -lopencv_imgcodecs

CXX=g++

all: cvcolor2gray

cvcolor2gray: cvcolor2gray.o kit.o
