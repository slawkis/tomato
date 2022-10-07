#include "command_new.hpp"
#include "command_side.hpp"
#include "command_rename.hpp"
#include <ctime>

using namespace std;

void command_new::exec(list<string> params){
    int found=0;
    string str;

    for (auto& it :format_list) {
        if (it.name==params.front()) { found=1; current_format=it; break; }
    }
    if (!found) {
        cout << "E : Unknown format " << params.front() << endl;
        return;
    }

    image.init();
    command_side s;
    command_rename r;
    if (current_format.sides) {
        s.exec(1);
        r.exec(initial_name);
        str = "2*" + to_string(current_format.size/2048);
    } else {
        str = to_string(current_format.size/1024);
    }
    s.exec(0);
    r.exec(initial_name);

    if (current_format.system) {
        memcpy(tos2ext, tos2a,16384);
        if (current_format.tracks > 70) {
            tos2ext[0x0e] = 1;
            image.set_au(4096);
        }
    } else {
        memset(tos2ext,0,16384);
    }

    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (buffer,80,string("diskimage_"+current_format.name+"_%Y%m%d_%H%M%S.img").c_str(),timeinfo);
    last_name = buffer;

    cout << "o : created empty image (format: " \
         << current_format.name << ", size: " \
         << str << "kB, name: " \
         << initial_name \
         << ")" << endl;
//         << " system " << current_format.system
//         << " tracks " << current_format.tracks

}

void command_new::help(void){
    string formats="";
    for (auto& it :format_list) {
	formats += it.name + " | ";
    }
    formats = formats.substr(0,formats.length() - 3) + ">";
    cout << " -new " << formats << endl;
    cout << "	Create new image." << endl;
}
int command_new::min_params() { return 1; }
int command_new::max_params() { return 1; }
