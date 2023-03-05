#include "./Socket/socket.hpp"
#include "./confParsing/parsingConf.hpp"
#include "./Socket/connection.hpp"

int main(int ac, char **av)
{
    std::vector<server> servers = ConfingParsing(ac, av);
    ws::socketStart(servers);
    return 0;
}