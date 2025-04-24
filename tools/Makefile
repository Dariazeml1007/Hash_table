# Настройки компилятора
COMP = g++
CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef \
         -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
         -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ \
         -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual \
         -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security \
         -Wformat=2 -Wignored-qualifiers -Wlogical-op \
         -Wno-missing-field-initializers -Wnon-virtual-dtor \
         -Woverloaded-virtual -Wpointer-arith -Wsign-promo \
         -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel \
         -Wtype-limits -Wwrite-strings -Werror=vla \
         -D_DEBUG -D_EJUDGE_CLIENT_SIDE

# Пути и файлы (Windows-стиль)
INCLUDE_DIRS = -I..\include -I.
SOURCE_FILES = prepare_words.cpp ..\source\read_to_buffer.cpp ..\source\hash_table.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)
TARGET = prep.exe

# Правила сборки
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECT_FILES)
	@if not exist ..\bin mkdir ..\bin
	$(COMP) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(COMP) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

..\source\read_to_buffer.o: ..\source\read_to_buffer.cpp ..\source\hash_table.cpp
	$(COMP) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean_objects:
	@echo "Cleaning object files..."
	@del /Q $(subst /,\,$(OBJECT_FILES)) 2>nul || exit 0

clean: clean_objects
	@del /Q $(TARGET) 2>nul || exit 0


