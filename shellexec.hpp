#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <iostream>
#include <conio.h>
using namespace std;

#ifndef _SHELLEXEC_
#define _SHELLEXEC_


vector<string> spiltLines(string s) {
	vector<string> v;
	string buf = "";
	for (int i = 0; i < s.length(); i++) {
		if (s[i]=='\n') {
			v.push_back(buf);
			buf = "";
		} else {
			buf = buf + s[i];
		}
	}
	if (buf.length()>0) v.push_back(buf);
	return v;
}

vector<string> split_arg(string cmd,bool allow_quotes) {
	vector<string> argv;
	// argc = argv.size()
	bool inquote = false;
	string buf = "";
	for (int i = 0; i < cmd.length(); i++) {
		switch (cmd[i]) {
			case ' ': 
				if (inquote) {
					buf = buf + ' ';
					break;
				}
				argv.push_back(buf);
				buf = "";
				break;
			case '"':
				if (allow_quotes) inquote = !inquote;
				else buf = buf + '"';
				break;
			default:
				buf = buf + cmd[i];
				break;
		} 
	}
	if (buf!="") argv.push_back(buf);
	return argv;
}

#endif
