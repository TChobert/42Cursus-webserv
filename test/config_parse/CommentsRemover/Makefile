# Compiler & flags
CXX     := c++
CXXFLAGS:= -Wall -Wextra -Werror -std=c++98 \
           -I../../../inc \
           -I../../../inc/config_parse

# Sources
SRCS    := ../../../test/config_parse/CommentsRemover/main.cpp \
           ../../../src/config_parse/CommentsRemover.cpp \
           ../../../src/config_parse/ConfigFileReader.cpp

OBJS    := $(SRCS:.cpp=.o)

# Executable name
NAME    := comments_remover_test

# Rules
all: $(NAME)

$(NAME): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
