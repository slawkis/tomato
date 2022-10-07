#ifndef COMMAND_OPEN_HPP_INCLUDED
#define COMMAND_OPEN_HPP_INCLUDED

#include "command.hpp"
#include <string>
#include <list>
#include <sys/stat.h>

using namespace std;

class command_open : public command {
private:
    long fsize(string name);
    int is_dsk(string name);
    int is_img(string name);
    int is_tap(string name);
//    int is_tzx(string name);
    void open_dsk(string name);
    void open_img(string name);
    void open_tap(string name);
//    void open_tzx(string name);
protected:
    int min_params();
    int max_params();

public:
    void exec(string name);
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_OPEN_HPP
