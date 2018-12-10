CC = gcc -pg -Wall
CXX = g++ -pg -Wall

.c.o: 
	$(CC) -c $<

.cc.o:
	$(CXX) -c $<

PARSER = getmodule.o
PARSER += writemodule.o
PARSER += inputparse.o
PARSER += hash.o
PARSER += global.o
PARSER += util.o

PARSER += fm.o

PART = pmain.o

install:
	make part

inputparse.o: 	standard.h inputparse.c
global.o: 		standard.h global.c
pmain.o:		standard.h pmain.cpp

part: $(PARSER) $(PART) 
	$(CXX) -o part $(PARSER) $(PART) -lm
	cp part ../test

clean:
	rm -rf $(PARSER) $(PART) part
