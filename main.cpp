#include "Socket/socket.hpp"
#include "Config/parsingConf.hpp"
#include "Connection.hpp"

int main(int ac, char **av, char **env)
{
    if (ac == 2)
    {
        std::string PWD = ws::get_PWD(env);
        std::vector<ws::server> servers = ConfingParsing(av, PWD);
        ws::socketStart(servers);
    }
    else
        std::cout << "WRONG ARGUMENTS" << std::endl;
    return 0;
}