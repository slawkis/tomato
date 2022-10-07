#include "command_sread.hpp"

using namespace std;

typedef struct header {
    unsigned char type;
    unsigned short par1;	// code : length, bas: autostart
    unsigned short par2;	// code : addr  , bas: length
    unsigned short par3;	// code : xxx	, bas: length+vars
} __attribute__((packed)) header;

void command_sread::exec(tos_file& file, string name_to) {
    char * ptr;
    int size;
    int tsize;
    int type;
    fstream outfile;

    size=file.size;
    
    if (size == 0) {
	cout << "o : File " << file.name << " is emtpy." << endl;
	return;
    }


    outfile.open(name_to,ios::out | ios::trunc | ios::binary);
    if (!outfile.is_open()) {
        cout << "E : cannot open file " << name_to << " !" << endl;
    }
    
    ptr = image.tos_au(file.au[0]);
    
//    for (int x =0; x<5; x++) { cout << "*(ptr+"<<x<<") = "<< std::hex << ((unsigned int)ptr[x]&255) << endl; }
    
    header * htos = (header*) ptr;
    
    if (htos->type==0) {
        type = 7;
        tsize = htos->par2;

    } else {
        type = 5;
        tsize = htos->par1;
/*
        tsize = ptr[2];
    cout << "tsize = " << tsize << endl;
        
        tsize *=256;
    cout << "tsize = " << tsize << endl;

	tsize +=ptr[1];       
    cout << "tsize = " << tsize << endl;
        
//	tsize = *(ptr+1)+256*(*(ptr+2));
*/
    }
//    for (int x =0; x<6; x++) { cout << "*(ptr+"<<x<<") = "<< std::hex << ((unsigned int)ptr[x]&255) << endl; }

//    cout << "type = " << type << endl;
//    cout << "tsize = " << tsize << endl;
//    cout << "size = " << size << endl;
    
    if ((tsize + type) != file.size) { type = 0; }
    
//    cout << "Type = " << type << endl;
    size -= type;
     for(auto& au:file.au) {
        ptr = image.tos_au(au) + type;
        tsize = image.get_au() - type;
        if (size>=tsize) {
            outfile.write(ptr,tsize);
            size-=tsize;
        } else {
            outfile.write(ptr,size);
        }
        type = 0;
    }
    outfile.close();
    cout << "o : File " << file.name << " saved as " << name_to << endl;

}

void command_sread::exec(list<string> params){
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
    if (find_file(file,name_from)) {

    } else {
        cout << "E : File not found." << endl;
    }
    tos_to_os(name_to);
    exec(file,name_to);
}

void command_sread::help(void){
    cout << " -sread <tosname> [<filename>]" << endl;
    cout << "	Strip header from a single tos file and save it to local file." << endl;
}
int command_sread::min_params() { return 1; }
int command_sread::max_params() { return 2; }
