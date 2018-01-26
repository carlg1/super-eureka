PROG     := tbdflow
BIT      := -m64

CXX      := g++
CXXFLAGS := ${BIT} -Wall -O3
LDFLAGS  := ${BIT} 
LIBS     :=

HDRS     := $(wildcard *.h)
SRCS     := $(wildcard *.cpp)
OBJS     := $(patsubst %.cpp,%.o,${SRCS})

all: ${OBJS}
	${CXX} ${LDFLAGS} ${OBJS} -o ${PROG}

%.o: %.cpp ${HDRS}
	${CXX} ${CXXFLAGS} -c $< -o $@

.PHONY(clean):
clean:
	@rm -f *.o ${PROG}


.PHONY(clobber):
clobber: clean
	@rm -f *~
