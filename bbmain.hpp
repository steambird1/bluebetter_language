#include "shellexec.hpp"
#include "bbvar.hpp" 
#include <string>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <conio.h>
using namespace std;

#ifndef _BLUEBETTER_
#define _BLUEBETTER_ 1

#define BLUEBETTER_VER "v202106"
// It's really different, so we do this

// Preparing REAL NUMBER. 

#define DEBUG_MODE false
#define STEP_BY_STEP false
#define PRINT_ERROR_INFO true
#define EXIT_IN_ERROR true

#define ifdebug if(DEBUG_MODE)
#define debugs(...) ifdebug printf(__VA_ARGS__)

// It'll be error if you don't do this!!!
#define __throw(errid) do { if (PRINT_ERROR_INFO) printf("\nAn error was occured.\n\nError id: %d\n",(errid*1024)+__LINE__); if (DEBUG_MODE) printf("Error line in interpreter: %d\n\n",__LINE__); if (EXIT_IN_ERROR) return 0-errid; goto cont ;} while (false)

struct strProcs {
	string bsname;
	int bsbegin;
	int bsend;
};

inline strProcs __makeStrprocs(string bsname,int bsbegin,int bsend) {
	strProcs _t; _t.bsbegin=bsbegin; _t.bsend=bsend; _t.bsname=bsname; return _t;
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

strProcs __getSproc(string exp,_varlist<int> int_list,_varlist<string> str_list,_varlist<double> real_list) {
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
		return __makeStrprocs(s.first,__getIntval(p[0],int_list,str_list,real_list),__getIntval(p[1],int_list,str_list,real_list));
	} else {
		int at = __getIntval(s.second,int_list,str_list,real_list);
		return __makeStrprocs(s.first,at,at);
	}
}

#define getSproc(exp) __getSproc(exp,int_list,str_list,real_list)


char __getVartype(string exp,_varlist<int> int_list,_varlist<string> str_list,_varlist<double> real_list) {
	if (isContain(exp,"(")) {
		pair<string,string> a = getArrayz(exp);
		if (int_list.countall(a.first)&1) return 'I';
		if (str_list.countall(a.first)&1) return 'S';
		if (real_list.countall(a.first)&1) return 'R';
		return '?';
	} else {
		if (int_list.countall(exp)&2) return 'i';
		if (str_list.countall(exp)&2) return 's';
		if (real_list.countall(exp)&2) return 'r';
		return '?';
	}
}
#define getVartype(exp) __getVartype(exp,int_list,str_list,real_list)

#define __op_comp_int(op) curr = (__getIntval(args[i],int_list,str_list,real_list) op __getIntval(args[i+2],int_list,str_list,real_list))
#define __op_comp_real(op) curr = (__getRealval(args[i],int_list,str_list,real_list) op __getRealval(args[i+2],int_list,str_list,real_list))
#define __op_comp_str(op) curr = (__getStrval(args[i],int_list,str_list,real_list) op __getStrval(args[i+2],int_list,str_list,real_list)) 

#define __op_proceed_int(op) int_list.declare(iprocd[0],getIntval(iprocd[2]) op getIntval(iprocd[4]))
#define __op_proceed_real(op) real_list.declare(iprocd[0],getRealval(iprocd[2]) op getRealval(iprocd[4]))
#define __op_proceeds_int(op) int_list.set(args[0],getIntval(args[2]) op getIntval(args[4]))
#define __op_proceeds_real(op) real_list.set(args[0],getRealval(args[2]) op getRealval(args[4]))
#define __op_proceeds_str(op) str_list.set(args[0],getStrval(args[2]) op getStrval(args[4]))
// str_var[args[0]] = getStrval(args[2]) op getStrval(args[4])
#define __op_proceeda_int(op) int_list.array_set(ps.first,dnid,getIntval(args[2]) op getIntval(args[4]))
#define __op_proceeda_real(op) real_list.array_set(ps.first,dnid,getRealval(args[2]) op getRealval(args[4]))
//int_arr[ps.first].first[dnid] = getIntval(args[2]) op getIntval(args[4])
#define __op_proceeda_str(op) str_list.array_set(ps.first,dnid,getStrval(args[2]) op getStrval(args[4])) 
// str_arr[ps.first].first[dnid] = getStrval(args[2]) op getStrval(args[4])
#define __op_singoc_int(op) do { \
	int t = int_list.get(args[0]); int_list.set(args[0],t op getIntval(args[2])); \
} while (false)
#define __op_singoc_real(op) do { \
	double t = real_list.get(args[0]); real_list.set(args[0],t op getRealval(args[2])); \
} while (false)
#define __op_singoc_str(op) do { \
	string t = str_list.get(args[0]); str_list.set(args[0],t op getIntval(args[2])); \
} while (false)
// str_var[args[0]] op getStrval(args[2])
#define __op_singrc_int(op) do { \
	int t = int_list.get(args[0]); int_list.set(args[0],(op getIntval(args[2]))); \
} while (false)
// int_var[args[0]] = (op getIntval(args[2]))
#define __op_singoa_int(op) do { \
	int t = int_list.array_get(ps.first,dnid); int_list.array_set(ps.first,dnid,t op getIntval(args[2])); \
} while (false)
#define __op_singoa_real(op) do { \
	double t = real_list.array_get(ps.first,dnid); real_list.array_set(ps.first,dnid,t op getRealval(args[2])); \
} while (false)
// int_list.array_set(args[0],dnid,t op getIntval(args[2]))
// int_arr[ps.first].first[dnid] op getIntval(args[2])
#define __op_singoa_str(op) do { \
	string t = str_list.array_get(ps.first,dnid); str_list.array_set(ps.first,dnid,t op getIntval(args[2])); \
} while (false)
// str_arr[ps.first].first[dnid] op getStrval(args[2])
#define __op_singra_int(op) do { \
	int t = int_list.array_get(ps.first,dnid); int_list.array_set(ps.first,dnid,(op getIntval(args[2]))); \
} while (false)
//int_arr[ps.first].first[dnid] = (op getIntval(args[2]))

