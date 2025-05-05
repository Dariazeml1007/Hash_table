CFLAGS= -g -std=c++17 -mavx2 -O2 -DUSE_CALLGRIND -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE
#
COMP=g++

INCLUDE=include
EXECUTABLE=main

SOURCES=$(wildcard source/*.cpp  )
OBJECTS=$(wildcard *.o)

all:
	$(COMP) $(CFLAGS) $(SOURCES) -I $(INCLUDE) -o $(EXECUTABLE)

obj:
	$(COMP) $(CFLAGS) -c $(SOURCES) -I $(INCLUDE)

link:
	$(COMP) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)
	clean_obj

clean:
	del *.exe

clean_obj:
	del *.o

.PHONY:
	clean clean_obj all link
