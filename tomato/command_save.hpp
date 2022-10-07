#ifndef COMMAND_SAVE_HPP_INCLUDED
#define COMMAND_SAVE_HPP_INCLUDED

#include "command.hpp"
#include <string>
#include <list>

using namespace std;

class command_save : public command {

protected:
    int min_params();
    int max_params();

public:
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_SAVE_HPP
