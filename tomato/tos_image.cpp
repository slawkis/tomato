#include "tos_image.hpp"

tos_dir::tos_dir(int pa, string n, string e, int p, int t, int s, int h, int r, array<unsigned char,16> &b) {
    parent = pa;
    name = n;
    ext = e;
    part = p;
    tail = t;
    size = s;
    hidden = h;
    readonly = r;
    blocks = b;
}

tos_dir::tos_dir(int pa, string n, string e, int p, int t, int s, int h, int r, unsigned char b)  {
    parent = pa;
    name = n;
    ext = e;
    part = p;
    tail = t;
    size = s;
    hidden = h;
    readonly = r;
    blocks.fill(b);
}

int             tos_image::mkdir(string iname) {
    int id,entry;
    id = *iname.c_str() &  0xff;
    if (iname.substr(9)!="DIR") {
        iname = iname.substr(0,9) + "DIR";
    }
    if (id && (!dir_exists(id))) {
        cout << "E : Cannot find parent folder ( id = "<<id<<" )."<<endl;
        return 0;
    }
    if (dir_exists(iname)) {
        cout << "E : Directory already exists."<<endl;
        return 0;
    }
    if (file_exists(iname.substr(0,9)+"   ")) {
        cout << "E : Directory and file cannot share the name."<<endl;
        return 0;
    }
    entry = static_cast<int>(find_free_entry());
    id    = static_cast<int>(find_free_id());
    if (!id) {
        cout << "E : cannot find free directory ID" << endl;
    }

    memset(&dir[entry].parent,0,32);
    dir[entry].blocks[0]=id&0xff;
    for (int i = 1; i<16; i++) { dir[entry].blocks[i]=0; }
    iname[0]|=0x80;
    memcpy(&dir[entry].parent,iname.c_str(),12);
    return 1;
}

int             tos_image::move(string from, string to) {
    int id;
    string tname;
    if (!file_exists(from)) {
        cout << "E : file doesnt exists." << endl;
        return 0;
    }
    if (file_exists(to)) {
        cout << "E : file already exists." << endl;
        return 0;
    }
    id = to[0]&0xff;
    if (id && (!dir_exists(id))) {
        cout << "E : directory ID "<<id<<" doesn't exists." << endl;
        return 0;
    }
    for (int i=1; i < 128; i++ ){
        tname=string(reinterpret_cast<char*>(&dir[i].parent),12);
        if (from == tname) {
            memcpy(reinterpret_cast<char*>(&dir[i].parent),to.c_str(),12);
        }
    }
    return 1;
}

unsigned long tos_image::find_free_id(void) {
    unsigned long id;
    for (unsigned long j=1; j<16; j++) {
        id = j;
        for (int i=1; i<128; i++) {
            if ((dir[i].parent & 0x80) && (dir[i].blocks[0] == (id&0xff))) {
                id=0;
                break;
            }
        }
        if (id) { return id; }}
    return 0;
}

int             tos_image::dir_exists(string iname) {
    string dname;
    for (int i=1; i<128;i++){
        dname=string(reinterpret_cast<const char*>(&dir[i].parent),12);
        if (dname == iname) { return 1;}
    }
    return 0;
}

int             tos_image::dir_exists(int id){
    for (int i=1; i<128;i++){
        if ((dir[i].parent&0x80)&&(dir[i].blocks[i]==(id&0xff))) { return 1;}
    }
    return 0;
}

int             tos_image::file_exists(string iname) {
    string tname;
    for (int i=1; i < 128; i++ ){
        tname=string(reinterpret_cast<char*>(&dir[i].parent),12);
        tname.at(9)&=0x7f;
        tname.at(10)&=0x7f;
        tname.at(11)&=0x7f;
        if (iname == tname) return 1;
    }
    return 0;
}

void    tos_image::find_free_au(void) {
    unsigned long au;
    int last_au,first_au;

    free_au.clear();
    free_au.push_back(0);
    if (au_size == 1024) {
        free_au.push_back(0);
        free_au.push_back(0);
        free_au.push_back(0);
        last_au  = 144;
        first_au = 4;
    }
    else {
        last_au  = 156;
        first_au = 1;
    }
    for (int i = first_au; i<last_au;i++) {
        free_au.push_back(i);
    }
    for (int i = 1; i<128;i++) {
        if (!(dir[i].parent&0x80)) {
            for (int j=0; j<16; j++) {
                au=dir[i].blocks[j];
                if (au) { free_au[au]=0;} else { break;}
            }
        }
    }
//    vector<int>::reverse_iterator rit = free_au.begin();
//    for (; rit!= free_au.crend(); ++rit) {
    for (auto rit = free_au.end()-1; rit != free_au.begin()-1; rit--) {
        if (*rit == 0) { free_au.erase(rit); }
    }
}

unsigned long     tos_image::find_free_entry(void) {
    for (unsigned long i=1; i<128; i++)
        if (dir[i].parent == 0xe5) { return i;}
    return 0; //shouldn't happen. Har, har, har...
}

