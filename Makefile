FLAGS = -Wall -Wextra -Werror
SRC = confParsing/*.cpp main.cpp
CC = c++

all : server 

server : $(SRC_CLIENT)
	@$(CC) $(FLAGS) $(SRC) -o webserv
	@tput setaf 2; echo "Server IS READY"
	@./webserv conf.conf


clean :
	@rm -f client webserv
	@tput setaf 1; echo "CLEAN COMPLET"

fclean : clean

push : 
	git add . && git commit -m "chunked is complet" && git push origin webserv 

re: fclean all