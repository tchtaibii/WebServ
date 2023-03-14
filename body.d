
----------------------------365891840241532904797431
Content-Disposition: form-data; name="kf"; filename="main.cpp"
Content-Type: text/x-c

#include "./Socket/socket.hpp"
#include "./confParsing/parsingConf.hpp"
#include "./connection.hpp"

int main(int ac, char **av)
{
    std::vector<server> servers = ConfingParsing(ac, av);
    ws::socketStart(servers);
    return 0;
}
----------------------------365891840241532904797431
Content-Disposition: form-data; name="k"; filename="connection.hpp"
Content-Type: application/octet-stream

