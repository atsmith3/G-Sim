INCPATH = -I. -Imodules/memory -Imodules/crossbar -Imodules -Iutil/mm_io -Iutil
CPPFLAGS += -std=c++17 -Wall $(INCPATH)
CFLAGS += -Wall $(INCPATH)
#CPPFLAGS += -pg 
LFLAGS += -lboost_program_options -lstdc++fs -lrt
LPATH = -L/usr/local/bin

PROG := g_sim

# simulator source files
CPP_SRCS = $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp)
CPP_HDRS = $(wildcard *.h) $(wildcard */*.h) $(wildcard */*/*.h)
CPP_DEPS = $(CPP_SRCS:.cpp=.d)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

C_SRCS = $(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
C_HDRS = $(wildcard *.h) $(wildcard */*.h) $(wildcard */*/*.h)
C_DEPS = $(C_SRCS:.c=.d)
C_OBJS = $(C_SRCS:.c=.o)

OBJS :=  $(CPP_OBJS) $(C_OBJS)

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
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(CPP_DEPS)
	rm -f $(C_DEPS)
	rm -f $(OBJS)
	rm -f $(PROG)

tidy:
	rm -f $(CPP_DEPS)
	rm -f $(C_DEPS)
	rm -f $(OBJS)

-include $(CPP_DEPS) $(C_DEPS)

