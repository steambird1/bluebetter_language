/*
Bluebetter language from BrainFuck

Transfer map:

>	b
<	l
+	u
-	e
.	t
,	r
[	B
]	E 
*/

#include <map>
#include <cstdio>
using namespace std;
#ifndef _BLUEBETTER_
#define _BLUEBETTER_

typedef map<int,int> memap;

// A program should start with "T" and end with "R"! 
#define T int main() { memap memory; int ptr = 0;
#define R return memory[ptr];}

#define b ++ptr;
#define l --ptr;
#define u ++memory[ptr];
#define e --memory[ptr];
#define t putchar(memory[ptr]);
#define r memory[ptr]=getchar();
#define B while(memory[ptr]) {
#define E }

#endif
