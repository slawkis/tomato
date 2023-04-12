#include "command_list.hpp"

using namespace std;

bool compare_file2 (const tos_file& first, const tos_file& second)
{
     return ( first.name < second.name );
}

void command_list::list_files(int id, unsigned long lev, int last) {
    (void)last;
    string spacer;
    string stype;
    int hlen;
    int type;
    int len;
    int start;
    int baslen;
    unsigned char * header;

//    spacer = (last)?"                 ":"||||||||||||||||";
    spacer = "                                             ";
    for (auto& it:files) {
        if (it.parentID==id) {
            hlen=0;
            len=it.size;
            start=baslen=0;
            type=5;
            if (len) {
                header = reinterpret_cast<unsigned char*>(image.tos_au(it.au[0]));
                type = header[0];
                if(type) {
                    hlen=5;
                    len = header[1]+256*header[2];
                    start = header[3]+256*header[4];
                    baslen = -1;
                } else {
                    hlen=7;
                    start = header[1]+256*header[2];
                    len = header[3]+256*header[4];
                    baslen = header[5]+256*header[6];
                }
                if ((len+hlen)!=it.size) {
                    type=5;
                    hlen=0;
                    len=it.size;
                    start=-1;
                    baslen=-1;
                }
            }
            cout << setw(0) << setfill(' ');
            if (lev!=0) { cout << "│" ; }
            cout << spacer.substr(0,lev);
//            cout << "└" << setw(14)<< it.name << "\t";
            stype = "└" + it.name + spacer;
            cout << stype.substr(0,15);
//            cout << stype << "<" <<endl;
            if(!lev){ cout << "  ";}
            cout << setw(7) << it.size << "\t" ;

            switch(type) {
                case 0 : stype = "BASIC"; break;
                case 1 : stype = "VAR()"; break;
                case 2 : stype = "VAR$()"; break;
                case 3 : stype = "CODE"; break;
                default : stype = "binary"; break;
            }
            if (type==5) start = -1;
            cout << setw(7) << stype << "\t" ;
            if (start >=0) {
                cout << setw(7) << start << "\t" ;
            } else {
                cout << setw(7) << " " << "\t" ;
            }
            cout << setw(7) << len << "\t" ;
            if((type==0)&&(baslen>=0)) {
                len -= baslen;
                cout << setw(7) << len << "\t" ;
            } else {
                cout << setw(7) << " " << "\t" ;
            }
            stype=(it.readonly)?"YES":" ";
            cout << setw(3)<< "\t" << stype;
            stype=(it.hidden)?"YES":" ";
            cout << "\t" << stype << endl;
        }
    }
}

void command_list::list_dirs(int id, unsigned long lev){
//    string spacer = "|||||||||||||||";
    string stype;
    string spacer = "                 ";

/*
//    dir_list last;
//    last=dirs.back();
    list<dir_list>::iterator it;
    it=dirs.begin();
//    for (auto& it:dirs) {
        if (it->parentID == id) {
//            cout << "";
            cout << spacer.substr(1,lev);
            cout << "└" << it->id << ":" << it->name << "\t\t\t\t";
            cout << it->readonly << "\t" << it->hidden << endl;
            if (*it == dirs.back() ) { list_files(it->id,lev+1,1); } else {list_files(it->id,lev+1);}
            list_dirs(it->id,lev+1);
        }
//    }
*/
//    cout << dirs.back().name<<endl;
    dir_list last;
    for (auto& it:dirs) {
        if(it.parentID==0) { last=it;}
    }
    for (auto& it:dirs) {
        if (it.parentID == id) {
            if(lev!=0) { cout << "│"; }// else { cout << " ";}
            cout << spacer.substr(0,lev);
            cout << "└" << it.id << ":" << it.name << "\t---------------------------------------------- ";

            stype=(it.readonly)?"YES":" ";
            cout << "\t" << stype;
            stype=(it.hidden)?"YES":" ";
            cout << "\t" << stype << endl;
            list_files(it.id,lev+1,0);
//            if (it == last ) { list_files(it.id,lev+1,1); } else {list_files(it.id,lev+1,0);}
            list_dirs(it.id,lev+1);
        }
    }
}


void command_list::exec(list<string> params){
    (void) params;
    dir_import();
    files.sort(compare_file2);
    cout << "name\t\t   size\t   type\t  start\t length\tvarsize\t   readonly  hidden"<<endl;
    cout << "==========================================================================="<<endl;
    cout << root << endl;
    list_dirs();
    list_files(0,0,0);
}

void command_list::help(void){
    cout << " -list" << endl;
    cout << "	List all files and folders." << endl;
}

int command_list::min_params() { return 0; }
int command_list::max_params() { return 0; }
