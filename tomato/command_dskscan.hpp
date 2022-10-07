#ifndef COMMAND_DSKSCAN_HPP_INCLUDED
#define COMMAND_DSKSCAN_HPP_INCLUDED

#include "command.hpp"
#include <string>
#include <list>

using namespace std;

class command_dskscan : public command {
protected:
    int min_params();
    int max_params();

    const string h_DSK  = "MV - CPCEMU Disk-File\r\nDisk-Info\r\n";
    const string h_EDSK = "EXTENDED CPC DSK File\r\nDisk-Info\r\n";
    const string h_TRACK= "Track-Info\r\n";

    char buf[16384];

    d_header *dh  = reinterpret_cast<d_header*>(buf);
    t_header *th  = reinterpret_cast<t_header*>(buf+256);

public:
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_DSKSCAN_HPP
