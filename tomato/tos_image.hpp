#ifndef TOS_IMAGE_HPP
#define TOS_IMAGE_HPP

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>

using namespace std;

typedef struct dirEntry {
    unsigned char parent;
             char name[8];
             char ext[3];
    unsigned char part;
    unsigned char tail;
    unsigned char sizeHi;
    unsigned char sizeLo;
    unsigned char blocks[16];
} __attribute__((aligned(4),packed)) dirEntry;

class tos_dir {
public:
    int     parent;
    string  name;
    string  ext;
    int     part;
    int     tail;
    int     size;
    int     hidden;
    int     readonly;
    array<unsigned char,16> blocks;
    tos_dir(){}
    tos_dir(int pa, string n, string e, int p, int t, int s, int h, int r, unsigned char b);
    tos_dir(int pa, string n, string e, int p, int t, int s, int h, int r,array<unsigned char,16>& b);
};

class tos_image
{
private:
    unsigned char image[160*4096];
    unsigned char * img = image;
    int           au_size;
    int           side;
//    int           delta_t;
    dirEntry      * dir  = reinterpret_cast<dirEntry*>(image) ;
    tos_dir       entry;
    const int trackSkew [16] = {0,7,14,5, 12,3,10,1, 8,15,6,13, 4,11,2,9} ;
    vector <int>  free_au;

private:
    void    find_free_au(void);
    unsigned long find_free_entry(void);
    unsigned long find_free_id(void);

public:
    tos_image(){}
    char * tos_sector(int sector);
    char * tos_sector(int track, int sector);
    char * tos_isector(int sector);
    char * tos_isector(int track, int sector);
    char * tos_track(int track);
    char * tos_au(int au);
    tos_dir&        get_dir(int i);
    void            set_dir(int i, tos_dir& d);
    void            set_side(int s = 0);
    void            set_au(int s = 1024);
    int             get_au(void);
    int             erase(string name);
    int             erase_file(string name);
    int             erase_dir(string name);
    int             file_exists(string iname);
    int             create_file(string iname,char*data,long size);
    int             dir_exists(string iname);
    int             dir_exists(int id);
    int             mkdir(string iname);
    int             move(string from, string to);
    void            init(void);
//    void            set_delta(int s = 4);
};

#endif // TOS_IMAGE_HPP
