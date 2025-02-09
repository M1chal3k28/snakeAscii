CC = g++
CFLAGS_DEBUG = -g -O0 -DDEBUG
CFLAGS_RELEASE = -O2 -DNDEBUG
LDFLAGS = 

# Domyślny tryb builda
BUILD_TYPE ?= release

ifeq ($(BUILD_TYPE), debug)
    CFLAGS = $(CFLAGS_DEBUG)
    OUTPUT = program_debug.exe
else
    CFLAGS = $(CFLAGS_RELEASE)
    OUTPUT = program_release.exe
endif

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /F /Q $(OBJS) program_debug.exe program_release.exe 2>nul || exit 0
