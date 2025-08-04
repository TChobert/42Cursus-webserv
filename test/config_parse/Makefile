# Nom de l'exécutable
NAME = test_config_parser

# Dossier de test
TEST_DIR = .

# Source principale du test
TEST_SRC = $(TEST_DIR)/test_config_parser.cpp \
../../src/webserv_utils.cpp

# Fichiers sources du parser
SRC_DIR = ../../src/config_parse
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Headers
INCLUDES = -I../../inc -I../../inc/config_parse

# Flags de compilation
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Commande finale
$(NAME): $(SRCS) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) $(TEST_SRC) -o $(NAME)

# Nettoyage
clean:
	rm -f $(NAME)

re: clean $(NAME)

.PHONY: clean re
