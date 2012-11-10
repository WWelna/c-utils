#include <windows.h>
#include <stdio.h>

/* xdump.c - @Sanguinarious <SanguineRose@OccultusTerra.com> */

void dump_array_2(char *name, char *data, int size) {
	int x=0, y=0; unsigned char c;
	fprintf(stdout, "char %s[] = {\n\t", name);
	for(x=0; x < size; ++x) {
		c=data[x];
		if(y == 8) {
			fprintf(stdout, "\n\t");
			y=0;
		}
		if(x == size-1)
			fprintf(stdout, "0x%02X\n", c);
		else
			fprintf(stdout, "0x%02X, ", c);
		++y;
	}
	fprintf(stdout, "};\n");
}

char *loadfile(char *f, int *s) {
	HANDLE hFile = CreateFile(f, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile) { // File is open!
		int totalread, size;
		char *ret;
		if(*s == 0)
			size = GetFileSize(hFile, NULL);
		else
			size = *s;
		ret = calloc(size, sizeof(char)+1);
		if(ret==NULL) {
			CloseHandle(hFile);
			*s=0;
			return NULL;
		}
		if(ReadFile(hFile, ret, size, &totalread, NULL)!=FALSE) {
			CloseHandle(hFile);
			*s=totalread;
			return ret;
		}
		else {
			CloseHandle(hFile);
			*s=0;
			return NULL;
		}
	} else
		return NULL;
}

char *uppercase(char *s, char *tmp) {
	int x, l=strlen(s);
	for(x=0; x < l; ++x)
		tmp[x] = toupper(s[x]);
	tmp[x] = 0;
	return tmp;
}

int main(int argc, char **argv) {
	char tmp[128];
	char *f;
	int x;
	if(argc != 3) exit(1);
	f = loadfile(argv[2], &x);
	if(f == NULL || x == 0) exit(1);
	fprintf(stdout, "#define %s_SIZE %i\n", uppercase(argv[1], tmp), x);
	dump_array_2(argv[1], f, x);
	free(f);
	return 0;
}
