FLAGS = -Wall -Wextra -Werror
SRC = confParsing/*.cpp main.cpp
CC = c++

all : server 

server : $(SRC_CLIENT)
	@$(CC) $(FLAGS) $(SRC) -o webserv
	@tput setaf 2; echo "Server IS READY"


clean :
	@rm -f client webserv
	@tput setaf 1; echo "CLEAN COMPLET"

fclean : clean

re: fclean all