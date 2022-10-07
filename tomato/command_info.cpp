#include "command_info.hpp"

using namespace std;

bool tos_file::operator== (const tos_file &a) const {
    if (
            (a.parentID != parentID)||
            (a.name != name)||
            (a.size != size)||
            (a.hidden != hidden)||
            (a.readonly != readonly) ) return false;
    for (unsigned long c=0; c < 16; c++) { if (a.au[c] != au[c]) return false; }
    return true;
}

tos_file::tos_file(int i, string n, unsigned char*a, int s, int h, int r){
    int c;
    parentID=i;
    name=n;
    size=s;
    hidden=h;
    readonly=r;
    c=0;
    while((*a!=0)&&(c<16)){au.push_back(*a); a++; c++;}
}

tos_file::tos_file(int i, string n, array<unsigned char,16>& a, int s, int h, int r) {
    parentID=i;
    name=n;
    size=s;
    hidden=h;
    readonly=r;
    for(unsigned long i=0; i<16;i++) { if(a[i]!=0) { au.push_back(a[i]); } }
}

bool dir_list::operator== (const dir_list &a) const {
    if (
            (a.parentID != parentID)||
            (a.id != id)||
            (a.name != name)||
            (a.hidden != hidden)||
            (a.readonly != readonly) ) return false;
    return true;
}

dir_list::dir_list(string n, int p, int i, int h, int r){
    name = n; parentID = p; id = i; hidden=h; readonly=r;
}
void tos_file::join(array<unsigned char,16>& a, int s) {
    size += s;
    for(unsigned long i=0; i<16;i++) { if(a[i]!=0) { au.push_back(a[i]); } }
}


bool compare_dir (const dir_list& first, const dir_list& second)
{
    if (first.parentID==second.parentID) { return ( first.name < second.name ); }
    return (first.parentID<second.parentID);
}

void command_info::get_dirs(list<tos_dir> &tos, list<dir_list>&dir) {
    for (auto &d: tos) {
        if (d.ext=="DIR") {
            dir.push_back(dir_list(d.name,d.parent&0x7f, d.blocks[0]&0xff,d.hidden, d.readonly));
        }
    }
    dir.sort(compare_dir);
/*
    list<tos_dir> tt;
    for(auto& it: tos) {
        if(it.ext != "DIR") {tt.push_back(it);}
    }
    tos=tt;
*/
}

bool compare_file (const tos_dir& first, const tos_dir& second)
{
    string dir1 = first.name+first.ext;
    string dir2 = second.name+second.ext;
    if(dir1 == dir2) {
        return first.part<second.part;
    }
    return ( dir1 < dir2 );
}

void command_info::get_files(list<tos_dir> &tos, list<tos_file>&files) {
    tos_file f;
    string name;
    int size;
    tos.sort(compare_file);
    for (auto& file: tos) {
        if(file.ext == "DIR") { continue;}
        name = file.name;
        if (file.ext != "   ") {
            name = rtrim(file.name) + "." + rtrim(file.ext);
        }
        name = name+"            ";
        name = name.substr(0,12);
//cerr << "<" << name << ">" <<endl;
        lowcase(name);
        name=rtrim(name);
        size=file.size*128;
        if (file.tail) { size = size - 256 + file.tail;}
//cerr << "b<" << files.back().name << endl;
        if(      (files.size()==0)||
                 (files.back().name!=name)||
                 ((files.back().name==name)&&
                 (files.back().parentID!=file.parent))) {
            f=tos_file(file.parent,name,file.blocks,size,file.hidden,file.readonly);
            files.push_back(f);
        } else {
            files.back().join(file.blocks,size);
        }
    }
}

int command_info::dir_import(){
    tos_dir dir;
    list<tos_dir> tos;
    tos.clear();
    dirs.clear();
    files.clear();
    dir = image.get_dir(0);
    if(dir.parent != 0xff) { return 1;}
    root = dir.name;
//    lowcase(root);
    for(int i = 1; i<128; i++) {
        dir = image.get_dir(i);
        if(dir.parent != 0xe5) { tos.push_back(dir);}
    }
    if (tos.size()) {
        get_dirs(tos, dirs);
        get_files(tos,files);
    }
    return 0;
}

void command_info::print_dirs(list<dir_list> &dirs, int id, unsigned long lev){
    string spacer = "|||||||||||||||";
/*    list<dir_list>tmp;
    for (auto& it:dirs) {
        if (it.id == id) { tmp.push_back(it) ;}
    }
    if (!tmp.size())  { return; }
    if (tmp.size()>1) { tmp.sort(); }
*/
    for (auto& it:dirs) {
        if (it.parentID == id) {
            cout << "     ";
            cout << spacer.substr(0,lev);
            cout << "└" << it.id << ":" << it.name << "\t";
            cout << it.readonly << "\t" << it.hidden << endl;
            print_dirs(dirs,it.id,lev+1);
        }
    }
}
// │└
void command_info::exec(list<string> params){
    (void) params;
    int first,last;
    string spacer = "               ";
    dir_import();
    cout << dec << setfill(' ');
    cout << "o : Image format   : " << current_format.name << endl;
    cout << "o : Tracks         : " << current_format.tracks;
    if (current_format.sides) {
        cout << "+" << current_format.tracks << "(swappable)";
    }
    cout << endl;
    cout << "o : Image size     : " << current_format.size << endl;
    cout << "o : System         : ";
    if(current_format.system) { cout << "on disk (first 4 sectors)";} else { cout << "none.";}
    cout << endl;

    cout << "o : Root directory : " << root << endl;
    if (dirs.size()) {
        cout << "o : Directories ("<<dirs.size() << " of 15)"<<endl;
        cout << "    id:name\t readonly  hidden"<<endl;
        cout << "    ============================="<<endl;
        cout << "     0:"<<root<<endl;
        cout << setw(1) << setfill(' ');
        print_dirs(dirs);
    }
    if (files.size()) {
        cout << "o : Files"<<endl;
        cout << "    dir:name          size readonly   hidden   au"<<endl;
        cout << "    ===============================================>"<<endl;
            for (auto& it:files) {
            cout << "     " << setfill(' ') \
                 << setw(2) << it.parentID <<":" \
                 << setw(12) << it.name \
                 << setw(6) << it.size  \
                 << setw(9) <<it.readonly \
                 << setw(9) << it.hidden;
//            last = it.au[0];
            cout << "   ";
            first=last=0;
//            vector<unsigned char> au = it.au;
            for(auto& a: it.au) {
                if (first==0){first=last=a; continue;}
                if ((a-last) ==1) { last = a; continue;}
                if (first != last) {
                    cout << hex << setw(2) << setfill('0') << (first) << "-";
                    cout << hex << setw(2) << setfill('0') << (last) << " ";
                } else {
                    cout << hex << setw(2) << setfill('0') << (first) << " ";
                }
                first=last=a;
            }
            if (first != last) {
                cout << hex << setw(2) << setfill('0') << (first) << "-";
                cout << hex << setw(2) << setfill('0') << (last) << " ";
            } else {
                cout << hex << setw(2) << setfill('0') << (first) << " ";
            }
            cout << dec << endl;
        }
    }
    cout << "o : " << dirs.size() << " folders and "<< files.size() << " files." << endl;
}

void command_info::help(void){
    cout << " -info" << endl;
    cout << "	Show short info about image." << endl;
}

int command_info::min_params() { return 0; }
int command_info::max_params() { return 0; }
