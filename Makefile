NAME= webserv

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98 -Iheaders

SRC= webserv.cpp HttpRequest.cpp 

VPATH= src/:obj/:headers/

OBJ = $(SRC:%.cpp=$(OBJ_PATH)/%.o)
OBJ_PATH = obj

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

$(OBJ_PATH)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	/bin/rm -rf $(OBJ_PATH)

fclean: clean
	/bin/rm -f webserv

re: fclean all

.PHONY: all clean fclean re