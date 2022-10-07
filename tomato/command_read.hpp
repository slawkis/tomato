#ifndef COMMAND_READ_HPP_INCLUDED
#define COMMAND_READ_HPP_INCLUDED

#include "command_info.hpp"
#include <string>
#include <list>

using namespace std;

class command_read : public command_info {
protected:
    int find_file(tos_file&f,string name, int dir = -1);

    int min_params();
    int max_params();

public:
    void exec(tos_file& file, string name_to);
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_READ_HPP
