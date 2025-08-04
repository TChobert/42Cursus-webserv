# # Nom de l'exécutable
# NAME = test_location_parser

# # Fichiers source
# SRCS = main.cpp \
#        ../../../src/config_parse/LocationSectionParser.cpp \
#        ../../../src/config_parse/LocationPropertiesProcessor.cpp \
#        ../../../src/config_parse/PropertyProcessor.cpp \
# 	   ../../../src/config_parse/locationPropertiesFlags.cpp \
# 	../../../src/config_parse/serverPropertiesFlags.cpp \
# 	../../../src/webserv_utils.cpp

# # Fichiers objets
# OBJS = $(SRCS:.cpp=.o)

# # Compilateur et flags
# CXX = g++
# CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I../../../inc -I../../../inc/config_parse

# # Cible principale
# all: $(NAME)

# # Règle de l'exécutable
# $(NAME): $(OBJS)
# 	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# # Nettoyage des .o
# clean:
# 	rm -f $(OBJS)

# # Nettoyage complet
# fclean: clean
# 	rm -f $(NAME)

# # Rebuild complet
# re: fclean all

# Noms des exécutables
NAME1 = test_location_process
NAME2 = test_location_section_parser

# Fichiers sources communs aux deux tests
COMMON_SRCS = \
	../../../src/config_parse/LocationSectionParser.cpp \
	../../../src/config_parse/LocationPropertiesProcessor.cpp \
	../../../src/config_parse/PropertyProcessor.cpp \
	../../../src/config_parse/locationPropertiesFlags.cpp \
	../../../src/config_parse/serverPropertiesFlags.cpp \
	../../../src/webserv_utils.cpp

# Sources spécifiques à chaque test
SRCS1 = main.cpp
SRCS2 = test_LocationSectionParser.cpp

# Objets
OBJS1 = $(SRCS1:.cpp=.o) $(COMMON_SRCS:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o) $(COMMON_SRCS:.cpp=.o)

# Compilateur et flags
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I../../../inc -I../../../inc/config_parse

# Cibles
all: $(NAME1) $(NAME2)

$(NAME1): $(OBJS1)
	$(CXX) $(CXXFLAGS) $(OBJS1) -o $(NAME1)

$(NAME2): $(OBJS2)
	$(CXX) $(CXXFLAGS) $(OBJS2) -o $(NAME2)

# Nettoyage des objets
clean:
	rm -f *.o
	rm -f ../../../src/config_parse/*.o

# Nettoyage complet
fclean: clean
	rm -f $(NAME1) $(NAME2)

# Rebuild complet
re: fclean all
