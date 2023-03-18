#include "Socket/socket.hpp"
#include "Config/parsingConf.hpp"
#include "Connection.hpp"

int main(int ac, char **av)
{
    std::vector<server> servers = ConfingParsing(ac, av);
    ws::socketStart(servers);
    return 0;
}