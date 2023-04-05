#include "Socket/socket.hpp"
#include "Config/parsingConf.hpp"
#include "Connection.hpp"

int main(int ac, char **av, char **env)
{
    if (ac == 2)
    {
        std::cout << "\033[32mWEBSERV WORKING NOW...\033[0m\n" << std::endl;
        std::string PWD = ws::get_PWD(env);
        std::vector<ws::server> servers = ConfingParsing(av, PWD);
        ws::socketStart(servers);
    }
    else
        std::cout << "\033[31mWRONG ARGUMENTS\033[0m\n" << std::endl;
    return 0;
}