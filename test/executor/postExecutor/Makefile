NAME = post_test

# Chemins sources (relatifs au Makefile)
SRC_DIR = ../../../src
TEST_DIR = .

# Binaire à générer ici même
BIN = test_post_executor

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

INC = -I../../../inc -I../../../inc/executor -I../../../inc/response -I../../../inc/parse

CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror

all:
	$(CXX) $(CXXFLAGS) $(SRCS) $(INC) -o $(BIN)

clean:
	rm -f $(BIN)

fclean: clean
	rm -rf test_upload_dir

re: fclean all
