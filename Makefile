CXXFLAGS = -g -Wall -std=c++14
LDLIBS = -lstdc++ -lm -lopencv_core -lopencv_imgcodecs -lopencv_highgui

CXX=g++

all: cvcolor2gray cvthreshold_otsu cvarith cvhistogram \
		cvscale cvequal \
		cvblur cvedge \
		cvshow \
		cvbinmorph

cvcolor2gray: cvcolor2gray.o kit.o

cvthreshold_otsu: cvthreshold_otsu.o kit.o

cvarith: cvarith.o kit.o

cvhistogram: cvhistogram.o kit.o

cvscale: cvscale.o kit.o

cvequal: cvequal.o kit.o

cvblur: cvblur.o kit.o

cvedge: cvedge.o kit.o

cvshow: cvshow.o kit.o

cvbinmorph: cvbinmorph.o kit.o