#define skipLines(opname) do { int j = i, stack = 0; \
						while (true) { \
							vector<string> argt = split_arg(lines[j],true,' '); \
							if (argt[0]=="while" || argt[0]=="for" || argt[0]=="do" || argt[0]=="if") stack++; \
							if (argt[0]=="end") stack--; \
							if (lines[j] == (string("end ") + opname) && stack <= 0) break; \
							j++; \
						} \
						i = j+1; goto fcont; } while (false)

bool __getCond(string exp,_varlist<int> int_list,_varlist<string> str_list,_varlist<double> real_list) {
	vector<string> args = split_arg(exp,true,' ');
	bool stat; 
	for (int i = 0; i < args.size(); i+=4) { // a op b op
		bool curr;
		char g0 = __getVartype(args[0],int_list,str_list,real_list);
		switch (g0) {
			case 'i': case 'I':
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
		}  else return false;
				break;
			case 'r': case 'R':
				if (args[i+1]=="==") {
			__op_comp_real(==);
		} else if (args[i+1]=="!=") {
			__op_comp_real(!=);
		} else if (args[i+1]==">") {
			__op_comp_real(>);
		} else if (args[i+1]==">=") {
			__op_comp_real(>=);
		} else if (args[i+1]=="<") {
			__op_comp_real(<);
		} else if (args[i+1]=="<=") {
			__op_comp_real(<=);
		}  else return false;
				break;
			case 's': case 'S':
				if (args[i+1]=="==") {
			__op_comp_str(==);
		} else if (args[i+1]=="!=") {
			__op_comp_str(!=);
		} else return false;
				break;
			default:
				return false; // how do you compare this?
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

#define getCond(exp) __getCond(exp,int_list,str_list,real_list)

struct _call {
	int callist;
	vector<pair<char,string> > call_var;
	char ret_type;
};

struct _ret {
	char ret_type;
	string var_name;
};

char nameToType(string at, bool arrayw) {
	char s = '?';
	if (at == "int" || at == "char") s = 'i';
	else if (at == "str") s = 's';
	if (arrayw) s = toupper(s);
	return s;
}

#define varpush() do { int_list.push(); str_list.push(); debugs("Pushing all (cur = %d,%d)\n",int_list.length(),str_list.length()); } while (false)
#define varpop() do { int_list.pop(); str_list.pop(); debugs("Poping all (cur = %d,%d)\n",int_list.length(),str_list.length()); } while (false)
#define varfree() do { int_list.free(); str_list.free(); debugs("Freeing all (cur = %d,%d)\n",int_list.length(),str_list.length()); } while (false)

#define set_swtc(set_name,val) do { if (set_name.count(val)) {set_name.erase(val);} else {set_name.insert(val);} } while (false)

int __runCode(string code, bool debugger, bool pipe) {
	map<string,_call> callist;
	_varlist<int> int_list;
	_varlist<string> str_list; 
	_varlist<double> real_list;
	map<int,int> ijumpto; // after you execute ..., you should jump to ...
	vector<string> lines = spiltLines(code);
	stack<int> calltrace;
	_ret getting; // Getting value type
	set<pair<string,string> > watch;// Watchpoint (debugger)
	set<int> breakp;//Breakpoint (debugger)
	set<pair<int,string> > breakc; // Conditional breakpoint (debugger) 
	int i = 0,rets = 0; // executor pointing
	char debug_cmd[2048];
	bool flag = true, breaknext = false;
	if (debugger) {
		printf("Welcome to BlueBetter Debugger\n\n");
		int bp,pt,t,t1,t2;
		pair<int,string> mp;
		do {
			printf(">>> ");
			gets(debug_cmd);
			vector<string> dcmd = split_argw(string(debug_cmd),true,' ');
			switch (dcmd[0][0]) {
				case 'n': case 'c': case 'j': case 's':
					printf("Error: program not running\n");
					break;
				case 'r':
					flag = false;
					break;
				case 'b':
					if (dcmd.size() != 2) {
						printf("Error: value not given\n");
						break;
					}
					bp = atoi(dcmd[1].c_str());
					set_swtc(breakp,bp);
					break;
				case 'd':
					if (dcmd.size() != 3) {
						printf("Error: value not given\n");
						break;
					}
					bp = atoi(dcmd[1].c_str());
					mp = make_pair(bp,dcmd[2]);
					set_swtc(breakc,mp);
					break;
				case 'l':
					for (auto i = breakp.begin(); i != breakp.end(); i++) {
						printf("Normal	%d\n",(*i));
					}
					for (auto i = breakc.begin(); i != breakc.end(); i++) {
						printf("Normal	%d	%s\n",i->first,i->second.c_str());
					}
					break;
				case 'w':
					// watch type value
					if (dcmd.size() != 3) {
						printf("Error: value not given\n");
						break;
					}
					set_swtc(watch,make_pair(dcmd[1],dcmd[2]));
					break;
				case 't':
					for (auto i = watch.begin(); i != watch.end(); i++) {
						printf("Watch	[%s] %s\n",i->first.c_str(),i->second.c_str());
					}
					break;
				case 'v':
					switch (dcmd.size()) {
						case 1:
							pt = 0;
							for (auto i = lines.begin(); i != lines.end(); i++) {
								printf("%3d %s\n",++pt,i->c_str()); 
							}
							break;
						case 2:
							t = atoi(dcmd[1].c_str());
							if (t < 1 || t > lines.size()) {
								printf("Error: line out of range\n");
								break;
							}
							printf("%s\n",lines[t-1].c_str());
							break;
						case 3:
							t1 = atoi(dcmd[1].c_str()), t2 = atoi(dcmd[2].c_str());
							if (t1 < 1 || t1 > lines.size() || t2 < 1 || t2 > lines.size()) {
								printf("Error: line out of range\n");
								break;
							}
							for (int i = t1 - 1; i < t2; i++) printf("%3d %s\n",i+1,lines[i].c_str());
							break;
					}
					break;
				case 'q':
					break;
			}
		} while (flag);
	}
	while (i < lines.size()) {
		debugs("Executing: %s\n",lines[i].c_str());
		vector<string> args = split_arg(lines[i],true,' ');
		if (args.size() != 0 && args[0][0]!='#') {
			string attl;
			if (args.size() > 1) attl = lines[i].substr(args[0].length()+1); // getting other things EXCEPT command
			else attl = "";
			if (args[0]=="int" || args[0]=="char") {
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					vector<string> iprocd = split_arg(*it,true,' ');
					if (int_list.count(iprocd[0])) __throw(1); 
					if (iprocd.size() == 3) {
						// a = (can only be) b
						if (iprocd[1] != "=") __throw(1);
						int_list.declare(iprocd[0],getIntval(iprocd[2]));
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
			} else if (args[0] == "real") {
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					vector<string> iprocd = split_arg(*it,true,' ');
					if (int_list.count(iprocd[0])) __throw(1); 
					if (iprocd.size() == 3) {
						// a = (can only be) b
						if (iprocd[1] != "=") __throw(1);
						real_list.declare(iprocd[0],getRealval(iprocd[2]));
					} else if (iprocd.size() == 5) {
						// a = (can only be) b op c
						if (iprocd[1] != "=") __throw(1);
						// what about "op"?
						if (iprocd[3] == "+") {
							__op_proceed_real(+);
						} else if (iprocd[3] == "-") {
							__op_proceed_real(-);
						} else if (iprocd[3] == "*") {
							__op_proceed_real(*);
						} else if (iprocd[3] == "/") {
							__op_proceed_real(/);
						} else { // double does not support bit and mod operation.
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
					if (str_list.count(iprocd[0])) __throw(1);
					if (iprocd.size() == 3) {
						// a = (can only be) b
						if (iprocd[1] != "=") {
							__throw(1);
						}
						string strv = getStrval(iprocd[2]);
						str_list.declare(iprocd[0],strv);
					} else if (iprocd.size() == 5) {
						// a = (can only be) b op c
						if (iprocd[1] != "=") __throw(1);
						// what about "op"?
						if (iprocd[3] == "+") {
							str_list.declare(iprocd[0],getStrval(iprocd[2]) + getStrval(iprocd[4]));
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
							//int_arr[string(buf1)] = make_pair(new int[len],len);
							if (!int_list.array_declare(string(buf1),len)) __throw(4);
						} else if (iprocd[0]=="str") {
							//str_arr[string(buf1)] = make_pair(new string[len],len);
							if (!str_list.array_declare(string(buf1),len)) __throw(4);
						} else if (iprocd[0]=="real") {
							if (!real_list.array_declare(string(buf1),len)) __throw(4);
							debugs("Countor: %d\n",real_list.count(string(buf1)));
						} else {
							__throw(5);
						}
					} else {
						__throw(4); 
					}
				}
			} else if (args[0]=="sub") {
				// Modified to: parameters supports ( sub a(int a, str b) )
				if (args.size() < 2) {
					__throw(5);
				} else {
					//callist[args[1]] = i;
					// Just push and set variables in "call".
					pair<string,string> s = getArrayz(attl);
					vector<pair<char,string> > callw;
					vector<string> ivars = split_arg(s.second,true,',');
					for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
						vector<string> iprocd = split_arg(*it,true,' ');
						switch (iprocd.size()) {
							case 2:
								callw.push_back(make_pair(nameToType(iprocd[0],false),iprocd[1]));
								break;
							default:
								__throw(20);
								break;
						}
					}
					_call cw = {i,callw,'/'};
					callist[args[1]] = cw;
					skipLines("sub");
				}
			} else if (args[0]=="for") {
				// for i = a..b (2)
				// collecting informations first
				int begin,end,step=1,is;
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
				if (begin == end) goto stop_for; // can't continue running
				// first time running this?
				debugs("for Counting: %d\n",int_list.countall(args[1]));
				debugs("int layer: %d	str layer: %d\n",int_list.length(),str_list.length());
				if (!(int_list.count(args[1])&2)) { // So you shouldn't declare the same variable outside.
					// yes
					debugs("Preparing for\n"); 
					int_list.push(); // protect area
					int_list.declare(args[1],begin);
					varpush();
					//int_var[args[1]] = begin;
				} else {
					debugs("Looper = %d\n",int_list.get(args[1]));
					is = int_list.get(args[1]) + step; 
					int_list.set(args[1],is);
					//int_var[args[1]] += step;
					if (is == end) { // REMEMBER THIS OPERATOR!
						stop_for: int_list.pop(); // ending
						//str_list.pop();
						//varpop();
						skipLines("for");
						 // MUST JUMP TO NEXT!!!!!
					} else varpush();
				}
			} else if (args[0]=="call") { // call a or call a(x) (DON'T REFER TO AN ARRAY)
				if (args.size() < 1) __throw(21);
				pair<string,string> ga = getArrayz(attl);
				if (!callist.count(ga.first)) __throw(7);
				calltrace.push(i); // don't run call again.
				vector<string> ivars = split_arg(ga.second,true,',');
				if (ivars.size() != callist[ga.first].call_var.size()) __throw(21);
				debugs("Preparing variable pushing: \n");
				varpush();
				for (int i = 0; i < ivars.size(); i++) {
					switch (callist[ga.first].call_var[i].first) {
						case 'i':
							int_list.declare(callist[ga.first].call_var[i].second,getIntval(ivars[i]));
							break;
						case 's':
							str_list.declare(callist[ga.first].call_var[i].second,getStrval(ivars[i]));
							break;
					}
				}
				getting = {'?',""}; // not looking for returns
				//varpush();
				i = callist[ga.first].callist;
			} else if (args[0]=="function") {
				//...
				if (args.size() < 1) __throw(22);
				string attl2 = attl.substr(args[1].length()+1);
				pair<string,string> s = getArrayz(attl2);
					vector<pair<char,string> > callw;
					vector<string> ivars = split_arg(s.second,true,',');
					for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
						vector<string> iprocd = split_arg(*it,true,' ');
						switch (iprocd.size()) {
							case 2:
								callw.push_back(make_pair(nameToType(iprocd[0],false),iprocd[1]));
								break;
							default:
								__throw(20);
								break;
						}
					}
					_call cw = {i,callw,nameToType(args[1],false)};
					callist[s.first] = cw;
					skipLines("function");
			} else if (args[0]=="let") { // ADDED KEYWORD (let a = func(args,args))
				// It's declarings
				// write RET code first
				if (args.size() < 4 || args[2] != "=") __throw(21);
				string attw = "";
				for (int i = 3; i < args.size()-1; i++) attw = attw + args[i] + " ";
				attw = attw + args[args.size()-1];
				pair<string,string> ga = getArrayz(attw);
				debugs("Calling function: %s (%s)\n",ga.first.c_str(),ga.second.c_str());
				if (!callist.count(ga.first)) __throw(7);
				if (callist[ga.first].ret_type == '/') __throw(21); // trying to get a sub's return 
				calltrace.push(i); // don't run call again.
				vector<string> ivars = split_arg(ga.second,true,',');
				if (ivars.size() != callist[ga.first].call_var.size()) __throw(21);
				varpush(); 
				debugs("Preparing function call... with ivars = %d\n",ivars.size());
				for (int i = 0; i < ivars.size(); i++) {
					switch (callist[ga.first].call_var[i].first) {
						case 'i':
							int_list.declare(callist[ga.first].call_var[i].second,getIntval(ivars[i]));
							break;
						case 's':
							str_list.declare(callist[ga.first].call_var[i].second,getStrval(ivars[i]));
							break;
						case 'r':
							real_list.declare(callist[ga.first].call_var[i].second,getRealval(ivars[i]));
							break;
					}
				}
				getting = {callist[ga.first].ret_type,args[1]};
				i = callist[ga.first].callist;
			} else if (args[0]=="if" || args[0]=="elseif") {
				if (!getCond(attl)) {
					//skipLines(if);
					int j = i+1, stack = 0;
					while (true) {
						vector<string> argt = split_arg(lines[j],true,' ');
						ifdebug printf("S=%d ",stack);
						if (argt[0] == "if") stack++;
						if (argt[0] == "end" && argt.size() >= 2 && argt[1] == "if") stack--;
						if (stack < 0) break;
						if (argt[0] == "elseif" && stack == 0) break;
						if (argt[0] == "else" && stack == 0) break;
						j++;
					}
					ifdebug printf(" -- Jumping to %d\n",j); 
					i = j;
					goto fcont;
				} else {
					int j = i+1, stack = 0, be;
					while (true) {
						vector<string> argt = split_arg(lines[j],true,' ');
						if (argt[0] == "if") stack++;
						if (argt[0] == "end" && argt.size() >= 2 && argt[1] == "if") stack--;
						if (stack < 0) break;
						if (argt[0] == "elseif" && stack == 0) break;
						if (argt[0] == "else" && stack == 0) break;
						j++;
					}
					//i = j-1;
					be = j-1;
					j = i, stack = 0; 
						while (true) { 
							vector<string> argt = split_arg(lines[j],true,' '); 
							if (argt[0]=="while" || argt[0]=="for" || argt[0]=="do" || argt[0]=="if") stack++; 
							if (argt[0]=="end") stack--; 
							if (lines[j] == "end if" && stack <= 0) break; 
							j++; 
						} 
						ijumpto[be]=j+1;
					varpush();
					ifdebug printf("Jump to %d in %d then\n",j+1,be);
				}
			} else if (args[0]=="else") {
				//;
				varpush();
			} else if (args[0]=="end") { // remember to skip to the next line of "end"!
				if (args.size() == 1 || (args.size() == 2 && (args[1] == "sub" || args[1] == "function"))) {
					if (calltrace.empty()) {
						rets = 0;
						goto ret;
					}
					i = calltrace.top();
					calltrace.pop();
				} else {
					varpop();
					if (args[1] == "if") goto cont;
					int j = i, stack = 0;
						while (true) { 
							vector<string> argt = split_arg(lines[j],true,' '); 
							if (argt[0]=="while" || argt[0]=="for" || argt[0]=="do" || argt[0]=="if") stack++; 
							if (argt[0]=="end") stack--; 
							ifdebug printf("S=%d ",stack);
							if (argt[0]==args[1] && stack == 0) break;
							j--; 
							
						} 
						i = j; goto fcont;
				//		skipLines(args[1]);
				ifdebug	printf("\n");
				}
			} else if (args[0]=="read") {
				// read int a, char b, str c (variables are declaring!)
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					vector<string> iprocd = split_arg(*it,true,' ');
					ifdebug printf(" Inputting %s: ",it->c_str());
					if (iprocd.size() == 2) {
						int readi;
						double readr;
						if (iprocd[0]=="int") {
							scanf("%d",&readi);
							//int_var[iprocd[1]]=readi;
							int_list.declare(iprocd[1],readi);
						} else if (iprocd[0]=="char") {
							scanf("%c",&readi);
							//int_var[iprocd[1]]=readi;
							int_list.declare(iprocd[1],readi);
						} else if (iprocd[0]=="str") {
							char buf[1024];
							scanf("%s",buf);
							//str_var[iprocd[1]]=string(buf);
							str_list.declare(iprocd[1],string(buf));
						} else if (iprocd[0]=="real") {
							scanf("%lf",&readr);
							real_list.declare(iprocd[1],readr);
						} else __throw(10);
					} else __throw(10);
				}
			} else if (args[0]=="readline") {
				// readline a (str)
				if (args.size() != 2) __throw(10);
				char buf[1024];
				gets(buf);
				//str_var[args[1]]=string(buf);
				str_list.declare(args[1],string(buf));
			} else if (args[0]=="readkey") {
				// readkey a (int)
				if (args.size() != 2) __throw(10);
				//int_var[args[1]]=getch();
				int_list.declare(args[1],getch());
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
						} else if (iprocd[0]=="real") {
							printf("%lf",getRealval(iprocd[1]));
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
				if (calltrace.empty()) {
					rets = getIntval(args[1]); 
					goto ret;
				} else {
					int si;
					string ss;
					double sr;
					switch (getting.ret_type) {
						case 'i':
							si = getIntval(args[1]);
							varpop();
							int_list.declare(getting.var_name,si);
							break;
						case 's':
							ss = getStrval(args[1]);
							varpop();
							str_list.declare(getting.var_name,ss);
							break;
						case 'r':
							sr = getRealval(args[1]);
							varpop();
							real_list.declare(getting.var_name,sr);
						default:
							// do nothing yet
							varpop();
							break;
					}
					
					i = calltrace.top();
					calltrace.pop(); 
				}
			} else if (args[0]=="get") { // Supports variable only; It's declaring
				if (args.size() != 4 || args[2] != "=") __throw(12);
				strProcs spc = getSproc(args[3]);
				string strv = getStrval(spc.bsname);
				if (spc.bsbegin==spc.bsend) {
					if (spc.bsbegin >= strv.length()) __throw(13);
					//int_var[args[1]] = strv[spc.bsbegin];
					int_list.declare(args[1],strv[spc.bsbegin]);
				} else {
					if (spc.bsbegin > spc.bsend || spc.bsend >= strv.length()) __throw(13);
					//str_var[args[1]] = strv.substr(spc.bsbegin,spc.bsend-spc.bsbegin);
					str_list.declare(args[1],strv.substr(spc.bsbegin,spc.bsend-spc.bsbegin));
				}
			} else if (args[0]=="set") {
				if (args.size() != 4 || args[2] != "=") __throw(12);
				strProcs spc = getSproc(args[1]);
				if (!str_list.countall(spc.bsname)&2) __throw(12);
				string strv = getStrval(spc.bsname);
				if (spc.bsbegin==spc.bsend) {
					if (spc.bsbegin >= strv.length()) __throw(13);
					strv[spc.bsbegin] = char(getIntval(args[3]));
					str_list.set(spc.bsname,strv);
				} else {
					if (spc.bsbegin > spc.bsend || spc.bsend >= strv.length()) __throw(13);
					string s = getStrval(args[3]);
					for (int i = 0; i < (spc.bsend-spc.bsbegin); i++) {
						strv[i+spc.bsbegin] = s[i];
						str_list.set(spc.bsname,strv);
					}
				}
			} else if (args[0]=="len") {
				// getting length of string. len x = y
				if (args.size() != 4) __throw(14);
				if (args[2] != "=") __throw(14);
				int_list.declare(args[1],getStrval(args[3]).length());
			} else if (args[0]=="erase") {
				// erase a component of string. (int id / int comp) erase x(a)  erase x(a:b)
				if (args.size() != 2) __throw(15);
				strProcs spc = getSproc(args[1]);
				string strv = str_list.get(spc.bsname);
				if (spc.bsbegin==spc.bsend) {
					if (spc.bsbegin >= strv.length()) __throw(15);
					strv.erase(strv.begin()+spc.bsbegin); 
				} else {
					if (spc.bsbegin > spc.bsend || spc.bsend > strv.length()) __throw(15);
					strv.erase(strv.begin()+spc.bsbegin,strv.begin()+spc.bsend); 
				}
				str_list.set(spc.bsname,strv);
			} else if (args[0]=="insert") {
				// insert a component of string. (char / str)  insert x(a) char (or str) y (for x)
				if (args.size() != 4 && args.size() != 6) __throw(16);
				strProcs spc = getSproc(args[1]);
				if (!str_list.countall(spc.bsname)&2) __throw(16);
				string strv = str_list.get(spc.bsname);
				if (spc.bsbegin != spc.bsend) __throw(16);
				if (args[2] == "char") {
					int res = getIntval(args[3]), t = 1;
					if (spc.bsbegin > strv.length()) __throw(16);
					if (args.size() == 6) {
						t = getIntval(args[5]);
						if (args[4] != "for" || t <= 0) __throw(17);
					} 
					strv.insert(spc.bsbegin,t,char(res));
				} else if (args[2] == "str") {
					string res = getStrval(args[3]);
					if (spc.bsbegin > strv.length()) __throw(16);
					strv.insert(spc.bsbegin,res);
				} else __throw(16);
				str_list.set(spc.bsname,strv); // HERE
			} else {
				// setting variable value.
				int dnid;
				pair<string,string> ps;
				if (args.size() < 3) __throw(1);
				//debugs("Existw: %d\n",real_list.arrinfo.front().size());
				switch (getVartype(args[0])) {
					case 'i':
						if (args.size() == 3) {
					// a op b (including =! =~)
					if (args[1] == "=") {
						int t = int_list.get(args[0]); int_list.set(args[0],getIntval(args[2]));
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
						int_list.array_set(ps.first,dnid,getIntval(args[2]));
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
								str_list.set(args[0],getStrval(args[2]));
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
								str_list.array_set(args[0],dnid,getStrval(args[2]));
							} else if (args[1] == "+=") {
								__op_singoa_str(+=);
							} else __throw(3);
						} else if (args.size() == 5) {
							if (args[3] == "+") {
								__op_proceeds_str(+);
							} else __throw(3);
						} else __throw(3);
						break;
					case 'r':
						if (args.size() == 3) {
					// a op b (including =! =~)
					if (args[1] == "=") {
						real_list.set(args[0],getRealval(args[2]));
					} else if (args[1] == "+=") {
						__op_singoc_real(+=);
					} else if (args[1] == "-=") {
						__op_singoc_real(-=);
					} else if (args[1] == "*=") {
						__op_singoc_real(*=);
					} else if (args[1] == "/=") {
						__op_singoc_real(/=);
					} else __throw(3);
				} else if (args.size() == 5) {
					// a = b op c
					if (args[1] != "=") __throw(1);
					if (args[3] == "+") {
							__op_proceeds_real(+);
						} else if (args[3] == "-") {
							__op_proceeds_real(-);
						} else if (args[3] == "*") {
							__op_proceeds_real(*);
						} else if (args[3] == "/") {
							__op_proceeds_real(/);
						} else {
							__throw(3);
						}
				} else __throw(3);
						break;
					case 'R':
						ps = getArrayz(args[0]);
						dnid = getIntval(ps.second);
						debugs("Setting %s(%d)\n",ps.first.c_str(),dnid);
						if (args.size() == 3) {
					// a op b (including =! =~)
					if (args[1] == "=") {
						double rv = getRealval(args[2]);
						debugs("previous = %lf\n",rv);
						int stat = real_list.array_set(ps.first,dnid,rv);
						debugs("state = %d\n",stat);
					//	int_arr = getIntval(args[2]); // debug replacement
					} else if (args[1] == "+=") {
						__op_singoa_real(+=);
					} else if (args[1] == "-=") {
						__op_singoa_real(-=);
					} else if (args[1] == "*=") {
						__op_singoa_real(*=);
					} else if (args[1] == "/=") {
						__op_singoa_real(/=);
					} else __throw(3);
				} else if (args.size() == 5) {
					// a = b op c
					if (args[1] != "=") __throw(1);
					if (args[3] == "+") {
							__op_proceeda_real(+);
						} else if (args[3] == "-") {
							__op_proceeda_real(-);
						} else if (args[3] == "*") {
							__op_proceeda_real(*);
						} else if (args[3] == "/") {
							__op_proceeda_real(/);
						} else {
							__throw(3);
						}
				} else __throw(3);
						break;
					default:
						__throw(12);
						break;
				}
			}
		} // else: comment.
		cont: ;
		if (ijumpto.count(i)) {
			int tmp = i;
			i = ijumpto[i];
			ijumpto.erase(tmp); // see you then
			varpop();
			ifdebug printf("Jumping to cause %d\n",i);
		}
		else i++; // continuing running next
		fcont: if (debugger) {
		int bp,pt,t,t1,t2;
		pair<int,string> mp;
		// is breakpoint?
		// i already added
		if (breakp.count(i-1)) {
			breaknext = true;
		}
		// is conditional breakpoint?
		for (auto it = breakc.begin(); it != breakc.end(); it++) {
			if (it->first == (i-1) && getCond(it->second)) {
				breaknext = true;
				break;
			} 
		}
		bool flag2 = breaknext;
		while (flag2) {
			printf("Execution at line %d\n%s\n",i,lines[i-1].c_str());
			for (auto it = watch.begin(); it != watch.end(); it++) {
				if (it->first == "int") {
					printf("Watch: %s = %d\n",it->second.c_str(),getIntval(it->second));
				} else if (it->first == "str") {
					printf("Watch: %s = \"%s\"\n",it->second.c_str(),getStrval(it->second).c_str());
				}
			}
			flag2 = true;
			// watchpoints
			printf(">>> ");
			gets(debug_cmd);
			vector<string> dcmd = split_argw(string(debug_cmd),true,' ');
			switch (dcmd[0][0]) {
				case 'n':
					breaknext = true;
					flag2 = false;
					break;
				case 'c':
					breaknext = false;
					flag2 = false;
					break;
				case 'j':
					if (dcmd.size() != 2) {
						printf("Error: value not given\n");
						break;
					}
					i = atoi(dcmd[1].c_str());
					flag2 = false;
					break;
				case 's':
					// s type x = v
					if (dcmd.size() != 5 || dcmd[3] != "=") {
						printf("Error: value not given\n");
						break;
					}
					if (dcmd[1] == "int" || dcmd[1]=="char") {
						int_list.set(dcmd[2],getIntval(dcmd[4]));
					} else if (dcmd[1] == "str") {
						str_list.set(dcmd[2],getStrval(dcmd[4]));
					} else {
						printf("Error: type not exist");
					}
				case 'r':
					printf("Error: program already running");
					break;
				case 'b':
					if (dcmd.size() != 2) {
						printf("Error: value not given\n");
						break;
					}
					bp = atoi(dcmd[1].c_str());
					set_swtc(breakp,bp);
					break;
				case 'd':
					if (dcmd.size() != 3) {
						printf("Error: value not given\n");
						break;
					}
					bp = atoi(dcmd[1].c_str());
					mp = make_pair(bp,dcmd[2]);
					set_swtc(breakc,mp);
					break;
				case 'l':
					for (auto i = breakp.begin(); i != breakp.end(); i++) {
						printf("Normal	%d\n",(*i));
					}
					for (auto i = breakc.begin(); i != breakc.end(); i++) {
						printf("Normal	%d	%s\n",i->first,i->second.c_str());
					}
					break;
				case 'w':
					// watch type value
					if (dcmd.size() != 3) {
						printf("Error: value not given\n");
						break;
					}
					set_swtc(watch,make_pair(dcmd[1],dcmd[2]));
					break;
				case 't':
					for (auto i = watch.begin(); i != watch.end(); i++) {
						printf("Watch	[%s] %s\n",i->first.c_str(),i->second.c_str());
					}
					break;
				case 'v':
					switch (dcmd.size()) {
						case 1:
							pt = 0;
							for (auto i = lines.begin(); i != lines.end(); i++) {
								printf("%3d %s\n",++pt,i->c_str()); 
							}
							break;
						case 2:
							t = atoi(dcmd[1].c_str());
							if (t < 1 || t > lines.size()) {
								printf("Error: line out of range\n");
								break;
							}
							printf("%s\n",lines[t-1].c_str());
							break;
						case 3:
							t1 = atoi(dcmd[1].c_str()), t2 = atoi(dcmd[2].c_str());
							if (t1 < 1 || t1 > lines.size() || t2 < 1 || t2 > lines.size()) {
								printf("Error: line out of range\n");
								break;
							}
							for (int i = t1 - 1; i < t2; i++) printf("%3d %s\n",i+1,lines[i].c_str());
							break;
					}
					break;
				case 'q':
					return -2;
					break;
				default:
					flag2 = true;
					break;
			}
		}
		
	}
		if (STEP_BY_STEP) system("pause");// force continuing running next 
	}
	ret: ;
	varfree();
	return rets;
}

inline int runCode(string code) {
	return __runCode(code,false,false);
}

#endif 
