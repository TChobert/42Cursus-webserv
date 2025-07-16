# === Variables ===

NAME = config_reader_test

# Fichiers source
SRCS = ../../../src/config_parse/ConfigFileReader.cpp \
       main.cpp

# Dossiers d'headers
INCLUDES = -I ../../../inc -I ../../../inc/config_parse

# Compilateur
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror

# === Règles ===

all:
	$(CXX) $(CXXFLAGS) $(SRCS) $(INCLUDES) -o $(NAME)

clean:
	rm -f $(NAME)

re: clean all
