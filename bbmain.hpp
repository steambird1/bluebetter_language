#include "shellexec.hpp"
#include <string>
#include <map>
#include <stack>
#include <vector>
using namespace std;

#ifndef _BLUEBETTER_
#define _BLUEBETTER_ 1

#define __throw(errid) int_var["error"]=errid; goto cont

bool isContain(string a,string b) {
	return a.find(b) != string::npos;
}
 
int __getIntval(string exp,map<string,int> int_varlist,map<string,pair<int*,int> > int_arrlist) {
	if (isdigit(exp[0])) {
		return atoi(exp.c_str());
	} else if (isContain(exp,"\"")) {
		return 0;
	} else if (isContain(exp,"(")) {// or ")"
		char buf1[1024],buf2[1024];
		sscanf(exp.c_str(),"%s(%s)",&buf1,&buf2);
		string bf1 = buf1;
		int z = __getIntval(string(buf2),int_varlist,int_arrlist);
		if (!int_arrlist.count(bf1)) return 0;
		if (z < 0 || z >= int_arrlist[bf1].second) return 0;
		return int_arrlist[bf1].first[z];
	} else {
		if (!int_varlist.count(exp)) return 0;
		return int_varlist[exp];
	}
} 

#define getIntval(exp) __getIntval(exp,int_var,int_arr)

string __getStrval(string exp,map<string,int> int_varlist,map<string,pair<int*,int> > int_arrlist,map<string,string> str_varlist,map<string,pair<string*,int> > str_arrlist) {
	if (isdigit(exp[0])) {
		return "";
	} else if (isContain(exp,"\"")) {
		return exp.substr(1,exp.length()-1);
	} else if (isContain(exp,"(")) {// or ")"
		char buf1[1024],buf2[1024];
		sscanf(exp.c_str(),"%s(%s)",&buf1,&buf2);
		string bf1 = buf1;
		int z = __getIntval(string(buf2),int_varlist,int_arrlist);
		if (!str_arrlist.count(bf1)) return "";
		if (z < 0 || z >= str_arrlist[bf1].second) return "";
		return str_arrlist[bf1].first[z];
	} else {
		if (!str_varlist.count(exp)) return "";
		return str_varlist[exp];
	}
}
#define getStrval(exp) __getStrval(exp,int_var,int_arr,str_var,str_arr)

#define __op_comp_int(op) curr = (__getIntval(args[i],int_varlist,int_arrlist) op __getIntval(args[i+2],int_varlist,int_arrlist))
#define __op_comp_str(op) curr = (__getStrval(args[i],int_varlist,int_arrlist,str_varlist,str_arrlist) op __getStrval(args[i+2],int_varlist,int_arrlist,str_varlist,str_arrlist)) 
#define __op_proceed_int(op) int_var[iprocd[0]] = getIntval(iprocd[2]) op getIntval(iprocd[4])

#define skipLines(opname) int j = i, stack = 0; \
						while (lines[j] != "end "#opname && stack <= 0) { \
							vector<string> argt = split_arg(lines[j],true,' '); \
							if (argt[0]==#opname) stack++; \
							if (argt[0]=="end") stack--; \
							j++; \
						} \
						i = j

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
}

#define getCond(exp) __getCond(exp,int_var,int_arr,str_var,str_arr)

int runCode(string code) {
	map<string,int> int_var, callist;
	map<string,string> str_var;
	map<string,pair<int*,int> > int_arr; // an memory space and length
	map<string,pair<string*,int> > str_arr;
	vector<string> lines = spiltLines(code);
	stack<int> calltrace;
	int i = 0; // executor pointing
	while (i < lines.size()) {
		vector<string> args = split_arg(lines[i],true,' ');
		if (args.size() != 0 && args[0][0]!='#') {
			string attl = lines[i].substr(args[0].length()+1); // getting other things EXCEPT command
			if (args[0]=="int") {
				vector<string> ivars = split_arg(attl,true,',');
				for (vector<string>::iterator it = ivars.begin(); it != ivars.end(); it++) {
					vector<string> iprocd = split_arg(*it,true,' ');
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
					if (iprocd.size() == 3) {
						// a = (can only be) b
						if (iprocd[1] != "=") __throw(1);
						str_var[iprocd[0]] = getStrval(iprocd[2]);
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
						char buf1[1024],buf2[1024];
						sscanf(iprocd[1].c_str(),"%s(%s)",buf1,buf2);
						int len = getIntval(buf2);
						if (iprocd[0]=="int") {
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
					callist[args[1]] = i+1;
				}
			} else if (args[0]=="for") {
				// for i = a..b (2)
				// collecting informations first
				char buf1[1024], buf2[1024];
				int begin,end,step=1;
				if (args.size()!=4 && args.size()!=6) {
					__throw(6);
				}
				sscanf(args[3].c_str(),"%s..%s",buf1,buf2);
				begin = getIntval(string(buf1)); end = getIntval(string(buf2));
				if (args.size()==6) {
					step = atoi(args[5].c_str());
				}
				// first time running this?
				if (!int_var.count(args[1])) {
					// yes
					int_var[args[1]] = begin;
				} else {
					if (int_var[args[1]] >= end) { // REMEMBER THIS OPERATOR!
						int_var.erase(args[1]);
						skipLines(for);
					} else {
						int_var[args[1]] += step;
					}
				}
			} else if (args[0]=="call") {
				if (args.size() != 2) __throw(7);
				if (!callist.count(args[1])) __throw(7);
				calltrace.push(i);
				i = callist[args[1]];
			} else if (args[0]=="if" || args[0]=="elseif") {
				if (!getCond(attl)) {
					//skipLines(if);
					int j = i, stack = 0;
					while (true) {
						vector<string> argt = split_arg(lines[j],true,' ');
						if (argt[0] == "if") stack++;
						if (argt[0] == "end") stack--;
						if (argt[0] == "elseif" && stack == 0) break;
						if (argt[0] == "else" && stack == 0) break;
					}
					i = j;
				}
			} else if (args[0]=="elseif" || args[0]=="else") {
				//;
			} else if (args[0]=="end") {
				if (args.size() == 1) {
					if (calltrace.empty()) return 0;
					i = calltrace.top();
					calltrace.pop();
				}
			} else if (args[0]=="read") {
				
			} else if (args[0]=="readline") {
				
			} else if (args[0]=="readkey") {
				
			} else if (args[0]=="write") {
				
			} else if (args[0]=="while") {
				
			} else if (args[0]=="next") {
				
			} else if (args[0]=="do") {
				
			} else if (args[0]=="ret") {
				
			} else {
				// setting variable value.
			}
		} // else: comment.
		cont: i++; // continuing running next
	}
}

#endif 
