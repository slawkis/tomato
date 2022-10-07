#ifndef COMMAND_SEXPORT_HPP_INCLUDED
#define COMMAND_SEXPORT_HPP_INCLUDED

#include "command_sread.hpp"
#include <string>
#include <list>

using namespace std;

class command_sexport : public command_sread {
protected:
    int min_params();
    int max_params();

public:
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_SEXPORT_HPP
