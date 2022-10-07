#ifndef COMMAND_MKDIR_HPP_INCLUDED
#define COMMAND_MKDIR_HPP_INCLUDED

#include "command.hpp"
#include <string>
#include <list>

using namespace std;

class command_mkdir : public command {
protected:
    int min_params();
    int max_params();

public:
    void exec(string name);
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_MKDIR_HPP
