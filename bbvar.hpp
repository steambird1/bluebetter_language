#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include "shellexec.hpp" 
using namespace std;
// Surely requires C++11

#ifndef _BBVAR_
#define _BBVAR_

// Process start with "array_" means array.
#define PROMPT_OUT_OF_RANGE true

template <typename _varType> class _varlist;
int __getIntval(string exp,_varlist<int> int_list);//decl

template <typename _varType>
class _varlist {
	public:
		_varlist() {
			push(); 
		}
		void push(void) {
			varinfo.push_back(*(new map<string,_varType>));
			arrinfo.push_back(*(new map<string,pair<_varType*,int> >));
		}
		void pop(void) {
			varinfo.pop_back(); 
			arrinfo.pop_back();
		}
		int count(string varName) {
			return (varinfo[varinfo.size()-1].count(varName)*2) + (arrinfo[arrinfo.size()-1].count(varName));
		}
		int countall(string varName) {
			int a = 0;
			for (auto i = varinfo.begin(); i != varinfo.end(); i++) {
				if (i->count(varName)) {
					a += 2;
					break;
				}
			}
			for (auto i = arrinfo.begin(); i != arrinfo.end(); i++) {
				if (i->count(varName)) {
					a += 1;
					break;
				}
			}
			return a;
		}
		bool declare(string varName, _varType value) {
			if (this->count(varName)) return false; //declaring so you can't
			varinfo.back()[varName]=value;
			return true;
		}
		bool array_declare(string varName, int size) {
			if (this->count(varName)) return false;
			arrinfo.back()[varName]=make_pair(new _varType[size],size);
		}
		bool set(string varName, _varType value) {
			for (auto i = varinfo.end()-1; i != varinfo.begin()-1; i--) {
				if (i->count(varName)) {
					(*i)[varName] = value;
					return true;
				}
			}
			return false;
		}
		int array_size(string varName) {
			for (auto i = arrinfo.end()-1; i != arrinfo.begin()-1; i--) {
				if (i->count(varName)) {
					return (*i)[varName].second;
				}
			}
			return -1;
		}
		bool array_set(string varName, int pos, _varType value) {
			for (auto i = arrinfo.end()-1; i != arrinfo.begin()-1; i--) {
				if (i->count(varName)) {
					if (pos > (*i)[varName].second) return false;
					(*i)[varName].first[pos] = value;
					return true;
				}
			}
			return false;
		}
		// If variable does not exist, return value undefined.
		_varType get(string varName) {
			for (auto i = varinfo.end()-1; i != varinfo.begin()-1; i--) {
				if (i->count(varName)) {
					return (*i)[varName];
				}
			}
			if (PROMPT_OUT_OF_RANGE) printf("\nError: Getting value out of range\n");
		}
		_varType* array_getall(string varName) {
			for (auto i = arrinfo.end()-1; i != arrinfo.begin()-1; i--) {
				if (i->count(varName)) {
					return (*i)[varName].first;
				}
			}
			if (PROMPT_OUT_OF_RANGE) printf("\nError: Getting value out of range\n");
		}
		// If out of range / variable does not exist, return value undefined.
		_varType array_get(string varName, int pos) {
			for (auto i = arrinfo.end()-1; i != arrinfo.begin()-1; i--) {
				if (i->count(varName)) {
					return (*i)[varName].first[pos];
				}
			}
			if (PROMPT_OUT_OF_RANGE) printf("\nError: Getting value out of range\n");
		}
		void free(void) {
			for (auto i = arrinfo.begin(); i != arrinfo.end(); i++) {
				for (auto j = i->begin(); j != i->end(); j++) {
					delete[] j->second.first;
				}
			}
		}
		~_varlist() {
			this->free();
		}
		friend int __getIntval(string,_varlist<int>);
		friend string __getStrval(string,_varlist<int>,_varlist<string>);
	private:
		vector<map<string,_varType> > varinfo;
		vector<map<string,pair<_varType*,int> > > arrinfo;
};

int __getIntval(string exp,_varlist<int> int_list) {
	if (isdigit(exp[0])) {
		return atoi(exp.c_str());
	} else if (exp[0]=='"') {
		return 0;
	} else if (isContain(exp,"(")) {// or ")"
		pair<string,string> a = getArrayz(exp);
		string buf1 = a.first,buf2 = a.second;
		int z = __getIntval(string(buf2),int_list);
		if (int_list.countall(buf1)&1) {
			if (z < 0 || z >= int_list.array_size(buf1)) return 0;
			return int_list.array_get(buf1,z);
		} else return 0;
	} else {
		if (int_list.countall(exp)&2) return int_list.get(exp);
		else return 0;
	}
} 

string __getStrval(string exp,_varlist<int> int_list,_varlist<string> str_list) {
	if (isdigit(exp[0])) {
		return exp; // automaticly stringz
	} else if (exp[0]=='"') {
		return exp.substr(1,exp.length()-2);
	} else if (isContain(exp,"(")) {// or ")"
		pair<string,string> a = getArrayz(exp);
		string buf1 = a.first,buf2 = a.second;
		int z = __getIntval(string(buf2),int_list);
		if (int_list.countall(buf1)&1) {
			if (z < 0 || z >= str_list.array_size(buf1)) return 0;
			return str_list.array_get(buf1,z);
		} else return 0;
	} else {
		if (str_list.countall(exp)&2) return str_list.get(exp);
		else if (int_list.countall(exp)&2) return to_string(int_list.get(exp));
		else return "";
	}
}

#endif
