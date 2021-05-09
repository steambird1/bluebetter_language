#include "shellexec.hpp"
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <cstdio>
#include <conio.h>
using namespace std;

#ifndef _BLUEBETTER_
#define _BLUEBETTER_ 1

#define BLUEBETTER_VER "v202105"

#define DEBUG_MODE false
#define PRINT_ERROR_INFO true
#define EXIT_IN_ERROR true

#define ifdebug if(DEBUG_MODE)

// It'll be error if you don't do this!!!
#define __throw(errid) do { int_var["error"]=errid; if (PRINT_ERROR_INFO) printf("\nAn error was occured.\n\nError id: %d\n",errid); if (DEBUG_MODE) printf("Error line in interpreter: %d\n\n",__LINE__); if (EXIT_IN_ERROR) return 0-errid; goto cont ;} while (false)

bool isContain(string a,string b) {
	return a.find(b) != string::npos;
}

struct strProcs {
	string bsname;
	int bsbegin;
	int bsend;
};

inline strProcs __makeStrprocs(string bsname,int bsbegin,int bsend) {
	strProcs _t; _t.bsbegin=bsbegin; _t.bsend=bsend; _t.bsname=bsname; return _t;
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

pair<string,string> getDotz(string exp) {
	string p[2]={"",""};
	bool mode = false;
	for (int i = 0; i < exp.length(); i++) {
		if (exp[i]=='.') {
			mode = true;
		}else {
			p[int(mode)]+=exp[i];
		}
	} 
	return make_pair(p[0],p[1]);
}

 
int __getIntval(string exp,map<string,int> int_varlist,map<string,pair<int*,int> > int_arrlist) {
	if (isdigit(exp[0])) {
		return atoi(exp.c_str());
	} else if (exp[0]=='"') {
		return 0;
	} else if (isContain(exp,"(")) {// or ")"
		pair<string,string> a = getArrayz(exp);
		string buf1 = a.first,buf2 = a.second;
		int z = __getIntval(string(buf2),int_varlist,int_arrlist);
		if (!int_arrlist.count(buf1)) return 0;
		if (z < 0 || z >= int_arrlist[buf1].second) return 0;
		return int_arrlist[buf1].first[z];
	} else {
		if (!int_varlist.count(exp)) return 0;
		return int_varlist[exp];
	}
} 

#define getIntval(exp) __getIntval(exp,int_var,int_arr)

strProcs __getSproc(string exp,map<string,int> int_varlist,map<string,pair<int*,int> > int_arrlist) {
	pair<string,string> s = getArrayz(exp);
	string p[2]={"",""};
	if (isContain(s.second,":")) {
		bool mode = false;
		for (int i = 0; i < s.second.length(); i++) {
			if (s.second[i]==':') {
				mode = true;
			} else {
				p[int(mode)]+=s.second[i];
			}
		}
		return __makeStrprocs(s.first,__getIntval(p[0],int_varlist,int_arrlist),__getIntval(p[1],int_varlist,int_arrlist));
	} else {
		int at = __getIntval(s.second,int_varlist,int_arrlist);
		return __makeStrprocs(s.first,at,at);
	}
}

#define getSproc(exp) __getSproc(exp,int_var,int_arr)

string __getStrval(string exp,map<string,int> int_varlist,map<string,pair<int*,int> > int_arrlist,map<string,string> str_varlist,map<string,pair<string*,int> > str_arrlist) {
	if (isdigit(exp[0])) {
		return "";
	} else if (exp[0]=='"') {
		return exp.substr(1,exp.length()-2);
	} else if (isContain(exp,"(")) {// or ")"
		pair<string,string> a = getArrayz(exp);
		string buf1 = a.first,buf2 = a.second;
		int z = __getIntval(string(buf2),int_varlist,int_arrlist);
		if (!str_arrlist.count(buf1)) return "";
		if (z < 0 || z >= str_arrlist[buf1].second) return "";
		return str_arrlist[buf1].first[z];
	} else {
		if (!str_varlist.count(exp)) return "";
		return str_varlist[exp];
	}
}
#define getStrval(exp) __getStrval(exp,int_var,int_arr,str_var,str_arr)

char __getVartype(string exp,map<string,int> int_varlist,map<string,pair<int*,int> > int_arrlist,map<string,string> str_varlist,map<string,pair<string*,int> > str_arrlist) {
	if (isContain(exp,"(")) {
		pair<string,string> a = getArrayz(exp);
		if (int_arrlist.count(a.first)) return 'I';
		if (str_arrlist.count(a.first)) return 'S';
		return '?';
	} else {
		if (int_varlist.count(exp)) return 'i';
		if (str_varlist.count(exp)) return 's';
		return '?';
	}
}
#define getVartype(exp) __getVartype(exp,int_var,int_arr,str_var,str_arr)

#define __op_comp_int(op) curr = (__getIntval(args[i],int_varlist,int_arrlist) op __getIntval(args[i+2],int_varlist,int_arrlist))
#define __op_comp_str(op) curr = (__getStrval(args[i],int_varlist,int_arrlist,str_varlist,str_arrlist) op __getStrval(args[i+2],int_varlist,int_arrlist,str_varlist,str_arrlist)) 

#define __op_proceed_int(op) int_var[iprocd[0]] = getIntval(iprocd[2]) op getIntval(iprocd[4])
#define __op_proceeds_int(op) int_var[args[0]] = getIntval(args[2]) op getIntval(args[4])
#define __op_proceeds_str(op) str_var[args[0]] = getStrval(args[2]) op getStrval(args[4])
#define __op_proceeda_int(op) int_arr[ps.first].first[dnid] = getIntval(args[2]) op getIntval(args[4])
#define __op_proceeda_str(op) str_arr[ps.first].first[dnid] = getStrval(args[2]) op getStrval(args[4])
#define __op_singoc_int(op) int_var[args[0]] op getIntval(args[2])
#define __op_singoc_str(op) str_var[args[0]] op getStrval(args[2])
#define __op_singrc_int(op) int_var[args[0]] = (op getIntval(args[2]))
#define __op_singoa_int(op) int_arr[ps.first].first[dnid] op getIntval(args[2])
#define __op_singoa_str(op) str_arr[ps.first].first[dnid] op getStrval(args[2])
#define __op_singra_int(op) int_arr[ps.first].first[dnid] = (op getIntval(args[2]))

#define skipLines(opname) int j = i, stack = 0; \
						while (true) { \
							vector<string> argt = split_arg(lines[j],true,' '); \
							if (argt[0]=="while" || argt[0]=="for" || argt[0]=="do" || argt[0]=="if") stack++; \
							if (argt[0]=="end") stack--; \
							if (lines[j] == (string("end ") + opname) && stack <= 0) break; \
							j++; \
						} \
						i = j+1; goto fcont

bool __getCond(string exp,map<string,int> int_varlist,map<string,pair<int*,int> > int_arrlist,map<string,string> str_varlist,map<string,pair<string*,int> > str_arrlist) {
	vector<string> args = split_arg(exp,true,' ');
	bool stat; 
	for (int i = 0; i < args.size(); i+=4) { // a op b op
		bool curr;
		if (args[i+1]=="==") {
			__op_comp_int(==);
		} else if (args[i+1]=="!=") {
			__op_comp_int(!=);
		} else if (args[i+1]==">") {
			__op_comp_int(>);
		} else if (args[i+1]==">=") {
			__op_comp_int(>=);
		} else if (args[i+1]=="<") {
			__op_comp_int(<);
		} else if (args[i+1]=="<=") {
			__op_comp_int(<=);
		} /*here are strings: I CAN'T GET TYPES*/ else if (args[i+1]=="===") {
			__op_comp_str(==);
		} else if (args[i+1]=="!==") {
			__op_comp_str(!=);
		} /*nothing yet*/ else {
			return false;
		}
		if (i!=0) {
			if (args[i-1]=="||") {
				stat = stat || curr;
			} else if (args[i-1]=="&&") {
				stat = stat && curr;
			} else {
				return false;
			}
		} else {
			stat = curr;
		}
	}
	return stat;
}

#define getCond(exp) __getCond(exp,int_var,int_arr,str_var,str_arr)

int runCode(string code) {
	map<string,int> int_var, callist;
	map<string,string> str_var;
	map<string,pair<int*,int> > int_arr; // an memory space and length
	map<string,pair<string*,int> > str_arr;
	vector<string> lines = spiltLines(code);
	stack<int> calltrace;
	int i = 0,rets = 0; // executor pointing
	str_var["RESV_EMPTY"]="";// Reserved
	while (i < lines.size()) {
		ifdebug printf("Processing command: %s\n",lines[i].c_str());
		vector<string> args = split_arg(lines[i],true,' ');
		if (args.size() != 0 && args[0][0]!='#') {
			string attl;
			if (args.size() > 1) attl = lines[i].substr(args[0].length()+1); // getting other things EXCEPT command
			else attl = "";
			if (args[0]=="int" || args[0]=="char") {
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					vector<string> iprocd = split_arg(*it,true,' ');
					if (int_var.count(iprocd[0])) __throw(1); 
					if (iprocd.size() == 3) {
						// a = (can only be) b
						if (iprocd[1] != "=") __throw(1);
						int_var[iprocd[0]]=getIntval(iprocd[2]);
					} else if (iprocd.size() == 5) {
						// a = (can only be) b op c
						if (iprocd[1] != "=") __throw(1);
						// what about "op"?
						if (iprocd[3] == "+") {
							__op_proceed_int(+);
						} else if (iprocd[3] == "-") {
							__op_proceed_int(-);
						} else if (iprocd[3] == "*") {
							__op_proceed_int(*);
						} else if (iprocd[3] == "/") {
							__op_proceed_int(/);
						} else if (iprocd[3] == "%") {
							__op_proceed_int(%);
						} else if (iprocd[3] == "|") {
							__op_proceed_int(|);
						} else if (iprocd[3] == "&") {
							__op_proceed_int(&);
						} else if (iprocd[3] == "^") {
							__op_proceed_int(^);
						} else if (iprocd[3] == "||") {
							__op_proceed_int(||);
						} else if (iprocd[3] == "&&") {
							__op_proceed_int(&&);
						} else {
							__throw(3);
						}
					} else {
						__throw(2);
					}
				}
			} else if (args[0]=="str") {
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					vector<string> iprocd = split_arg(*it,true,' ');
					if (iprocd.size() < 3) __throw(1);
					if (str_var.count(iprocd[0])) __throw(1);
					if (iprocd.size() == 3) {
						// a = (can only be) b
						if (iprocd[1] != "=") {
							__throw(1);
						}
						string strv = getStrval(iprocd[2]);
						str_var[iprocd[0]] = strv;
					} else if (iprocd.size() == 5) {
						// a = (can only be) b op c
						if (iprocd[1] != "=") __throw(1);
						// what about "op"?
						if (iprocd[3] == "+") {
							str_var[iprocd[0]] = getStrval(iprocd[2]) + getStrval(iprocd[4]);
						} else {
							__throw(3);
						}
					} else {
						__throw(2);
					}
				}
			} else if (args[0]=="array") {
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					vector<string> iprocd = split_arg(*it,true,' ');
					if (iprocd.size() == 2) {
						pair<string,string> a = getArrayz(iprocd[1]);
						string buf1 = a.first, buf2 = a.second;
						int len = getIntval(buf2);
						ifdebug printf("Setting length: %d\n",len);
						if (iprocd[0]=="int" || iprocd[0]=="char") {
							int_arr[string(buf1)] = make_pair(new int[len],len);
						} else if (iprocd[0]=="str") {
							str_arr[string(buf1)] = make_pair(new string[len],len);
						} else {
							__throw(5);
						}
					} else {
						__throw(4); 
					}
				}
			} else if (args[0]=="sub") {
				if (args.size() != 2) {
					__throw(5);
				} else {
					callist[args[1]] = i;
					skipLines("sub");
				}
			} else if (args[0]=="for") {
				// for i = a..b (2)
				// collecting informations first
				int begin,end,step=1;
				if (args.size()!=4 && args.size()!=6) {
					__throw(6);
				}
				//sscanf(args[3].c_str(),"%s..%s",buf1,buf2);
				pair<string,string> a = getDotz(args[3]);
				string buf1 = a.first, buf2 = a.second;
				begin = getIntval(string(buf1)); end = getIntval(string(buf2));
				if (args.size()==6) {
					step = getIntval(args[5]);
				}
				// first time running this?
				if (!int_var.count(args[1])) {
					// yes
					int_var[args[1]] = begin;
				} else {
					if (int_var[args[1]] == end) { // REMEMBER THIS OPERATOR!
						int_var.erase(args[1]);
						skipLines("for");
						 // MUST JUMP TO NEXT!!!!!
					} else {
						int_var[args[1]] += step;
					}
				}
			} else if (args[0]=="call") {
				if (args.size() != 2) __throw(7);
				if (!callist.count(args[1])) __throw(7);
				calltrace.push(i); // don't run call again.
				i = callist[args[1]];
			} else if (args[0]=="if" || args[0]=="elseif") {
				if (!getCond(attl)) {
					//skipLines(if);
					int j = i+1, stack = 0;
					while (true) {
						j++;
						vector<string> argt = split_arg(lines[j],true,' ');
						if (argt[0] == "if") stack++;
						if (argt[0] == "end" && argt.size() >= 2 && argt[1] == "if") stack--;
						if (stack < 0) break;
						if (argt[0] == "elseif" && stack == 0) break;
						if (argt[0] == "else" && stack == 0) break;
					}
					i = j;
				}
			} else if (args[0]=="elseif" || args[0]=="else") {
				//;
			} else if (args[0]=="end") { // remember to skip to the next line of "end"!
				if (args.size() == 1 || (args.size() == 2 && args[1] == "sub")) {
					if (calltrace.empty()) {
						rets = 0;
						goto ret;
					}
					i = calltrace.top();
					calltrace.pop();
				} else {
					if (args[1] == "if") goto cont;
					int j = i, stack = 0;
						while (true) { 
							vector<string> argt = split_arg(lines[j],true,' '); 
							if (argt[0]==args[1] && stack <= 0) break;
							if (argt[0]=="while" || argt[0]=="for" || argt[0]=="do" || argt[0]=="if") stack++; 
							if (argt[0]=="end") stack--; 
							j--; 
						} 
						i = j;
						goto fcont;
				}
			} else if (args[0]=="read") {
				// read int a, char b, str c
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					vector<string> iprocd = split_arg(*it,true,' ');
					ifdebug printf(" Inputting %s: ",it->c_str());
					if (iprocd.size() == 2) {
						int readi;
						if (iprocd[0]=="int") {
							scanf("%d",&readi);
							int_var[iprocd[1]]=readi;
						} else if (iprocd[0]=="char") {
							scanf("%c",&readi);
							int_var[iprocd[1]]=readi;
						} else if (iprocd[0]=="str") {
							char buf[1024];
							scanf("%s",buf);
							str_var[iprocd[1]]=string(buf);
						} else __throw(10);
					} else __throw(10);
				}
			} else if (args[0]=="readline") {
				// readline a (str)
				if (args.size() != 2) __throw(10);
				char buf[1024];
				gets(buf);
				str_var[args[1]]=string(buf);
			} else if (args[0]=="readkey") {
				// readkey a (int)
				if (args.size() != 2) __throw(10);
				int_var[args[1]]=getch();
			} else if (args[0]=="write" || args[0]=="writeln") {
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					ifdebug printf(" Outputting %s: ",it->c_str());
					vector<string> iprocd = split_arg(*it,true,' ');
					if (iprocd.size() == 2) {
						if (iprocd[0]=="int") {
							printf("%d",getIntval(iprocd[1]));
						} else if (iprocd[0]=="char") {
							printf("%c",getIntval(iprocd[1]));
						} else if (iprocd[0]=="str") {
							printf("%s",(getStrval(iprocd[1])).c_str());
						} else {
							__throw(10);
						}
					} else {
						__throw(10);
					}
				}
				if (args[0]=="writeln") printf("\n");
			} else if (args[0]=="while") {
				if (!getCond(attl)) {
					skipLines("while");
				} // else go down
			} else if (args[0]=="next") {
				int j = i, stack = 0; 
						while (true) { 
							vector<string> argt = split_arg(lines[j],true,' '); 
							if (argt[0]=="end") stack++;
							if (argt[0]=="while" || argt[0]=="for" || argt[0]=="do") stack--;
							if (stack < 0) break; 
							j--;
						} 
						i = j;
						goto fcont;
			} else if (args[0]=="do") {
				// nothing; keep going down
			} else if (args[0]=="break") {
				int j = i; 
						while (true) { 
							vector<string> argt = split_arg(lines[j],true,' '); 
							if (argt[0]=="end") break; 
							j++;
						} 
						i = j+1;
						goto fcont;
			} else if (args[0]=="ret") {
				if (args.size() != 2) {
					__throw(11);
				}
				rets = getIntval(args[1]); 
				goto ret;
			} else if (args[0]=="get") { // Supports variable only
				if (args.size() != 4 || args[2] != "=") __throw(12);
				strProcs spc = getSproc(args[3]);
				string strv = getStrval(spc.bsname);
				if (spc.bsbegin==spc.bsend) {
					if (spc.bsbegin >= strv.length()) __throw(13);
					int_var[args[1]] = strv[spc.bsbegin];
				} else {
					if (spc.bsbegin > spc.bsend || spc.bsend >= strv.length()) __throw(13);
					str_var[args[1]] = strv.substr(spc.bsbegin,spc.bsend-spc.bsbegin);
				}
			} else if (args[0]=="set") {
				if (args.size() != 4 || args[2] != "=") __throw(12);
				strProcs spc = getSproc(args[1]);
				if (!str_var.count(spc.bsname)) __throw(12);
				string strv = getStrval(spc.bsname);
				if (spc.bsbegin==spc.bsend) {
					if (spc.bsbegin > strv.length()) __throw(13);
					str_var[spc.bsname][spc.bsbegin] = char(getIntval(args[3]));
				} else {
					if (spc.bsbegin > spc.bsend || spc.bsend >= strv.length()) __throw(13);
					string s = getStrval(args[3]);
					for (int i = 0; i < (spc.bsend-spc.bsbegin); i++) {
						str_var[spc.bsname][i+spc.bsbegin] = s[i];
					}
				}
			} else {
				// setting variable value.
				int dnid;
				pair<string,string> ps;
				if (args.size() < 3) __throw(1);
				switch (getVartype(args[0])) {
					case 'i':
						if (args.size() == 3) {
					// a op b (including =! =~)
					if (args[1] == "=") {
						__op_singoc_int(=);
					} else if (args[1] == "+=") {
						__op_singoc_int(+=);
					} else if (args[1] == "-=") {
						__op_singoc_int(-=);
					} else if (args[1] == "*=") {
						__op_singoc_int(*=);
					} else if (args[1] == "/=") {
						__op_singoc_int(/=);
					} else if (args[1] == "%=") {
						__op_singoc_int(%=);
					} else if (args[1] == "&=") {
						__op_singoc_int(&=);
					} else if (args[1] == "|=") {
						__op_singoc_int(|=);
					} else if (args[1] == "^=") {
						__op_singoc_int(^=);
					} else if (args[1] == "=!") {
						__op_singrc_int(!);
					} else if (args[1] == "=~") {
						__op_singrc_int(~);
					} else __throw(3);
				} else if (args.size() == 5) {
					// a = b op c
					if (args[1] != "=") __throw(1);
					if (args[3] == "+") {
							__op_proceeds_int(+);
						} else if (args[3] == "-") {
							__op_proceeds_int(-);
						} else if (args[3] == "*") {
							__op_proceeds_int(*);
						} else if (args[3] == "/") {
							__op_proceeds_int(/);
						} else if (args[3] == "%") {
							__op_proceeds_int(%);
						} else if (args[3] == "|") {
							__op_proceeds_int(|);
						} else if (args[3] == "&") {
							__op_proceeds_int(&);
						} else if (args[3] == "^") {
							__op_proceeds_int(^);
						} else if (args[3] == "||") {
							__op_proceeds_int(||);
						} else if (args[3] == "&&") {
							__op_proceeds_int(&&);
						} else {
							__throw(3);
						}
				} else __throw(3);
						break;
					case 'I':
						ps = getArrayz(args[0]);
						dnid = getIntval(ps.second);
						if (args.size() == 3) {
					// a op b (including =! =~)
					if (args[1] == "=") {
						__op_singoa_int(=);
					//	int_arr = getIntval(args[2]); // debug replacement
					} else if (args[1] == "+=") {
						__op_singoa_int(+=);
					} else if (args[1] == "-=") {
						__op_singoa_int(-=);
					} else if (args[1] == "*=") {
						__op_singoa_int(*=);
					} else if (args[1] == "/=") {
						__op_singoa_int(/=);
					} else if (args[1] == "%=") {
						__op_singoa_int(%=);
					} else if (args[1] == "&=") {
						__op_singoa_int(&=);
					} else if (args[1] == "|=") {
						__op_singoa_int(|=);
					} else if (args[1] == "^=") {
						__op_singoa_int(^=);
					} else if (args[1] == "=!") {
						__op_singra_int(!);
					} else if (args[1] == "=~") {
						__op_singra_int(~);
					} else __throw(3);
				} else if (args.size() == 5) {
					// a = b op c
					if (args[1] != "=") __throw(1);
					if (args[3] == "+") {
							__op_proceeda_int(+);
						} else if (args[3] == "-") {
							__op_proceeda_int(-);
						} else if (args[3] == "*") {
							__op_proceeda_int(*);
						} else if (args[3] == "/") {
							__op_proceeda_int(/);
						} else if (args[3] == "%") {
							__op_proceeda_int(%);
						} else if (args[3] == "|") {
							__op_proceeda_int(|);
						} else if (args[3] == "&") {
							__op_proceeda_int(&);
						} else if (args[3] == "^") {
							__op_proceeda_int(^);
						} else if (args[3] == "||") {
							__op_proceeda_int(||);
						} else if (args[3] == "&&") {
							__op_proceeda_int(&&);
						} else {
							__throw(3);
						}
				} else __throw(3);
						break;
					case 's':
						if (args.size() == 3) {
							if (args[1] == "=") {
								__op_singoc_str(=);
							} else if (args[1] == "+=") {
								__op_singoc_str(+=);
							} else __throw(3);
						} else if (args.size() == 5) {
							if (args[3] == "+") {
								__op_proceeds_str(+);
							} else __throw(3);
						} else __throw(3);
						break;
					case 'S':
						ps = getArrayz(args[0]);
						dnid = getIntval(ps.second);
						if (args.size() == 3) {
							if (args[1] == "=") {
								__op_singoa_str(=);
							} else if (args[1] == "+=") {
								__op_singoa_str(+=);
							} else __throw(3);
						} else if (args.size() == 5) {
							if (args[3] == "+") {
								__op_proceeds_str(+);
							} else __throw(3);
						} else __throw(3);
						break;
					default:
						__throw(12);
						break;
				}
			}
		} // else: comment.
		cont: i++; // continuing running next
		fcont: ;// force continuing running next 
	}
	ret: ;
	for (map<string,pair<int*,int> >::iterator it = int_arr.begin(); it != int_arr.end(); it++) {
		delete[] it->second.first;
	}
	for (map<string,pair<string*,int> >::iterator it = str_arr.begin(); it != str_arr.end(); it++) {
		delete[] it->second.first;
	}
	return rets;
}

#endif 
