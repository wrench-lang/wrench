DEPS_RUNTIME = 
DEPS_COMPILER = -l:$(MINOR_NAME)
CFLAGS += -std=c11 -Wall -Wextra -Wconversion -pedantic -ftrapv -Wshadow -Wundef -Wcast-align -Wunreachable-code -Wno-switch -fstack-protector -fno-common -D_FORTIFY_SOURCE=2

ifdef NODEBUG
	CFLAGS += -O1 -Os -s
else
	CFLAGS += -ggdb -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer
endif

ifdef USE_MALLOC
	CFLAGS += -D USE_MALLOC=1
endif


CFLAGS += -I ./src
CFLAGS_RUNTIME += $(CFLAGS) -isystem ./deps/map/src -I ./deps/mem-pool/include
CFLAGS_COMPILER = $(CFLAGS)
CFLAGS_TEST = $(CFLAGS) $(CFLAGS_RUNTIME) -I . -I ./deps/ctest

SRC_RUNTIME = $(shell find src/runtime -name "*.c") $(shell find deps/mem-pool/src -name "*.c") $(shell find deps/map/src -name "*.c") 
SRC_COMPILER = $(shell find src/compiler -name "*.c")
TEST_SRC = $(shell find test -name "*.c") $(SRC_RUNTIME) $(shell find src/compiler -name "*.c" -not -name "main.c")

TARGET_RUNTIME = wrench-runtime
TARGET_COMPILER = wrench
TEST_TARGET = wrench-test

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 0

RUNTIME_OBJ = lib$(TARGET_RUNTIME).so
SONAME = $(RUNTIME_OBJ).$(VERSION_MAJOR)
MINOR_NAME = $(SONAME).$(VERSION_MINOR)
FULL_NAME_RUNTIME = $(MINOR_NAME).$(VERSION_PATCH)
FULL_NAME_COMPILER = wrench

INCLUDE_DIR = /usr/local/include/wrench
RUNTIME_DIR = /usr/local/lib
BIN_DIR = /usr/local/bin

DIR_BUILD=./build

OBJECTS_RUNTIME = $(patsubst %.c, $(DIR_BUILD)/runtime/%.o, $(SRC_RUNTIME))
OBJECTS_COMPILER = $(patsubst %.c, $(DIR_BUILD)/compiler/%.o, $(SRC_COMPILER))
TEST_OBJECTS = $(patsubst %.c, $(DIR_BUILD)/test/%.o, $(TEST_SRC))

default: install-compiler

-include $(shell find $(DIR_BUILD) -name "*.d")


$(DIR_BUILD)/runtime/%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS_RUNTIME) -MMD -c -fpic $< -o $@

$(DIR_BUILD)/compiler/%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS_COMPILER) -MMD -c -fpic $< -o $@

$(DIR_BUILD)/test/%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS_TEST) -MMD -c -fpic $< -o $@

.PRECIOUS: $(TARGET_RUNTIME) $(OBJECTS_RUNTIME) $(TARGET_COMPILER) $(OBJECTS_COMPILER)


$(TARGET_RUNTIME): $(OBJECTS_RUNTIME)
	$(CC) $(CFLAGS_RUNTIME) -shared -fpic -Wl,-soname,$(SONAME) -o $(FULL_NAME_RUNTIME) $(OBJECTS_RUNTIME) $(DEPS_RUNTIME)

$(TARGET_COMPILER): $(OBJECTS_COMPILER)
	$(CC) $(CFLAGS_COMPILER)  -o $(FULL_NAME_COMPILER) $(OBJECTS_COMPILER) $(DEPS_COMPILER)


$(TEST_TARGET): install-runtime $(TEST_OBJECTS)
	$(CC) $(CFLAGS_TEST) $(TEST_OBJECTS) -o $@

clean:
	rm -rf $(DIR_BUILD)/*

install-runtime: $(TARGET_RUNTIME)
	sudo mv $(FULL_NAME_RUNTIME) $(RUNTIME_DIR)
	ln -sf $(RUNTIME_DIR)/$(FULL_NAME_RUNTIME) $(RUNTIME_DIR)/$(MINOR_NAME)
	ln -sf $(RUNTIME_DIR)/$(MINOR_NAME) $(RUNTIME_DIR)/$(SONAME)
	ln -sf $(RUNTIME_DIR)/$(SONAME) $(RUNTIME_DIR)/$(FULL_NAME_COMPILER)
	sudo mkdir -p $(INCLUDE_DIR)
	sudo cp src/runtime/include/*.h $(INCLUDE_DIR)

install-compiler: install-runtime $(TARGET_COMPILER)
	sudo mv $(FULL_NAME_COMPILER) $(BIN_DIR)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

test-valgrind: $(TEST_TARGET)
	valgrind --track-origins=yes --leak-check=full --show-reachable=yes --show-leak-kinds=all --dsymutil=yes --trace-children=yes ./$(TEST_TARGET)

