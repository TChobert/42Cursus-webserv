CXX := c++ -g3 #-fsanitize=address
CXXFLAGS := -std=c++98 -Wall -Wextra #-Werror

NAME := webserv

SRC_DIR := src/
SRC := $(shell find src -name '*.cpp')

INC_DIR := inc/

OBJ_DIR := .obj/
OBJ := $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))
DEP := $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.d, $(SRC))

all: $(NAME)

bonus: all

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $^ -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@ mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP  -c $< -o $@ 

-include $(DEP)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
