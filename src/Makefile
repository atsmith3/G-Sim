INCPATH = -I. -Imodules/memory -Imodules/crossbar -Imodules -Iutil/mm_io -Iutil -IgraphMat -Imodules/memory/DRAMSim2
CPPFLAGS += -std=c++17 -Wall -Wfatal-errors -Werror $(INCPATH)
CFLAGS += -Wall $(INCPATH)

ifdef DRAMSIM2
CPPFLAGS += -DDRAMSIM2
endif
ifdef MODULE_TRACE
CPPFLAGS += -DMODULE_TRACE
endif
#CPPFLAGS += -pg 
LFLAGS += -lboost_program_options -lstdc++fs -lrt -ldramsim
LPATH = -L/usr/local/bin -L./modules/memory/DRAMSim2

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

bfs: CPPFLAGS += -O3 -DAPP_BFS -DVERBOSE -DDEBUG
bfs: $(PROG)

cc: CPPFLAGS += -O3 -DAPP_CC -DVERBOSE -DDEBUG
cc: $(PROG)

sssp: CPPFLAGS += -O3 -DAPP_SSSP -DVERBOSE -DDEBUG
sssp: $(PROG)

pr: CPPFLAGS += -O3 -DAPP_PR -DVERBOSE -DDEBUG
pr: $(PROG)

debug: CPPFLAGS += -O0 -g -DVERBOSE -DDEBUG -DAPP_BFS
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

