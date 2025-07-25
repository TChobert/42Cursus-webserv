# Variables
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I../../../inc -I../../../inc/executor -I../../../inc/response -I../../../inc/parse

# Chemins sources (relatifs au Makefile)
SRC_DIR = ../../../src
TEST_DIR = .

# Binaire à générer ici même
BIN = test_get_executor

# Fichiers sources (test + fichiers src)
SRCS = $(TEST_DIR)/main.cpp \
       $(SRC_DIR)/executor/GetExecutor.cpp \
       $(SRC_DIR)/executor/Executor.cpp \
       $(SRC_DIR)/executor/ResourceChecker.cpp \
       $(SRC_DIR)/executor/StaticFileHandler.cpp \
	   $(SRC_DIR)/executor/PostExecutor.cpp \
	   $(SRC_DIR)/executor/DeleteExecutor.cpp \
	   $(SRC_DIR)/executor/CGIHandler.cpp \
	   $(SRC_DIR)/IModule.cpp \
	   $(SRC_DIR)/webserv_utils.cpp \

all: $(BIN)

$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(BIN)

.PHONY: all clean
