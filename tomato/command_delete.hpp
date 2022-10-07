#ifndef COMMAND_DELETE_HPP_INCLUDED
#define COMMAND_DELETE_HPP_INCLUDED

#include "command.hpp"
#include <string>
#include <list>

using namespace std;

class command_delete : public command {
protected:
    int min_params();
    int max_params();

public:
    void exec(list<string> params);
    void exec(string name);
    void help(void);
};

#endif //COMMAND_DELETE_HPP
