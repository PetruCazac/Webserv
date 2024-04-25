NAME= webserv

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98 -Iheaders -Iheaders/config -g

SRC= Webserv.cpp HttpRequest.cpp
SRC+= main.cpp Logger.cpp Socket.cpp Server.cpp Http.cpp
# Parsing files
SRC+= Config.cpp ParseHttp.cpp ParseLocation.cpp ParseServer.cpp



VPATH= src/:obj/:http_request_parser/:src/config/

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
	/bin/rm -f webserv

re: fclean all

.PHONY: all clean fclean re