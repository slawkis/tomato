#include "command_export.hpp"
#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;



void command_export::exec(list<string> params){
    string folder = ".";
    string name;
    if (params.size()) {
        folder = params.front();
#if defined(WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
        if (mkdir(folder.c_str()) == -1) {
                cout << "E : cannot create folder "<<folder<<endl;
                return;
        }
#else
        if (mkdir(folder.c_str(), 0777) == -1) {
                cout << "E : cannot create folder "<<folder<<endl;
                return;
        }
#endif
    }
    dir_import();
    for (auto& f: files){
        name = f.name;
        tos_to_os(name);
        if(f.parentID) { name = to_string(f.parentID)+"_"+name; }
        name = folder + "/" + name;
        command_read::exec(f,name);
//        cout <<"o : image " << f.name << " dumped." << endl;
    }

    cout <<"O : all files exported to  folder '"<< folder << "'" << endl;
}

void command_export::help(void){
    cout << " -export [<directory>]" << endl;
    cout << "	Extract all files from image to current or given folder." << endl;
}

int command_export::min_params() { return 0; }
int command_export::max_params() { return 1; }
