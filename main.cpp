#include <cstdio>
#include <string>
#include <cstdlib>
#include "bbmain.hpp"
using namespace std;

/*
Run to mailslot:

-D		Debugging in mailslot.
		A mailslot named dbginslot receive command (requires open first).
		A mailslot named dbgoutslot output.
-R		Run in mailslot.
		A mailslot named dbgoutslot output.
*/ 

void help() {
	printf("Usage: blue (-h | -v | file)\n\n-h\n--help      Show this help message and quit.\n-v\n--version   Show version message and quit.\n-d\n--debug     Run program with debugger.\n\nRun blue with filename will run bluebetter program.\n");
	exit(0);
}

void vers() {
	printf("BlueBetter interpreter v3.0\nBlueBetter %s\n",BLUEBETTER_VER);
	exit(0);
}

int main(int argc, char* argv[]) {
	bool debugs = false, mails = false;
	string s;
	if (argc == 1) help();
	s = argv[1];
	if (s == "-h" || s == "--help") help();
	if (s == "-v" || s == "--version") vers();
	if (s == "-d" || s == "--debug") {
		if (argc < 3) {
			printf("Error: cannot read file\n");
			return 1;
		}
		debugs = true;
		s = argv[2];
	}
	FILE *f;
	f = fopen(s.c_str(),"r");
	if (f == NULL) {
		printf("Error: cannot read file\n");
		return 1;
	}
	string buf = "";
	while (!feof(f)) {
		buf += fgetc(f);
	}
	buf=buf.substr(0,buf.length()-1); // removing something not good
	fclose(f);
	return __runCode(buf,debugs,mails); //runCode(buf);
}
