#ifndef COMMAND_INFO_HPP_INCLUDED
#define COMMAND_INFO_HPP_INCLUDED

#include "command.hpp"
#include <string>
#include <list>

using namespace std;

class tos_file {
public:
    int parentID;
    string name;
    vector<unsigned char>au;
    int size;
    int hidden;
    int readonly;
    tos_file(){}
    tos_file(int i, string n, unsigned char*a, int s=0, int h=0, int r=0);
    tos_file(int i, string n, array<unsigned char,16>& a, int s=0, int h=0, int r=0);
    void join(array<unsigned char,16>& a, int s);
    bool operator== (const tos_file &a) const;
};

class dir_list {
public:
    int parentID;
    string name;
    int id;
    int hidden;
    int readonly;
    dir_list(){}
    dir_list(string n, int p=0, int i=0, int h=0, int r=0);
    bool operator== (const dir_list &a) const;
};

class command_info : public command {
protected:
    list<dir_list> dirs;
    list<tos_file>files;
    string  root;
protected:
    int min_params();
    int max_params();
    int dir_import();
    void print_dirs(list<dir_list> &dirs, int id=0, unsigned long lev=0);
    void get_dirs(list<tos_dir> &t, list<dir_list>&d);
    void get_files(list<tos_dir> &t, list<tos_file>&f);

public:
    void exec(list<string> params);
    void help(void);
};

#endif //COMMAND_INFO_HPP
