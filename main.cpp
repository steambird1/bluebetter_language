#include <cstdio>
#include <string>
#include <cstdlib>
#include "bbmain.hpp"
using namespace std;

/*
Pipeline options:

-D	Run debugger as pipe
-R	Run program as pipe
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
	bool debugs = false, pipes = false;
	if (argc == 1) help();
	string s = argv[1];
	if (s == "-h" || s == "--help") help();
	if (s == "-v" || s == "--version") vers();
	if (s == "-d" || s == "--debug" || s == "-D") {
		if (argc < 3) {
			printf("Error: cannot read file\n");
			return 1;
		}
		debugs = true;
		if (s == "-D") pipes = true;
		s = argv[2];
	}
	if (s == "-R") {
		if (argc < 3) {
			printf("Error: cannot read file\n");
			return 1;
		}
		pipes = true;
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
	return __runCode(buf,debugs,pipes); //runCode(buf);
}
