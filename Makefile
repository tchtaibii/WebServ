FLAGS = -Wall -Wextra -Werror
SRC = main.cpp 
CC = c++

all : server 

server : $(SRC_CLIENT)
	@$(CC) $(FLAGS) $(SRC) -o webserv
	@./webserv conf.conf
	@touch Session_management/sessionIds


clean :
	@rm -f webserv ./Session_management/sessionIds ./Assets/uploads/*
	@tput setaf 1; echo "CLEAN COMPLET"

fclean : clean

push : 
	git add . && git commit -m "adding querry" && git push origin webserv 

re: fclean all