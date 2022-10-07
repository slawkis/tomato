#include "command_delete.hpp"

using namespace std;

void command_delete::exec(string name) {
    string tname = os_to_internal(name);
    if (image.erase(tname)) {
        cout << "O : « " << name << " » was deleted" << endl;
    } else {
        cout << "E : « " << name << " » not found (wrong name? wrong folder?)" << endl;
    }
}

void command_delete::exec(list<string> params){
    for(auto& n: params) {
        exec(n);
    }
}

void command_delete::help(void) {
    cout << " -delete <tosname[.dir]> [<tosname[.dir]> ...] " << endl;
    cout << "	Delete file(s) or folder(s) from image." << endl;
    cout << "	(Subfolders included. No questions asked.)" << endl;
}

int command_delete::min_params() { return 1; }
int command_delete::max_params() { return 127; }
