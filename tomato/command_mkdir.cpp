#include "command_mkdir.hpp"

using namespace std;

void command_mkdir::exec(string name){
    string tname = os_to_internal(name);
    if (image.mkdir(tname)) {
        cout << "O : directory « " << name << " » created" << endl;
    }
}

void command_mkdir::exec(list<string> params){
    for(auto& n: params) {
        exec(n);
    }
}

void command_mkdir::help(void){
    cout << " -mkdir <tosname> [<tosname> ...]" << endl;
    cout << "	Create new folder(s)." << endl;
}
int command_mkdir::min_params() { return 1; }
int command_mkdir::max_params() { return 128; }
