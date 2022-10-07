#include "command_move.hpp"

using namespace std;

void command_move::exec(list<string> params){
    string from, to;
    from = params.front();
    to = params.back();
    from=os_to_internal(from);
    to=os_to_internal(to);
    if (image.move(from, to) ) {
        cout <<"o : «" << params.front() << "» moved to «" << params.back() << "»." << endl;    }
}

void command_move::help(void){
    cout << " -move <tosname> <tosname>" << endl;
    cout << "	Rename or move a file." << endl;
}
int command_move::min_params() { return 2; }
int command_move::max_params() { return 2; }
