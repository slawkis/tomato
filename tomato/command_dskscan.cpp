#include "command_dskscan.hpp"

using namespace std;

void command_dskscan::exec(list<string> params){
    string name = params.front();
    string creator;
    int format;
    int size;
    int csize;
    int dsize;
    int sectors;

    cout << endl << "----====    dskinfo    ====----" << endl << endl;
    fstream file;
    file.open(name,ios::in | ios::binary);
    if (!file.is_open()) {
        cout << "E : Cannot open file «"<<name<<"»"<< endl<<endl;
        return;
    }
    file.read(buf,256);
    cout << "File name : " << name << endl;
    cout << "Format    : ";
    if      (string(dh->ID,34) == h_DSK)  { cout << "DSK"  << endl; format = 0; }
        else if (string(dh->ID,34) == h_EDSK) { cout << "EDSK" << endl; format = 1; }
        else { cout << "Unknown." << endl << endl; file.close(); return; }
    creator = string(dh->creator,14);
    cout << "Creator   : " << creator.c_str() << endl;
    cout << "Sides     : " << static_cast<int>(dh->sides) << endl;
    cout << "Tracks    : " << static_cast<int>(dh->tracks) << endl;

    cout << endl << "----====   trackinfo   ====----" << endl;
    for (int i=0; i<static_cast<int>(dh->tracks) * static_cast<int>(dh->sides) ; i++) {
    dsize = (format)?((static_cast<int>(dh->size_table[i])&0xff)-1)*256:static_cast<int>(dh->track_size)-256;
    file.read(buf+256,256);
    if (file.bad() || file.eof()) {
        file.close();
        cout << endl << "E : [EOF!] No more tracks to read. (should be "<< static_cast<int>(dh->tracks) - i << " more)" << endl << endl;
        return;
    }
    if (string(th->ID,12) != h_TRACK) {
        file.close();
        cout << endl << endl << "E : DSK internal error. Track size doesn't match." << endl << endl;
        return;
    }
    csize = 0;
    cout  << endl<< "Track "<< setw(2) << i << ":" << endl;
    cout << "  size: " << dsize;
    cout << ", trk: " << setw(2)<<  static_cast<int>(th->track);
    cout << ", side: " << static_cast<int>(th->side);
    size = static_cast<int>(th->sector_size);
    size = int(pow(2,size+7));
    cout << ", sector size: " << setw(4) << size ;
    cout << ", GAP: 0x" << hex << setw(2) << setfill('0') << static_cast<int>(th->GAP_len);
    cout << ", filler: 0x" << hex << setw(2) << static_cast<int>(th->filler) << dec << setfill(' ');
    sectors = static_cast<int>(th->no_of_sectors);
    cout << ", sectors: " << sectors << endl;
    for (int s = 0; s < sectors; s++) {
        cout << "    Sector ID: " << setw(3) << static_cast<int>(th->sectors[s].sector_id);
        cout << ", trk: " << setw(2)<<  static_cast<int>(th->sectors[s].track);
        cout << ", side: " << setw(1)<<  static_cast<int>(th->sectors[s].side);
        size = static_cast<int>(th->sectors[s].sector_size);
        size = int(pow(2,size+7));
        cout << ", size: " << setw(4) <<  size;
        csize+=size;
        size = static_cast<int>(th->sectors[s].data_size);
        cout << ", data size: " << setw(4) <<  size;
        cout << endl;
    }
    if (csize != dsize) {
        cout << "o :     [!] header size != calculated size : " << dsize << " / " << csize << endl;
    }
        file.read(buf+256,dsize);
    }

    cout << endl;
}

void command_dskscan::help(void){
    cout << " -dskscan <filename>" << endl;
    cout << "	Shows internal structure of a dsk image." << endl;
}

int command_dskscan::min_params() { return 1; }
int command_dskscan::max_params() { return 1; }