int             tos_image::create_file(string iname,char*data,long size) {
    unsigned long last_au, current_au, dir_entry, sectors;

    int part;
    find_free_au();
    if (static_cast<long>((free_au.size() * static_cast<unsigned long>(au_size))) < size) {
        cout << "E : disk full." << endl;
        return 0;
    }
    last_au = static_cast<unsigned long>(floor(size/static_cast<double>(au_size)));
    current_au = 0;
    part = 0;
    while(current_au<=last_au) {
        dir_entry=find_free_entry();
        memcpy(&dir[dir_entry].parent,iname.c_str(),12);
        dir[dir_entry].part = part&0xff;
        if (size>16*au_size) {
            dir[dir_entry].tail=0;
            dir[dir_entry].sizeLo = (au_size==1024)?0x80:0x00;
            dir[dir_entry].sizeHi = (au_size==1024)?0x00:0x02;
            size-=(16*au_size);
        } else {
            sectors=static_cast<unsigned int>(ceil(size/256.0)*2);
            dir[dir_entry].sizeLo=sectors&0xff;
            dir[dir_entry].sizeHi=(sectors>>8)&0xff;
            dir[dir_entry].tail=size&0xff;
        }
        for (int i = 0; i<16 ; i++) {
            if(current_au<=last_au) {
                dir[dir_entry].blocks[i]=free_au[current_au]&0xff;
                memcpy(tos_au(free_au[current_au]&0xff),data,static_cast<unsigned long>(au_size));
                data+=au_size;
                current_au++;
            } else {
                dir[dir_entry].blocks[i]=0;
            }
        }
        part++;
    }
    return 1;
}

int             tos_image::erase_dir(string name) {
    int id = 0;
    string tname;
//    if (!(*name.c_str() & 0x80 )) { return 0;}
    for (int i=1; i < 128; i++ ){
        tname=string(reinterpret_cast<char*>(&dir[i].parent),12);
        tname.at(9)&=0x7f;
        tname.at(10)&=0x7f;
        tname.at(11)&=0x7f;
        if (name == tname) {
            id=*dir[i].blocks&0xff;
            memset(&dir[i].parent,0xe5,32);
        }
    }
    return id;

}

int             tos_image::erase_file(string name) {
    int deleted = 0;
    string tname;
//    if (*name.c_str() & 0x80 ) { return 0;}
    for (int i=1; i < 128; i++ ){
        tname=string(reinterpret_cast<char*>(&dir[i].parent),12);
        tname.at(9)&=0x7f;
        tname.at(10)&=0x7f;
        tname.at(11)&=0x7f;
        if (name == tname) {
            memset(&dir[i].parent,0xe5,32);
            deleted++;
        }
    }
    return deleted;
}

int             tos_image::erase(string name) {
    int id;
    string tname;
    if ( (name.substr(name.length()-3) == "DIR") /*&& (*name.c_str() & 0x80)*/ ) {
        id = erase_dir(name);
        for (int i=1; i < 128; i++ ){
            if (dir[i].parent == id){ memset(&dir[i].parent,0xe5,32); }
            else if ((dir[i].parent&0x7f) == id) {
                tname=string(reinterpret_cast<char*>(&dir[i].parent),12);             
                erase(tname);
            }
        }
    } else {
        id = erase_file(name);
    }
    return id;
}

char * tos_image::tos_sector(int sector) {
    return reinterpret_cast<char*>(img) + sector * 256;
}

char * tos_image::tos_sector(int track, int sector){
    return reinterpret_cast<char*>(img) + track * 4096 + sector * 256;
}

char * tos_image::tos_isector(int sector) {
    return reinterpret_cast<char*>(img) + (sector>>4) * 4096 + trackSkew[sector%16] * 256;
}

char * tos_image::tos_isector(int track, int sector){
    return reinterpret_cast<char*>(img) + track * 4096 + trackSkew[sector] * 256;
}

char * tos_image::tos_track(int track){
    return reinterpret_cast<char*>(img) + track * 4096;
}

char * tos_image::tos_au(int au){
    return reinterpret_cast<char*>(img) + au * au_size;
}

tos_dir&        tos_image::get_dir(int i){
    entry.parent = dir[i].parent;
    entry.name   = string(dir[i].name,8);
    entry.ext    = string(dir[i].ext,3);
    if (dir[i].ext[1]&0x80) { entry.hidden = 1;   entry.ext[1]&=0x7f;} else { entry.hidden = 0;}
    if (dir[i].ext[0]&0x80) { entry.readonly = 1; entry.ext[0]&=0x7f;} else { entry.readonly = 0;}

    entry.part   = dir[i].part;
    entry.size   = dir[i].sizeHi*256+dir[i].sizeLo;
    entry.tail   = dir[i].tail;
    memcpy(entry.blocks.data(),dir[i].blocks,16);
    return entry;
}

void            tos_image::set_dir(int i, tos_dir& d) {
    transform(d.name.begin(), d.name.end(), d.name.begin(), [](char c){ return toupper(c); });
    transform(d.ext.begin(),  d.ext.end(),  d.ext.begin(),  [](char c){ return toupper(c); });
    d.name+="        ";
    d.ext+="   ";
    dir[i].parent = d.parent & 0xff;
    memcpy(dir[i].name,d.name.c_str(),8);
    memcpy(dir[i].ext,d.ext.c_str(),3);
    if(d.hidden){dir[i].ext[1]|=0x80; }
    if(d.readonly){dir[i].ext[0]|=0x80; }
    dir[i].part = d.part & 0xff;
    dir[i].sizeHi = (d.size / 0x100) & 0xff;
    dir[i].sizeLo = d.size & 0xff;
    dir[i].tail = d.tail & 0xff;
    memcpy(dir[i].blocks,d.blocks.data(),16);
}

void            tos_image::set_side(int s) {
    if ((s>=0)&&(s<=1)) {
        side = s;
        img  = image + 80*4096*side;
        dir  = reinterpret_cast<dirEntry*>(image+ 80*4096*side) ;
    }
}

void            tos_image::set_au(int s){
    au_size = s;
}

int             tos_image::get_au(void) {
    return au_size;
}

void            tos_image::init(void) {
    memset(image,0xe5,160*4096);
    img = image;
    side = 0;
    au_size = 1024;
}
