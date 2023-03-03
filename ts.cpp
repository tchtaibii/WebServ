#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::string path = "/data/open/lk";
    std::vector<std::string> pathComponents;
    std::stringstream ss(path);

    std::string component;
    std::getline(ss, component, '/');
    std::string tmp = "\0";
    pathComponents.push_back("/" + component);
    while (std::getline(ss, component, '/'))
    {
        pathComponents.push_back(tmp + "/" + component);
        tmp += "/" + component;
    }

    // Print the path components
    for (std::vector<std::string>::const_iterator it = pathComponents.begin(); it != pathComponents.end(); ++it) {
        std::cout << *it << std::endl;
    }

    return 0;
}