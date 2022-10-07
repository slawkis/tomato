#ifndef COMMAND_SAVEAS_HPP_INCLUDED
#define COMMAND_SAVEAS_HPP_INCLUDED

#include "command_info.hpp"
#include <string>
#include <list>

using namespace std;

class command_saveas : public command_info {
private:
    void save_tap(string name);
    void save_raw(string name);
    void save_img(string name);
    void save_dsk(string name, int edsk = 0);

protected:
    int min_params();
    int max_params();

public:
    void exec(string name);
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_SAVEAS_HPP
