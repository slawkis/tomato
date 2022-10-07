#include "command_open.hpp"
#include "command_new.hpp"

using namespace std;



long command_open::fsize(string name){
    struct stat stat_buf;
    int rc = stat(name.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

int command_open::is_dsk(string name){
    fstream file;
    string  id;
    long size = fsize(name); // TODO: zmienić na iterację po wielkościach z tabeli formatów
    if ( (size != 174336) && // tos160
         (size != 348416) && // tos160ds
         (size != 696576) && // tos640
         (size != 183040) && // tos4
         (size != 365824)) { // tos4ds
        return 0;
    }
    char buf[512];
    file.open(name,ios::in | ios::binary);
    if (!file.is_open()) {
        cout << "E : Canot open file " << name << endl;
        return 0;
    }
    file.read(buf,512);
    file.close();
    d_header *dh=reinterpret_cast<d_header*>(buf);
    id = string(dh->ID,34);
    if( (id != "EXTENDED CPC DSK File\r\nDisk-Info\r\n") &&
        (id != "MV - CPCEMU Disk-File\r\nDisk-Info\r\n")) {
        return 0;
    }
    if ((dh->track[0].no_of_sectors != 16)||
            (dh->track[0].filler  != 0xe5)||
            (dh->track[0].sector_size != 1) ) {
        return 0;
    }
    return 1;
}

int command_open::is_img(string name){
    fstream file;
    string  id;
    long size = fsize(name); // TODO: zmienić na iterację po wielkościach z tabeli formatów
    if ( (size !=  40*4096) && // tos160
         (size !=  80*4096) && // tos160ds
         (size != 160*4096) && // tos640
         (size !=  42*4096) && // tos4
         (size !=  84*4096)) { // tos4ds
        return 0;
    }
    size/=4096;
    char buf[4096];
    file.open(name,ios::in | ios::binary);
    if (!file.is_open()) {
        cout << "E : Canot open file " << name << endl;
        return 0;
    }
    if ((size != 42) && (size != 84)) { // TOS2
        file.read(buf,4096);
        if ( (buf[0x005] != 'P') ||
             (buf[0x19e] != '@') ||
             (buf[0xf32] != '2') ||
             (buf[0x5aa] != '%') ) { file.close(); return 0; }
        if (size == 80) {  file.seekg(0x8000);  } else { file.seekg(0x4000); }
    } else { file.seekg(0x00); } // TOS4
    file.read(buf,512);
    file.close();
    if ((buf[0] != -1) && (string(buf+8,3) != "DIR")) return 0;
    return 1;

}


void command_open::open_dsk(string name){
    fstream file;
    char buf[256];
    int size;
    img_format backup = current_format;
    file.open(name,ios::in | ios::binary);
    file.read(buf,256);
    d_header *dh=reinterpret_cast<d_header*>(buf);
    size = dh->tracks*dh->sides*4096;
    current_format.name = "";
    for (auto it: format_list) {
        if(it.size == size) { current_format = it; break;}
    }
    if (current_format.name == ""){
        current_format = backup;
        cout << "E : cannot find valid format for file " << name;
        return;
    }
    image.init();
    if(current_format.tracks > 80) { image.set_au(4096); }

    int start = 0;
    if (current_format.system) {
        start = 4;
        for(int t=0; t<start; t++) {
            file.read(buf,256);
            file.read(tos2ext + t * 4096,4096);
            if (current_format.sides) {
                file.read(buf,256);
                file.read(tos2ext + t * 4096,4096);
            }

        }
    }
    for (int i = 0; i < current_format.tracks - 1 - start; i++) {
        file.read(buf,256);
        for (int s = 0; s< 16; s++) {
            file.read(image.tos_isector(i,s),256);
        }
        if (current_format.sides) {
            image.set_side(1);
            file.read(buf,256);
            for (int s = 0; s < 16; s++) {
                file.read(image.tos_isector(i,s),256);
            }
            image.set_side(0);
        }
    }
    file.close();

    cout << "o : Opened file " << name << endl;
}

void command_open::open_img(string name){
    fstream file;
//    char buf[256];
    long size = fsize(name);
    img_format backup = current_format;
    int raw_format=0;

    current_format.name = "";
    for (auto it: format_list) {
        if(it.size == size) { current_format = it; break;}
    }
    if (current_format.name == ""){
        current_format = backup;
        cout << "E : cannot find valid format for file " << name;
        return;
    }
    if (name.substr(name.length()-3)=="raw") {
        raw_format=1;
    }
    image.init();
    if(current_format.tracks > 80) { image.set_au(4096); }
    
    file.open(name,ios::in | ios::binary);

    if (current_format.system) {
        char * ptr = tos2ext;
        for (int t = 0; t < 4; t++) {
            file.read(ptr,4096);
            if (current_format.sides) file.read(ptr,4096);
            ptr+=4096;
        }
    }

    for (int i = 0; i < (current_format.tracks - (4 * current_format.system)) ; i++) {
        for (int s = 0; s < 16; s++) {
            if (raw_format) {
                file.read(image.tos_sector(i,s),256);
            }else {
                file.read(image.tos_isector(i,s),256);
            }
        }
        if (current_format.sides) {
            image.set_side(1);
            for (int s = 0; s < 16; s++) {
                if (raw_format) {
                    file.read(image.tos_sector(i,s),256);
                }else {
                    file.read(image.tos_isector(i,s),256);
                }
            }
            image.set_side(0);
        }
    }

    file.close();
    cout << "o : Opened file " << name << endl;
/*
    file.open("dump.tos",ios::out | ios::binary | ios::trunc);
    file.write(tos2ext,16384);
    file.close();

    file.open("dump.body",ios::out | ios::binary | ios::trunc);
    file.write(image.tos_au(0),640*1024);
    file.close();
*/
}

int command_open::is_tap(string name) {
    fstream file;
    int lo,hi;
    long size,csize;
    string tmpname;

    tmpname=name;
    lowcase(tmpname);
    if (tmpname.substr(tmpname.length()-3)!="tap") {
        return 0;
    }

    file.open(name,ios::in | ios::binary);
    if (!file.is_open()) {
        return 0;
    }
    size=fsize(name);
    csize=0;
    while(csize<size) {
        lo=file.get();
        hi=file.get();
        csize+=2+lo+hi*256;
        file.seekg(csize);
    }
    file.close();
    if (csize!=size) { return 0;}
    return 1;
}

//int command_open::is_tzx(string name) {
//}

void command_open::open_tap(string name){
    fstream file;
    string bname,iname;
    unsigned long lo,hi,sum;
    unsigned long len,start,bass;
    int type;
    unsigned long count;
    unsigned long size,csize;
    unsigned long bsize;
    unsigned char data[1024*1024];

    file.open(name,ios::in | ios::binary);
    if (!file.is_open()) {
        return;
    }
    size=fsize(name)&0x00ffffff;

    if (current_format.name=="") {
        list<string>f;
        for (auto&fmt:format_list){
            if (((fmt.tracks&0xff) * 4096) > size ){
                f.clear();
                f.push_back(fmt.name);
                break;
            }
        }
        if (!f.size()) {
            cout << "TAP too long. No valid IMG format." << endl;
            return;
        }
        command_new n;
        n.exec(f);
    }

    csize=0;
    type=-1;
    len=start=bass=0;
    count=0;
    while(csize<size) {
        lo=static_cast<unsigned long>(file.get());
        hi=static_cast<unsigned long>(file.get());
        bsize = lo+hi*256;
        if (bsize > (1024*1024)) {
            cout << "E : block too long. I'm impressed..."<< endl;
            file.close();
            return;
        }
        file.read(reinterpret_cast<char*>(data),static_cast<long>(bsize));
        sum=0;
        for (unsigned int i=0; i<bsize; i++) {
            sum^=data[i];
        }
        if (sum!=0){
            file.close();
            cout << "E : Invalid checksum." << endl;
            return;
        }
        if (data[0]==0) {               // header
            bname=string(reinterpret_cast<char*>(data)+2,10);
            len  =data[12]+data[13]*256;
            start=data[14]+data[15]*256;
            bass =data[16]+data[17]*256;
            type =data[1];
        } else if (data[0]==0xff ){     //data
            if(type==-1) {              // headerless data
                bname="0000"+to_string(count);
                bname=bname.substr(bname.length()-4);
                bname="DATA"+bname;
                len=bsize-2;
                start=65536-len;
                bass=32768;
                type=3;
            }
            if (type == 0) {
                memmove(data+7,data+1,size&0xfffff);
                data[0]=type&0xff;
                data[1]= start&0xff;
                data[2]=(start>>8)&0xff;
                data[3]= len&0xff;
                data[4]=(len>>8)&0xff;
                data[5]= bass&0xff;
                data[6]=(bass>>8)&0xff;
                len+=7;
            } else {
                memmove(data+5,data+1,size&0xfffff);
                data[0]=type&0xff;
                data[1]= len&0xff;
                data[2]=(len>>8)&0xff;
                data[3]= start&0xff;
                data[4]=(start>>8)&0xff;
                len+=5;
            }

            os_to_internal(bname,iname);
            if (image.file_exists(iname)) {
                int i = 0;
                string nname;
                do {
                    nname="000"+to_string(i);
                    nname=bname+"."+nname.substr(0,3);
                    os_to_internal(nname,iname);
                    i++;
                } while(image.file_exists(iname) && (i<1000));
                if (i>=1000) {
                    cout << "E : cannot convert the zx name «"<<bname<<"»."<<endl;
                }
            }
            if (image.create_file(iname,reinterpret_cast<char*>(data),static_cast<long>(len)) ) {
                cout << "o : File «" << bname << "» was imported"<<endl;
            }
            type=-1;
        } else {                        // custom block. Not yet.
            file.close();
            cout << "E : custom tape block." << endl;
            return;
        }


        csize+=bsize+2;
    }
    file.close();

}
//void command_open::open_tzx(string name) {
//}


void command_open::exec(string name){
    if      (is_dsk(name)) { open_dsk(name); }
    else if (is_img(name)) { open_img(name); }
    else if (is_tap(name)) { open_tap(name); }
//    else if (is_tzx(name)) { open_tzx(name); }
    else {
        cout << "E : unknown file type : " << name << endl;
    }
}

void command_open::exec(list<string> params){
    string name=params.front();
    exec(name);
}

void command_open::help(void){
    cout << " -open <filename.[img|dsk|edsk]>" << endl;
    cout << "	Open an image." << endl;
}
int command_open::min_params() { return 1; }
int command_open::max_params() { return 1; }
