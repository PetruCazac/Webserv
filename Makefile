NAME= webserv

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98 -Iheaders

SRC= webserv.cpp HttpRequest.cpp getMethod.cpp

VPATH= src/:obj/:headers/

OBJ = $(SRC:%.cpp=$(OBJ_PATH)/%.o)
OBJ_PATH = obj

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
	@printf "compiled\n"

$(OBJ_PATH)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_PATH)
	@printf "cleaned\n"

fclean: clean
	@/bin/rm -f webserv

re: fclean all

.PHONY: all clean fclean re