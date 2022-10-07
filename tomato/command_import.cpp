#include "command_import.hpp"
#include <dirent.h>

using namespace std;

void command_import::exec(list<string> params){
    string folder,file;
    folder = params.front();

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (folder.c_str())) != nullptr) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != nullptr) {
          file=string(ent->d_name);
          if (( file== ".")||(file=="..")) { continue;}
        params.clear();
        file=folder+"/"+file;
        params.push_back(file);
        command_write::exec(params);
      }
      closedir (dir);
    } else {
      /* could not open directory */
      perror ("");
        cout << "E : Cannot open folder «" << folder << "»." << endl;
        return;
    }

    cout << "O : all files from «" << folder << "» were imported."<<endl;
}

void command_import::help(void){
    cout << " -import <directory>" << endl;
    cout << "	Put all files from folder into root directory of image." << endl;
    cout << "	(without creating TOS headers! No subfolders!)" << endl;
}

int command_import::min_params() { return 1; }
int command_import::max_params() { return 1; }
