#include "command_read.hpp"

using namespace std;


int command_read::find_file(tos_file&f,string name, int dir) {
    for(auto& it:files) {
        if (((dir<0)&&(it.name == name))||((dir>=0)&&(it.name==name)&&(it.parentID == dir))) {
            f=it;
            return 1;
        }
    }
    return 0;
}

void command_read::exec(tos_file& file, string name_to) {
    fstream outfile;
    char * ptr;
    int size;

    outfile.open(name_to,ios::out | ios::trunc | ios::binary);
    if (!outfile.is_open()) {
        cout << "E : cannot open file " << name_to << " !" << endl;
    }
    size=file.size;
    for(auto& au:file.au) {
        ptr = image.tos_au(au);
        if (size>=image.get_au()) {
            outfile.write(ptr,image.get_au());
            size-=image.get_au();
        } else {
            outfile.write(ptr,size);
        }
    }
    outfile.close();
    cout << "o : File " << file.name << " saved as " << name_to << endl;

}

void command_read::exec(list<string> params){
    string name_from, name_to;
    string sdir;
    tos_file file;

    int dir = -1;

    name_from = name_to = params.front();
    if (params.size()>1) {
        name_to=params.back();
    }
    if (string::npos != name_from.find(":")) {
        sdir = name_from.substr(0,name_from.find(":"));
        dir = stoi(sdir);
    }
    if (dir > 15) {
        cout << "E : Invalid dir number " << dir << ". (15 max)" << endl;
    }
    dir_import();
    if (find_file(file,name_from,dir)) {

    } else {
        cout << "E : File not found." << endl;
    }
    tos_to_os(name_to);
    exec(file,name_to);
}

void command_read::help(void){
    cout << " -read <tosname> [<filename>]" << endl;
    cout << "	Save a single tos file to local file." << endl;
}
int command_read::min_params() { return 1; }
int command_read::max_params() { return 2; }
