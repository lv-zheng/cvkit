CXXFLAGS = -g -Wall -std=c++14
LDLIBS = -lstdc++ -lm -lopencv_core -lopencv_imgcodecs

CXX=g++

all: cvcolor2gray cvthreshold_otsu cvarith cvhistogram \
		cvscale

cvcolor2gray: cvcolor2gray.o kit.o

cvthreshold_otsu: cvthreshold_otsu.o kit.o

cvarith: cvarith.o kit.o

cvhistogram: cvhistogram.o kit.o

cvscale: cvscale.o kit.o
