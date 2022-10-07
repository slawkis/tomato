#include "command.hpp"

using namespace std;

void command::exec(list<string> params){ (void) params;}
void command::help(void){;}

string command::rtrim(string s){
    size_t found = s.find_last_not_of(" ");
    if (found!=std::string::npos) s.erase(found+1);
    return s;
}

void command::process(list<string> params){
    int count = int(params.size());
    string nums;
    if ((count >= min_params())&&(count <= max_params())) { 
	exec(params);
    } else {
	(max_params() == min_params())? nums = to_string(max_params()) : nums = to_string(min_params()) + " to " + to_string(max_params());
	cout << "E : this command requires "<< nums << " parameter(s). Got " << count << ". Skipped." << endl;
    }
}

void command::upcase(string& s ){
    transform(s.begin(), s.end(), s.begin(), [](char c){ return toupper(c); });
}

void command::lowcase(string& s ){
    transform(s.begin(), s.end(), s.begin(), [](char c){ return tolower(c); });
}

void command::replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
}

void command::tos_to_os(string& s) {
    /*
    replace( s.begin(), s.end(), '*', '+');
    replace( s.begin(), s.end(), '/', '\\');
    replace( s.begin(), s.end(), '^', '');
    */
    replace_all(s,"*","+");
    replace_all(s,"/","╱");
    replace_all(s,"`","£");
    lowcase(s);
}

void command::os_to_tos(string& s) {
    /*
    replace( s.begin(), s.end(), '*', '+');
    replace( s.begin(), s.end(), '/', '\\');
    replace( s.begin(), s.end(), '^', '');
    */
    replace_all(s,"+","*");
    replace_all(s,"╱","/");
    replace_all(s,"£","`");
    replace_all(s,"ł","`");
    replace_all(s,"Ł","`");
    replace_all(s,":","=");
    upcase(s);
    if(s.length() > 8) { s=s.substr(0,8); }
}

void command::os_to_internal(string os, string &dir) {
    dir = os_to_internal(os);
}

string command::os_to_internal(string name) {
    string ext;
    int dir;
    size_t pos;

    pos=name.find(':');
    if(pos != string::npos) {
        dir=stoi(name.substr(0,pos));
        name=name.substr(pos+1);
    } else dir = 0;
    os_to_tos(name);
    ext = "";
    pos=name.find('.');
    if(pos != string::npos) {
        ext=name.substr(pos+1,3);
        name=name.substr(0,pos);
    }
    ext=rtrim(ext);
    name=rtrim(name);
    replace_all(ext," ","_");
    replace_all(name," ","_");
    for(auto&c: ext) {
        if (c<' ') { c='#';}
        if (c>'~') { c='$';}
    }
    for(auto&c: name) {
        if (c<' ') { c='#';}
        if (c>'~') { c='$';}
    }

    name+= "          ";
    ext+="   ";
    name=name.substr(0,8)+ext.substr(0,3);
    if (ext.substr(0,3)=="DIR") {
        name=char(dir|0x80)+name;
    } else {
        name=char(dir)+name;
    }
    return name;
}


const list<img_format> command::format_list = {
    {"tos140"  , 40*4096, 40,1,4, 4,0},
    {"tos140ds", 80*4096, 40,1,4,44,1},
    {"tos620"  ,160*4096,160,1,4, 4,0},
    {"tos4"    , 42*4096, 42,0,0, 0,0},
    {"tos4ds"  , 84*4096, 42,0,0,46,1}
};

const string command::initial_name = "ANEWDISK";
img_format command::current_format = img_format();
char command::tos2ext[] = {0};
int command::current_side = 0;
tos_image command::image = tos_image();
string command::last_name = "";
