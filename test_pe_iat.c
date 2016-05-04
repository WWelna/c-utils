/*
* Copyright (C) 2016 William H. Welna All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// For some reason this is always broken so it gets defined here
typedef struct {
	union {
		DWORD Characteristics;
		PIMAGE_THUNK_DATA OriginalFirstThunk;
	};
	DWORD TimeDateStamp;
	DWORD ForwarderChain;
	DWORD Name;
	PIMAGE_THUNK_DATA FirstThunk;
} MY_IMAGE_IMPORT_DESCRIPTOR;

int newp() {
	printf("NEWP\n");
	return 0;
}

int write_protect_address(void *p, void *d) {
	MEMORY_BASIC_INFORMATION i;
	DWORD t;
	void **p2=(void **)p;
	if(!VirtualQuery(p, &i, sizeof(MEMORY_BASIC_INFORMATION)))
		return -1;
	if(!VirtualProtect(i.BaseAddress, i.RegionSize, PAGE_EXECUTE_READWRITE, &i.Protect))
		return -2;
	p2[0]=d; // This void** recast is to make gcc/mingw happy
	if(!VirtualProtect(i.BaseAddress, i.RegionSize, i.Protect, &t))
		return -3;
	return 0;
}

int mylen(char *s) {
    int c=0;
    char *p;
    for(p=s; *p != '\0'; *p++)
	    c++;
    return c;
}

int mycmp(char *s1, char *s2, int size) {
    int x;
    for(x=0; x < size; x++)
        if(s1[x] != s2[x])
            return 0;
    return 1;
}

int mysafecmp(char *s1, char *s2) {
	int s1_size = mylen(s1), s2_size = mylen(s2);
	if(s1_size > s2_size)
		return mycmp(s1, s2, s2_size);
	else
		return mycmp(s1, s2, s1_size);
}

char *upper(char *s) {
	char *p;
	for(p=s; *p!=0; *p++) {
		if((*p > 96) && (*p < 123)) {
			*p=*p-32;
		}
	}
	return s;
}

void *self_iat_patch(char *fname, void *new_address) {
	void *base = (void *)0x400000;
	void *old_address=NULL;
	IMAGE_FILE_HEADER *image_header;
	IMAGE_OPTIONAL_HEADER *optional_header;
	MY_IMAGE_IMPORT_DESCRIPTOR *ip, *Imports;
	unsigned int *el = (unsigned int *)0x40003c;
	int x, ret;
	
	x=*(el);
	image_header = x+base+4;
	optional_header = (void *)image_header+sizeof(IMAGE_FILE_HEADER);
	Imports=(MY_IMAGE_IMPORT_DESCRIPTOR *)(((char *)base)+optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	for(ip=Imports; ip->Name!=0; ip++) {
		PIMAGE_IMPORT_BY_NAME thunkData; 
		DWORD *thunk, thunkref;
		FARPROC *addrs;
		thunkref=(DWORD)ip->FirstThunk; addrs=(FARPROC *)(((void *)base)+thunkref);
		thunkref=(DWORD)ip->OriginalFirstThunk; thunk=(DWORD *)(((char *)base)+thunkref);
		for(x=0; thunk[x]!=0; ++x) {
			if(!IMAGE_SNAP_BY_ORDINAL(thunk[x])) {
				thunkData=(PIMAGE_IMPORT_BY_NAME)(((char *)base)+thunk[x]);
				if(mysafecmp(fname, thunkData->Name)) {
					old_address=addrs[x];
					if(!write_protect_address(&addrs[x], new_address))
						printf("Patched %p to %p -> %s\n", old_address, addrs[x], thunkData->Name);
				}
			}
		}
	}
	return old_address;
}

int main(int argc, char **argv) {
	self_iat_patch("fprintf", &newp);
	fprintf(stdout, "test");
	fprintf(stdout, "test");
	fprintf(stdout, "test");
	fprintf(stdout, "test");
}

