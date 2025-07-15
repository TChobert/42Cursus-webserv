# Makefile pour test/config_parse/Formatter/main.cpp

# Compilateur et options
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I../../../inc/config_parse

# Dossier sources
SRC_DIR = ../../../src/config_parse
TEST_DIR = .

# Fichiers sources nécessaires pour ce test
SRCS = $(TEST_DIR)/main.cpp \
       $(SRC_DIR)/CommentsRemover.cpp \
       $(SRC_DIR)/ConfigFileReader.cpp \
       $(SRC_DIR)/Formatter.cpp

# Fichiers objets (on remplace .cpp par .o)
OBJS = $(SRCS:.cpp=.o)

# Nom de l'exécutable
TARGET = formatter_test

# Règles

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
