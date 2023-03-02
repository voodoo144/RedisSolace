#set environment variable RM_INCLUDE_DIR to the location of redismodule.h
ifndef RM_INCLUDE_DIR
	RM_INCLUDE_DIR=../redismodule/6.2/
endif

ifndef RMUTIL_LIBDIR
	RMUTIL_LIBDIR=../rmutil
endif

# find the OS
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

# Compile flags for linux / osx
ifeq ($(uname_S),Linux)
	SHOBJ_CFLAGS ?=  -fno-common -g -ggdb
	SHOBJ_LDFLAGS ?= -shared -Bsymbolic
else
	SHOBJ_CFLAGS ?= -dynamic -fno-common -g -ggdb
	SHOBJ_LDFLAGS ?= -bundle -undefined dynamic_lookup
endif
CFLAGS = -I$(RM_INCLUDE_DIR) -Wall -g -fPIC -lc -lm -std=gnu99  
CC=gcc

OS_LD_FLAGS :=
ifeq ($(OS),Windows_NT)
	@echo "Not supporting builds on Windows"
	exit 1
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OS_LD_FLAGS:= -lrmutil -lc
	endif
	ifeq ($(UNAME_S),Darwin)
		OS_LD_FLAGS:= -lrmutil -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -lSystem
	endif
endif

all: rmutil redissolace.so

rmutil: FORCE
	$(MAKE) -C $(RMUTIL_LIBDIR)

redissolace.so: redissolace.o
	$(LD) -o $@ redissolace.o $(SHOBJ_LDFLAGS) $(LIBS) -L$(RMUTIL_LIBDIR) $(OS_LD_FLAGS)

clean:
	rm -rf *.xo *.so *.o

done:
	@echo "All done"

FORCE:
