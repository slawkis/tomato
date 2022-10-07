#ifndef COMMAND_EXPORT_HPP_INCLUDED
#define COMMAND_EXPORT_HPP_INCLUDED

#include "command_read.hpp"
#include <string>
#include <list>

using namespace std;

class command_export : public command_read {
protected:
    int min_params();
    int max_params();

public:
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_EXPORT_HPP
