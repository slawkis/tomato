#ifndef COMMAND_SIDE_HPP_INCLUDED
#define COMMAND_SIDE_HPP_INCLUDED

#include "command.hpp"
#include <string>
#include <list>

using namespace std;

class command_side : public command {
protected:
    int min_params();
    int max_params();

public:
    void exec(int side);
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_SIDE_HPP
