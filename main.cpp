#include <cstdio>
#include <string>
#include <cstdlib>
#include "bbmain.hpp"
using namespace std;

void help() {
	printf("Usage: blue (-h | -v | file)\n\n-h\n--help      Show this help message and quit.\n-v\n--version   Show version message and quit.\n\nRun blue with filename will run bluebetter program.\n");
	exit(0);
}

void vers() {
	printf("BlueBetter interpreter v3.0\nBlueBetter %s\n",BLUEBETTER_VER);
	exit(0);
}

int main(int argc, char* argv[]) {
	if (argc == 1) help();
	string s = argv[1];
	if (s == "-h" || s == "--help") help();
	if (s == "-v" || s == "--version") vers();
	FILE *f;
	f = fopen(argv[1],"r");
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
	return runCode(buf);
}
