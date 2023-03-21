FLAGS = -Wall -Wextra -Werror
SRC = Config/*.cpp main.cpp
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
	git add . && git commit -m "fix some tkhrbi9" && git push origin boukili 

re: fclean all