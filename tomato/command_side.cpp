#include "command_side.hpp"

using namespace std;

void command_side::exec(int side) {
    if (current_format.sides) {
        image.set_side(side);
        current_side = side;
        cout << "o : side changed to "<< side << endl;
    } else {
        if(side) { cout << "E : single side image." << endl;}
    }
}

void command_side::exec(list<string> params){
    int side = stoi(params.front());
    exec(side);
}

void command_side::help(void){
    cout << " -side <0|1>" << endl;
    cout << "	Change disk side (tos140ds|tos4ds only)" << endl;
}
int command_side::min_params() { return 1; }
int command_side::max_params() { return 1; }
