NAME= webserv

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98 -Iheaders -g

SRC= main.cpp Logger.cpp Socket.cpp Server.cpp

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
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f webserv

re: fclean all

.PHONY: all clean fclean re