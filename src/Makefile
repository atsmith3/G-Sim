INCPATH = -I. -Imodules/memory -Imodules/crossbar -Imodules -Iutil
CPPFLAGS += -std=c++11 -Wall $(INCPATH)
#CPPFLAGS += -pg 
LFLAGS += -lboost_program_options
LPATH = -L/usr/local/bin

PROG := g_sim

# simulator source files
CPP_SRCS = $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp)
CPP_HDRS = $(wildcard *.hpp) $(wildcard */*.hpp) $(wildcard */*/*.hpp)
CPP_DEPS = $(CPP_SRCS:.cpp=.d)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

OBJS :=  $(CPP_OBJS)

.PHONY: clean

all: CPPFLAGS += -O0
all: $(PROG)

debug: CPPFLAGS += -O0 -g -DVERBOSE -DDEBUG
debug : $(PROG)

verbose: CPPFLAGS += -O3 -DVERBOSE -DDEBUG
verbose: $(PROG)

$(PROG): $(OBJS)
	 $(CXX) $^ $(LPATH) $(LFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -MMD -c $< -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(CPP_DEPS)
	rm -f $(OBJS)
	rm -f $(PROG)

tidy:
	rm -f $(CPP_DEPS)
	rm -f $(OBJS)

-include $(CPP_DEPS)

