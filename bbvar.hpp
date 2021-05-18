#include <map>
#include <vector>
#include <string>
#include <cstdio>
using namespace std;
// Surely requires C++11

#ifndef _BBVAR_
#define _BBVAR_

// Process start with "array_" means array.
#define PROMPT_OUT_OF_RANGE true

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
		bool count(string varName) {
			return bool(varinfo[varinfo.size()-1].count(varName)) || bool(arrinfo[arrinfo.size()-1].count(varName));
		}
		bool countall(string varName) {
			for (auto i = varinfo.begin(); i != varinfo.end(); i++) {
				if (i->count(varName)) return true;
			}
			for (auto i = arrinfo.begin(); i != arrinfo.end(); i++) {
				if (i->count(varName)) return true;
			}
			return false;
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
	private:
		vector<map<string,_varType> > varinfo;
		vector<map<string,pair<_varType*,int> > > arrinfo;
};

#endif
