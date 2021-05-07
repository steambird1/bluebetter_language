#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <iostream>
#include <conio.h>
using namespace std;

#ifndef _SHELLEXEC_
#define _SHELLEXEC_

// From seabird_shell.

string getl(void) {
	string cmd = "";
	char c;
		while ((c=getchar())!='\n') {
			if (c=='\n') break;
			cmd = cmd + c;
		}
	return cmd;
}

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

vector<string> split_arg(string cmd,bool allow_quotes,char splitor) {
	vector<string> argv;
	// argc = argv.size()
	bool inquote = false;
	string buf = "";
	for (int i = 0; i < cmd.length(); i++) {
		if (cmd[i]==splitor) {
			if (inquote) {
					buf = buf + splitor;
					break;
				}
				argv.push_back(buf);
				buf = "";
		} else if (cmd[i]=='"') {
			if (allow_quotes) inquote = !inquote;
				else buf = buf + '"';
		} else {
			buf = buf + cmd[i];
		}
	}
	if (buf!="") argv.push_back(buf);
	return argv;
}
string pwd_input(void) {
	int k;
		string s = "";
		do {
			k=getch();
			if (k==13) break;
			if (k==8&&s.length()>0) {
				cout<<"\b \b";
				s=s.substr(0,s.length()-1);
				continue;
			}
			cout<<"*";
			s+=char(k);
		} while (1);
	return s;
}

#endif
