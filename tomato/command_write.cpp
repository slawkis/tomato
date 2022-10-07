#include "command_write.hpp"

using namespace std;

long command_write::fsize(string name){
    struct stat stat_buf;
    int rc = stat(name.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


void command_write::exec(string osname , string tosname) {
    string iname;
    fstream file;
    char data[1024*1024];
    long size = fsize(osname);

    if ((size==0)||(size>620*1024)) {
        if (size) {
            cout << "E : file too big ( «" << osname << "» is "<<size<<" bytes long.)" << endl;
        } else {
            cout << "E : file «" << osname << "» doesn't exist." << endl;
        }
        return;
    }

    os_to_internal(tosname,iname);
    os_to_tos(tosname);
    if (image.dir_exists(iname.substr(0,9)+"DIR")){
        cout << "E : Directory and file cannot share the name."<<endl;
        return;
    }

    file.open(osname,ios::in | ios::binary);
    if (!file.is_open()) {
        cout << "E : cannot open file " << osname << endl;
        return;
    }

    if (image.file_exists(iname)) {
        image.erase_file(iname);
        cout << "o : old file " << osname << " was deleted." << endl;
    }
    memset(data,0,1024*1024);
    file.read(data,size);
    file.close();
    if(image.create_file(iname,data,size)) {
        cout << "o : file «" << osname << "» saved as «" << tosname << "»" <<endl;
    }
}

void command_write::exec(list<string> params){
    string n1, n2;
    size_t pos;
    n1 = params.front();
    if(params.size()>1){
        n2 = params.back();
    } else {
        n2=n1;
        pos = n2.find_last_of('/');
        if (pos != string::npos) {
            n2=n2.substr(pos+1);
        }
    }
    exec(n1,n2);
}

void command_write::help(void){
    cout << " -write <filename> [<tosname>]" << endl;
    cout << "	Wtite a single local file to image." << endl;
}
int command_write::min_params() { return 1; }
int command_write::max_params() { return 2; }
