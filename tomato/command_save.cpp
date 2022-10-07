#include "command_save.hpp"
#include "command_saveas.hpp"

using namespace std;
void command_save::exec(list<string> params){
    (void)params;
    command_saveas s;
    s.exec(last_name);
}

void command_save::help(void){
    cout << " -save" << endl;
    cout << "	Save current image back to file." << endl;
}
int command_save::min_params() { return 0; }
int command_save::max_params() { return 0; }
