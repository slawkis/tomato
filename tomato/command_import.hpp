#ifndef COMMAND_IMPORT_HPP_INCLUDED
#define COMMAND_IMPORT_HPP_INCLUDED

#include "command_write.hpp"
#include <string>
#include <list>

using namespace std;

class command_import : public command_write {
protected:
    int min_params();
    int max_params();

public:
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_IMPORT_HPP
