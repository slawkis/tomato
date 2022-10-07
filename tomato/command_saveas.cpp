#include "command_saveas.hpp"

using namespace std;

void command_saveas::save_raw(string name) {
    fstream file;
    const char * data;
    int size,sector;

    if (current_format.sides) {
        cout << "E : cannot write to double sided images. Yet."<<endl;
        return;
    }

    file.open(name,ios::out | ios::trunc | ios::binary);
    if (!file.is_open()) {
        cout << "E : Cannot create file "<<name<<endl;
        return;
    }
    size = current_format.tracks * 16;

    if (tos2ext[0]) {
        file.write(tos2ext,16384);
        size -= 4*16;
    }

    sector = 0;

    while(sector < size) {
        for (int i=0; i < 16; i++) {
            data = image.tos_sector(sector+i);
            file.write(data,256);
        }
        sector+=16;
    }
    file.close();
    cout << "o : current image saved as " << name << endl;
}


void command_saveas::save_img(string name) {
    fstream file;
    const char * data;
    int size,sector;

    if (current_format.sides) {
        cout << "E : cannot write to double sided images. Yet."<<endl;
        return;
    }

    file.open(name,ios::out | ios::trunc | ios::binary);
    if (!file.is_open()) {
        cout << "E : Cannot create file "<<name<<endl;
        return;
    }
    size = current_format.tracks * 16;

    if (tos2ext[0]) {
        file.write(tos2ext,16384);
        size -= 4*16;
    }

    sector = 0;

    while(sector < size) {
        for (int i=0; i < 16; i++) {
            data = image.tos_isector(sector+i);
            file.write(data,256);
        }
        sector+=16;
    }
    file.close();
    cout << "o : current image saved as " << name << endl;
}

void command_saveas::save_dsk(string name, int edsk){
    char buf[1024*1024];
    char * bptr;
    char * dptr;
    fstream file;

    if (current_format.sides) {
        cout << "E : cannot write to double sided images. Yet."<<endl;
        return;
    }

    if (edsk) {
        name = name.substr(0,name.length()-4) + "dsk";
    }
    file.open(name,ios::out | ios::binary | ios::trunc);
    if (!file.is_open()) {
        cout << "E : Canot open file " << name << endl;
        return;
    }

    bptr = buf;
    if (current_format.system) {
        memcpy(bptr,tos2ext,16384);
        bptr += 16384;
    }


    for (int t = 0; t < current_format.tracks; t++){
        for (int s=0; s<16; s++) {
            dptr = image.tos_isector(t,s);
            memcpy(bptr,dptr,256);
            bptr += 256;
        }
    }

    char dsk[512] = {0};
    int tracks = current_format.tracks;
    int sides=1;
    if(tracks > 80) { tracks /= 2; sides = 2; }

    d_header *dh=reinterpret_cast<d_header*>(dsk);
    if (edsk) {
        memcpy(dh->ID,string("EXTENDED CPC DSK File\r\nDisk-Info\r\n").c_str(),34);
    } else {
        memcpy(dh->ID,string("MV - CPCEMU Disk-File\r\nDisk-Info\r\n").c_str(),34);
    }
    memcpy(dh->creator,string("TOMATO v0.1\0\0\0\0").c_str(),14);

    dh->tracks = tracks & 0xff;
    dh->sides  = sides & 0xff;
    if (edsk) {
        dh->track_size = 0x0000;
        for (int i = 0; i< tracks*sides; i++) {dh->size_table[i]=0x11;}
    } else {
        dh->track_size = 0x1100;
    }
/*
    fstream d;
    d.open("dump.dsk",ios::out | ios::binary | ios::trunc);
    d.write(buf,640*1024);
    d.close();

    d.open("dump2.dsk",ios::out | ios::binary | ios::trunc);
    d.write(image.tos_au(0),current_format.size);
    d.close();
*/
    file.write(dsk,256); // disk header

    memcpy(dh->track[0].ID,string("Track-Info\r\n").c_str(),13);
    dh->track[0].sector_size = 1;
    dh->track[0].GAP_len = 0x2a;
    dh->track[0].filler = 0xe5;
    dh->track[0].no_of_sectors = 0x10;
    dh->track[0].unused2 = 0x0201;
    bptr = buf;
    for (int t = 0; t < tracks; t++) {
        dh->track[0].track = t & 0xff;
        for (int i = 0; i < sides ; i++) {
            dh->track[0].side = i & 0xff;
            for (int s = 0; s < 16; s++) {
                dh->track[0].sectors[s].track = t & 0xff;
                dh->track[0].sectors[s].side = 0;
                dh->track[0].sectors[s].sector_id = s & 0xff;
                dh->track[0].sectors[s].sector_size = 0x01;
                dh->track[0].sectors[s].data_size =  0x100;
            }
            file.write(dsk+256,256);    // track header;
            file.write(bptr,4096);       // track data;
            bptr+=4096;
        }
    }
    file.close();
    cout << "o : current image saved as " << name << endl;
}

