/*
 *  To do:
 *  -view (basic, code/bin, arrays)
 *  -read/write txt basic files (arrays?)
 *  -save scr as bmp/tga/pcx/ppm/
 *  -attrib
 *  -cd
 *  -free
 *  MDR images ?
 */


#include <string>
#include <list>
#include <map>
#include <iostream>

#include "command.hpp"

#include "command_delete.hpp"
#include "command_dskscan.hpp"
#include "command_export.hpp"
#include "command_import.hpp"
#include "command_info.hpp"
#include "command_list.hpp"
#include "command_mkdir.hpp"
#include "command_move.hpp"
#include "command_new.hpp"
#include "command_open.hpp"
#include "command_read.hpp"
#include "command_rename.hpp"
#include "command_save.hpp"
#include "command_saveas.hpp"
#include "command_sexport.hpp"
#include "command_side.hpp"
#include "command_sread.hpp"
#include "command_write.hpp"

using namespace std;

static map<string,command*> cmd_map = {
    {"-delete",	new command_delete()},
    {"-dskscan",new command_dskscan()},
    {"-export",	new command_export()},
    {"-import",	new command_import()},
    {"-info",	new command_info()},
    {"-list",	new command_list()},
    {"-mkdir",	new command_mkdir()},
    {"-move",	new command_move()},
    {"-new",	new command_new()},
    {"-open",	new command_open()},
    {"-read",	new command_read()},
    {"-rename",	new command_rename()},
    {"-save",	new command_save()},
    {"-saveas",	new command_saveas()},
    {"-sexport",new command_sexport()},
    {"-side",	new command_side()},
    {"-sread",	new command_sread()},
    {"-write",	new command_write()}
};

void command_help() {
    map<string,command*>::iterator cmd_it;
    command *cmd;
    cout << "help : " << endl << endl;
    cmd_it = cmd_map.begin();
    while(cmd_it != cmd_map.end()) {
        cmd=cmd_it->second;
        cmd->help();
        cout << endl;
        cmd_it++;
    }
}

int main (int argc, char *argv[]) {
    int help = 0;
    int bye = 0;

    string par;
    string cmd;
    list <string> args;
    list <string> pars;
    map<string,command*>::iterator cmd_it;

    cerr << endl << "TOs MAnipulation TOol. v0.1.2" << endl << endl;
    if (argc<2) { help = 1; bye = 1; }
    else {
        for (int i = 1; i<argc; i++) { 
	    par=string(argv[i]);
	    if  (par == "-help") { help = 1; continue;}
	    if ((par[0]=='-')&&(cmd_map.count(par)==0)) { 
		cerr << "Unknown command : " << par << endl;
		bye = 2; 
	    }
	    args.push_back(par);
	}
    }
    if (help) { command_help(); }
    if (bye)  { return bye; }
    
    par="";
    pars.clear();
    while(args.size()) {
    	par=args.front();
    	args.pop_front(); 
    	if (par[0]=='-') { 
    	    if (cmd.length()) { 
    		cmd_it = cmd_map.find(cmd);
        	cmd = cmd.substr(1);
		cout << "C : " << cmd << endl;
    		cmd_it->second->process(pars);
    	    }
    	    cmd=par;
    	    pars.clear();
    	} else { pars.push_back(par); }
    }
    if (cmd.length()) { 
    	cmd_it = cmd_map.find(cmd);
    	cmd = cmd.substr(1);
	cout << "C : " << cmd << endl;
    	cmd_it->second->process(pars);
    }
    cout << endl;
    for (auto& x: cmd_map) { delete x.second; }
    return 0;
}
