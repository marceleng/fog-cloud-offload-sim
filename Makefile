OS := $(shell uname)
ifeq ($(OS),Darwin)
CC = clang
FIND = gfind
else
CC = gcc
FIND = find
LFLAGS = -lm
endif
CFLAGS = -g -Wall -Werror -std=gnu99
INCLUDE="."

BUILDIR = $(shell pwd)/build

SUBDIRS = caches helpers queueing

2LRU_OBJ = $(addprefix $(BUILDIR)/,2lru.o)
FOG_SIM_OBJ =  $(addprefix $(BUILDIR)/,fog_simulator.o)
6CN_OBJ = $(addprefix $(BUILDIR)/,6cn.o)

HSOURCES = $(shell find $(SUBDIRS) -type f -iname '*.c')

HOBJECTS = $(addprefix $(BUILDIR)/,$(notdir $(HSOURCES:%.c=%.o)))

.PHONY: clean create_dir

fog_sim: create_dir $(HOBJECTS) $(FOG_SIM_OBJ)
	$(CC) -I$(INCLUDE) $(FOG_SIM_OBJ) $(HOBJECTS) -o fog_simulator $(LFLAGS)

2lru: create_dir $(HOBJECTS) $(2LRU_OBJ)
	$(CC) -I$(INCLUDE) $(2LRU_OBJ) $(HOBJECTS) -o 2lru_sim $(LFLAGS)

6cn: create_dir $(HOBJECTS) $(6CN_OBJ)
	$(CC) -I$(INCLUDE) $(6CN_OBJ) $(HOBJECTS) -o ats_lru $(LFLAGS)

.SECONDEXPANSION:

$(BUILDIR)/%.o: $$(shell $(FIND) -name $$*.c)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $^

create_dir:
	mkdir -pm 777 $(BUILDIR)

clean:
	rm -fr $(BUILDIR)
