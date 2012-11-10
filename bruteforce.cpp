/*
* Copyright (C) 2011, William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Compile Instructions:
 * 
 * g++ -O2 -o bruteforce bruteforce.cpp
 *
*/

#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>

using namespace std;

char ALPHA_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0";
char alpha_map[] = "abcdefghijklmnopqrstuvwxyz\0";
char num_map[] = "0123456789\0";
char special_map[] = "~`!@#$%^&*()-_=+[]{}\\|;:'\",<.>/?\0";
char specialcom_map[] = "!@#$%^&*.,+=_-?\0";

ptrdiff_t myrandom (ptrdiff_t i) { return rand()%i;}
ptrdiff_t (*p_myrandom)(ptrdiff_t) = myrandom;

class BruteForce {
	private:
		map<int,char> charmap;
		map<int,int> passmap;
		int search_length, charmap_size;
		bool nomore;
		void step() { // step password
			register int x;
			passmap[0] += 1; // inc one
			// Do the rounds
			for(x=0; x < search_length; x++) {
				if(passmap[x] > charmap_size) {
					passmap[x] = 1;
					if((x+1) == search_length) {
						nomore = true;
					} else
						passmap[x+1] += 1;
				}
			}
		}
		void init(string bmap, int length) {
			search_length = length;
			charmap_size = bmap.length();
			register int x;
			nomore=false;
			for(x=0; x < bmap.length(); x++)
				charmap[x+1] = bmap[x]; // index the charmap
			for(x=0; x < length; x++)
				passmap[x] = 1; // set initial value	
		}
	public:
		BruteForce(string bmap, int length) {
			init(bmap, length);
		}
		bool moar() {
			step();
			return !nomore;
		}
		string next_password() {
			string ret;
			register int x;
			for(x=0; x < search_length; x++)
				ret.append(1, charmap[passmap[x]]);
			return ret;
		}
		~BruteForce() {
		}
};

void usage(char *p) {
	cout << "BruteForce By William Welna <SanguineRose@OccultusTerra.com>" << endl << endl;
	cout << "Usage Instructions:" << endl << endl;
	cout << p << " -Aanscp <password length> [<pre|post>fix]" << endl;
	cout << "\tA - upper case map \'" << ALPHA_map << "'" << endl;
	cout << "\tl - lower case map \'" << alpha_map << "'" << endl;
	cout << "\tn - numbers map \'" << num_map << "'" << endl;
	cout << "\ts - special map \'" << special_map << "'" << endl;
	cout << "\tc - special common map \'" << specialcom_map << "'" << endl;
	cout << "\tr - randomize character map" << endl;
	cout << "\tp - append string to end rather then at beginning" << endl << endl;
	exit(0);
}

int main(int argc, char **argv) {
	string password_map;
	bool postfix=false, randomize=false;;
	if(argc < 3)
		usage(argv[0]);
	srand(time(NULL));
	if(argv[1][0] == '-') {
		int x;
		for(x=1; x < string(argv[1]).length(); x++) {
			switch(argv[1][x]) {
				case 'A':
					password_map.append(ALPHA_map);
					break;
				case 'a':
					password_map.append(alpha_map);
					break;
				case 'n':
					password_map.append(num_map);
					break;
				case 's':
					password_map.append(special_map);
					break;
				case 'c':
					password_map.append(specialcom_map);
					break;
				case 'p':
					postfix=true;
					break;
				case 'r':
					randomize=true;
					break;
				default:
					cout << "Unknown Flag " << argv[1][x] << endl;
			}
		}
		try {
			if(randomize) 
				random_shuffle(password_map.begin(), password_map.end());
			if(argc == 4) {
				if(postfix==false) {
					string prefix(argv[3]);
					BruteForce b(password_map, boost::lexical_cast<int>(argv[2]));
					while(b.moar())
						cout << prefix << b.next_password() << endl;
				} else {
					string postfix(argv[3]);
					BruteForce b(password_map, boost::lexical_cast<int>(argv[2]));
					while(b.moar())
						cout << b.next_password() << postfix << endl;
				}
			} else {
				BruteForce b(password_map, boost::lexical_cast<int>(argv[2]));
				while(b.moar())
					cout << b.next_password() << endl;
			}
		} catch(exception &e) {
			cout << e.what() << endl;
		}
	} else
		usage(argv[0]);
	return 0;
}
