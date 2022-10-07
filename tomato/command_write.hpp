#ifndef COMMAND_WRITE_HPP_INCLUDED
#define COMMAND_WRITE_HPP_INCLUDED

#include "command_info.hpp"
#include "command_delete.hpp"
#include <string>
#include <list>
#include <string>
#include <list>
#include <sys/stat.h>

using namespace std;

class command_write : public command_info {
private:
        long fsize(string name);
protected:
    int min_params();
    int max_params();
    command_delete erase;
public:
    void exec(list<string> params);
    void exec(string osname , string tosname);
    void help(void);
};

#endif //COMMAND_WRITE_HPP
