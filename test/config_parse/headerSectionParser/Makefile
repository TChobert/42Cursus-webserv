NAME = test_header_parser

SRCS = main.cpp \
       ../../../src/config_parse/HeaderSectionParser.cpp

OBJS = $(SRCS:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++98 -I../../../inc -I../../../inc/config_parse

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
