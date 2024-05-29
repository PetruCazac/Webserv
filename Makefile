NAME = webserv

CXX= c++
CXXFLAGS= -Wall -Wextra  -std=c++98 -Iheaders -Iheaders/config -g
# -Werror
SRC= Webserv.cpp HttpRequest.cpp HttpResponse.cpp
SRC+= main.cpp Logger.cpp Socket.cpp Server.cpp
# Parsing files
SRC+= Config.cpp DefaultValues.cpp
SRC+= UtilsHttp.cpp

VPATH= src/:obj/:http_request_parser/:src/config/

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