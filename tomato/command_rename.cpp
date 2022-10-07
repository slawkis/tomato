#include "command_rename.hpp"

using namespace std;

void command_rename::exec(string name) {
    tos_dir d = tos_dir(0xff,name,"DIR",0,0,0,0,0,0);
    image.set_dir(0,d);
    cout << "o : name changed to <" << name << ">" << endl;
}

void command_rename::exec(list<string> params){
    string n = params.front();
    exec (n);
}

void command_rename::help(void){
    cout << " -rename <dirname>" << endl;
    cout << "	Change disk name." << endl;
}
int command_rename::min_params() { return 1; }
int command_rename::max_params() { return 1; }
