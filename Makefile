CXX = g++
ASM = nasm

# Оптимизированные флаги компиляции
CXXFLAGS = -O3 -mavx2 -maes -msse4.2 -Iinclude -fPIC \
           -Wall -Wextra -std=c++17 -flto \
           -Wno-unused-parameter -Wno-missing-field-initializers

ASMFLAGS = -f elf64 -O3
LDFLAGS = -no-pie -lc -lstdc++ -flto

SRC_CPP = source/main.cpp source/hash_table.cpp source/read_to_buffer.cpp
OBJ_CPP = $(SRC_CPP:.cpp=.o)
OBJ_ASM = asm/search_asm.o

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ_CPP) $(OBJ_ASM)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

asm/%.o: asm/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(OBJ_CPP) $(OBJ_ASM) $(TARGET)

.PHONY: all clean
