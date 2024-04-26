NAME = webserv

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98 -Iheaders -g

SRC= Webserv.cpp HttpRequest.cpp
SRC+= main.cpp Logger.cpp Socket.cpp Server.cpp Http.cpp
# Parsing files
SRC+= Config.cpp



VPATH= src/:obj/:headers/:http_request_parser/:src/config/

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