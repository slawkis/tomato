#ifndef COMMAND_HPP_INCLUDED
#define COMMAND_HPP_INCLUDED

#include <array>
#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <map>

#include "tos_image.hpp"

using namespace std;

#pragma pack(push,1)

typedef struct s_header {
    uint8_t     track;
    uint8_t     side;
    uint8_t     sector_id;
    uint8_t     sector_size;
    uint8_t     status_1;
    uint8_t     status_2;
    uint16_t    data_size;
} s_header;

typedef struct t_header {
    char        ID [13];
    uint8_t     unused[3];
    uint8_t     track;
    uint8_t     side;
    uint16_t    unused2;
    uint8_t     sector_size;
    uint8_t     no_of_sectors;
    uint8_t     GAP_len;
    uint8_t     filler;
    s_header    sectors[29];
    uint8_t     data[4096];
} t_header;

typedef struct d_header {
    char        ID [34];
    char        creator[14];
    uint8_t     tracks;
    uint8_t     sides;
    uint16_t    track_size;
    uint8_t     size_table[204];
    t_header    track[80];
} d_header;

#pragma pack(pop)

class img_format {
public:
    string	name;
    int		size;
    int		tracks;
    int		system;
    int		first_cat;
    int		second_cat;
    int		sides;
};

class command {
protected:
    const static unsigned char tos2a[];
          static char tos2ext[16384];
    const static string initial_name;
          static string last_name;
    const static list<img_format> format_list;
          static img_format current_format;
          static int current_side;
          static tos_image image;

protected:
    virtual int min_params() { return 0; }
    virtual int max_params() { return 0; }
           void upcase(string& s);
           void lowcase(string& s );
           void tos_to_os(string &s);
           void os_to_tos(string &s);
           void   os_to_internal(string os, string &dir);
           string os_to_internal(string os);
           void replace_all(string &str, const string& from, const string& to);
           string rtrim(string s);
public:
    void process(list<string> params) ;
    virtual void exec(list<string> params) ;
    virtual void help(void) ;

    virtual ~command(){;}
};

#endif //COMMAND_HPP
