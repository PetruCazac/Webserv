# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fkrug <fkrug@student.42heilbronn.de>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/11 15:07:34 by fkrug             #+#    #+#              #
#    Updated: 2024/03/29 17:49:18 by fkrug            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -g -Werror

# Define targets for both server and client
all: server client

# Rule to make server
server: server.o
	$(CXX) $(CXXFLAGS) -o server server.o

# Rule to make client
client: client.o
	$(CXX) $(CXXFLAGS) -o client client.o

# Generic rule for object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean object files
clean:
	rm -f server.o client.o

# Full clean (includes executables)
fclean: clean
	rm -f server client

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re
