# Nom de l'exécutable
NAME = test_location_parser

# Fichiers source
SRCS = main.cpp \
       ../../../src/config_parse/LocationSectionParser.cpp \
       ../../../src/config_parse/LocationPropertiesProcessor.cpp \
       ../../../src/config_parse/PropertyProcessor.cpp \
	   ../../../src/config_parse/locationPropertiesFlags.cpp \
	../../../src/config_parse/serverPropertiesFlags.cpp \
	../../../src/webserv_utils.cpp

# Fichiers objets
OBJS = $(SRCS:.cpp=.o)

# Compilateur et flags
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I../../../inc -I../../../inc/config_parse

# Cible principale
all: $(NAME)

# Règle de l'exécutable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# Nettoyage des .o
clean:
	rm -f $(OBJS)

# Nettoyage complet
fclean: clean
	rm -f $(NAME)

# Rebuild complet
re: fclean all
