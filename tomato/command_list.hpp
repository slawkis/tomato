#ifndef COMMAND_LIST_HPP_INCLUDED
#define COMMAND_LIST_HPP_INCLUDED

#include "command_info.hpp"
//#include "command.hpp"
#include <string>
#include <list>

using namespace std;

class command_list : public command_info {
protected:
    int min_params();
    int max_params();
    void list_dirs (int id=0, unsigned long lev=0);
    void list_files(int id=0, unsigned long lev=0, int last = 0);
public:
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_LIST_HPP