// tape header:
// type name[10] len autostart baslen
// type name[10] len start 32768

// tape block
// size id==0x00 <header> checksum
// size id==0xff < data > checksum

//TOS header
// type autostart len baslen
// type len start

void command_saveas::save_tap(string name) {
    unsigned char data[1024*1024];
    unsigned char *optr, *iptr;
    unsigned long size,tsize,delta;
    unsigned long checksum;
    unsigned long hstart, hlen, hbas, type;
    fstream file;
    string tname;

    file.open(name,ios::out | ios::binary | ios::trunc);
    if (!file.is_open()) {
        cout << "E : Cannot create file "<<name<<endl;
        return;
    }

    dir_import();
    for (auto& f: files) {
        memcpy(data,image.tos_au(f.au[0]),20);
        type=data[0]&0xff;
//        size=u-int(f.size);
        if(type==0) { // basic
            hstart=256*(data[2]&0xff)+(data[1]&0xff);
            hlen  =256*(data[4]&0xff)+(data[3]&0xff);
            hbas  =256*(data[6]&0xff)+(data[5]&0xff);
//            size-=7;
            delta=7;
        } else if (type<5) { // vars&code
            hstart=256*(data[4]&0xff)+(data[3]&0xff);
            hlen  =256*(data[2]&0xff)+(data[1]&0xff);
            hbas  =32768;
//            size-=5;
            delta=5;
        } else {    // binary
            hlen  = f.size&0x00ffffff;
            hstart= 65536-hlen;
            hbas  = 32768;
            delta = 0;
        }
        data[0]=19;
        data[1]=0;
        data[2]=0; //header;
        data[3]=type&0xff;
        tname=f.name+"            ";
        tname=tname.substr(0,12);
        memcpy(data+4,tname.c_str(),10);
        data[14]=hlen&0xff;
        data[15]=(hlen>>8)&0xff;
        data[16]=hstart&0xff;
        data[17]=(hstart>>8)&0xff;
        data[18]=hbas&0xff;
        data[19]=(hbas>>8)&0xff;
        checksum=0;
        for(int i=2;i<20;i++){
            checksum^=data[i];
        }
        data[20]=checksum&0xff;

        size  = ((f.size)&0x00ffffff)-delta+2;
        data[21]=size&0xff;
        data[22]=(size>>8)&0xff;
        data[23]=0xff;
        optr=data+24;

        size  = (f.size)&0x00ffffff;
        tsize = image.get_au()&0x00ffffff;
        for(auto& au:f.au) {
            iptr = reinterpret_cast<unsigned char*>(image.tos_au(au));
            if (size>=tsize) {
                memmove(optr,iptr,tsize);
                size-=tsize;
                optr+=tsize;
            } else {
                memmove(optr,iptr,size);
            }
        }
        size  = f.size&0x00ffffff;
        if(delta) {
            memmove(data+24,data+24+delta,size);
            size=size-delta;
        }
        checksum=0;
        for(unsigned long i=23;i<23+size+1;i++){
           checksum^=data[i];
        }
        data[24+size]=checksum&0xff;
        file.write(reinterpret_cast<char*>(data),long(size+25));
        file.flush();
        cout << "o : File «"<<f.name<<"» exported."<<endl;
    }
    file.close();
    cout << "TAP file «" << name << "» created."<<endl;
}

void command_saveas::exec(string name) {
    string ext=name.substr(name.length()-4);
    if (ext==".dsk")      { save_dsk(name,0); }
    else if (ext=="edsk") { save_dsk(name,1);}
    else if (ext==".img") { save_img(name); }
    else if (ext==".raw") { save_raw(name); }
    else if (ext==".tap") { save_tap(name); }
    else {
        cout << "E : unknown file type : " << name << endl;
    }
}


void command_saveas::exec(list<string> params){
    string name=params.front();
    exec(name);
}

void command_saveas::help(void){
    cout << " -saveas <filename.[img|edsk|dsk|tap]>" << endl;
    cout << "	Save current image to given file." << endl;
}
int command_saveas::min_params() { return 1; }
int command_saveas::max_params() { return 1; }
