FLAGS = -Wall -Wextra -Werror
SRC = main.cpp
CC = c++

all : server 

server : $(SRC_CLIENT)
	@$(CC) $(FLAGS) $(SRC) -o webserv
	@./webserv conf.conf


clean :
	@rm -f client webserv
	@tput setaf 1; echo "CLEAN COMPLET"

fclean : clean

push : 
	git add . && git commit -m "merge complet" && git push origin webserv 

re: fclean all