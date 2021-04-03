#include "bbmain.hpp"
#include <string>
#include <iostream>
using namespace std;

void intp(void) {
	string s;
	splits asplit;
	bmemap mem;
	jmpmap jmps;
	int i; // jump is not works. 
	while (true) {
		cout << endl << ">>> ";
		getline(cin,s);
		asplit = split_arg(s,false);
		if (asplit.size() < 1) continue; // no command already
		if (asplit[0][0]==';') {
			continue;
		} else if (asplit[0]=="bjmp") {
			check_parameter(3);
			int ptrz = getRealVal(&mem,asplit[1]);
			if (!jmps.count(asplit[2])) {
				throws(bad_jump);
			}
			if (ptrz!=0) {
				i = jmps[asplit[2]];
			}
		} else if (asplit[0]=="bset") {
			check_parameter(3);
			int val = getRealVal(&mem,asplit[2]), ptr = atoi(asplit[1].c_str());
			mem[ptr]=val;
		} else if (asplit[0]=="tand") {
			_op(); mem[ptr]=mem[ptr]&val;
		} else if (asplit[0]=="band") {
			_op(); mem[ptr]=mem[ptr]&&val;
		} else if (asplit[0]=="tor") {
			_op(); mem[ptr]=mem[ptr]|val; 
		} else if (asplit[0]=="bor") {
			_op(); mem[ptr]=mem[ptr]||val;
		} else if (asplit[0]=="txor") {
			_op(); mem[ptr]=mem[ptr]^val;
		} else if (asplit[0]=="bnot") {
			check_parameter(2);
			int ptr = atoi(asplit[1].c_str());
			if (!mem.count(ptr)) mem[ptr]=0;
			mem[ptr]=!mem[ptr];
		} else if (asplit[0]=="tlm") {
			_op(); mem[ptr]=mem[ptr]<<val;
		} else if (asplit[0]=="trm") {
			_op(); mem[ptr]=mem[ptr]>>val;
		} else if (asplit[0]=="badd") {
			_op(); mem[ptr]+=val;
		} else if (asplit[0]=="bded") {
			_op(); mem[ptr]-=val;
		} else if (asplit[0]=="btim") {
			_op(); mem[ptr]*=val;
		} else if (asplit[0]=="bdiv") {
			_op(); if (val==0) {
				throws(math_error);
			}
			mem[ptr]/=val;
		} else if (asplit[0]=="bmod") {
			_op(); if (val==0) {
				throws(math_error);
			}
			mem[ptr]%=val;
		} else if (asplit[0]=="bequ") {
			_cop();
			if (ptr1==ptr2) mem[dst]=1;
			else mem[dst]=0;
		} else if (asplit[0]=="bcmp") {
			_cop();
			if (ptr1>ptr2) mem[dst]=1;
			else mem[dst]=0;
		} else if (asplit[0]=="btel") {
			check_parameter(3);
			int ptr = getRealVal(&mem,asplit[2]);
			if (asplit[1]=="get") {
				mem[ptr]=int(getchar());
			} else if (asplit[1]=="put") {
				putchar(char(ptr));
			} else if (asplit[1]=="read") {
				int s;
				scanf("%d",&s);
				mem[ptr]=s;
			} else if (asplit[1]=="write") {
				printf("%d",ptr);
			} else {
				throws(bad_tell);
			}
		}
	} 
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		intp();
	} else {
		string c = argv[1];
		if (argv[1]=="shell") intp();
	}
	return 0;
} 
