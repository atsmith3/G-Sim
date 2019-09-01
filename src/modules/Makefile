INCPATH = -I. -Imemory -Icrossbar
CPPFLAGS += -std=c++11 -Wall $(INCPATH)
#CPPFLAGS += -pg 
#LFLAGS += -pg

PROG := g_sim

# simulator source files
CPP_SRCS = $(wildcard *.cpp) $(wildcard */*.cpp) 
CPP_HDRS = $(wildcard *.hpp) $(wildcard */*.hpp)
CPP_DEPS = $(CPP_SRCS:.cpp=.d)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

OBJS :=  $(CPP_OBJS)

.PHONY: clean

all: CPPFLAGS += -O3
all: $(PROG)

debug: CPPFLAGS += -O0 -g -DVERBOSE -DDEBUG
debug : $(PROG)

verbose: CPPFLAGS += -O3 -DVERBOSE -DDEBUG
verbose: $(PROG)

$(PROG): $(OBJS)
	 $(CXX) $(LFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -MMD -c $< -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(CPP_DEPS)
	rm -f $(OBJS)
	rm -f $(PROG)

-include $(CPP_DEPS)

