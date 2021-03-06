#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <conio.h>
#include <windows.h>
using namespace std;

#ifndef _SHELLEXEC_
#define _SHELLEXEC_

//#define DEBUGS 1 

// From seabird_shell.

bool isFileExist(string fn) {
	const char *file_name = fn.c_str();
	bool flag = false;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(file_name, &wfd);
	if ( INVALID_HANDLE_VALUE != hFind )   flag = true;
	else                                   flag = false;
	CloseHandle(hFind);
	return flag;
	/*
Link??https://blog.csdn.net/guowenyan001/article/details/17259173
Modified by steambird1 2021-4-30
*/
}

string waitForFile(string filename) {
	string buf;
	while (!isFileExist(filename));
	FILE *f;
	f = fopen(filename.c_str(),"r");
	while (!feof(f)) {
		char c = fgetc(f);
		buf += c;
	}
	buf=buf.substr(0,buf.length()-1); // removing something not good
	fclose(f);
	remove(filename.c_str()); // otherwise looping
	return buf;
}

void writeLine(string filename,string data) {
	FILE *f;
	f = fopen(filename.c_str(),"w");
	fprintf(f,"%s\n",data.c_str());
	fclose(f);
}

int to_int(const char *s) {
	string w = s;
	if (s[0]=='+' || s[0] == '-') w = w.substr(1);
	int t = atoi(w.c_str());
	if (s[0]=='-') return 0-t;
	return t;
} 

double to_double(const char *s) {
	string w = s;
	if (s[0]=='+' || s[0] == '-') w = w.substr(1);
	double d = atof(w.c_str());
	if (s[0]=='-') return 0.000000-d;
	return d;
}

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
				} else {
					argv.push_back(buf);
					buf = "";
				}
		} else if (cmd[i]=='"') {
			if (allow_quotes) {
				inquote = !inquote;
			}
			buf = buf + '"';
		} else {
			buf = buf + cmd[i];
		}
	}
	if (buf!="") argv.push_back(buf);
	return argv;
}

vector<string> split_argw(string cmd,bool allow_quotes,char splitor) {
	vector<string> argv;
	// argc = argv.size()
	bool inquote = false;
	string buf = "";
	for (int i = 0; i < cmd.length(); i++) {
		if (cmd[i]==splitor) {
			if (inquote) {
					buf = buf + splitor;
				} else {
					argv.push_back(buf);
					buf = "";
				}
		} else if (cmd[i]=='"') {
			if (allow_quotes) {
				inquote = !inquote;
			}
			else buf = buf + '"';
//			here is the difference
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

bool isContain(string a,string b) {
	return a.find(b) != string::npos;
}

pair<string,string> getArrayz(string exp) {
	string p[2]={"",""};
	bool mode = false;
	for (int i = 0; i < exp.length(); i++) {
		if (exp[i]=='(') {
			mode = true;
		} else if (exp[i]==')') {
			break;
		} else {
			p[int(mode)]+=exp[i];
		}
	} 
	return make_pair(p[0],p[1]);
}

#endif
