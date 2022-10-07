#ifndef COMMAND_SREAD_HPP_INCLUDED
#define COMMAND_SREAD_HPP_INCLUDED

#include "command_read.hpp"
#include <string>
#include <list>

using namespace std;

class command_sread : public command_read {
protected:
    int min_params();
    int max_params();

public:
    void exec(tos_file& file, string name_to);
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_SREAD_HPP
