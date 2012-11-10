/* @Sanguinarious <Sanguinarious@OccultusTerra.com> */

#define SIZE 200

int push_string(char **buff, int buff_len, char *string, int string_len) {
	int x=0;
	char *t = buff[0]; 
	for(x=1; x <= buff_len; ++x) // Move everything down
		buff[x-1] = buff[x];
	buff[buff_len] = t; // Append to end
	my_memcpy(t, 4096, string, string_len); // Put string in buffer
	return 0;
}

int my_memcpy(void *dst, int dst_len, void *src, int src_len) {
	int x;
	char *t1=dst, *t2=src;
	for(x=0; x <= dst_len-2 && x <= src_len-1; ++x) {
		*t1 = *t2;
		*t1++; *t2++;
	}
	*t1++; *t1=0; // zero pad
	return x;
}

char **allocate_buffer(int buff_len, int line_length) {
	int x;
	char **buff = calloc(buff_len, sizeof(char *));
	if(!buff)
		return NULL;
	for(x=0; x <= buff_len; ++x) {
		buff[x] = calloc(line_length, sizeof(char));
		if(!buff[x])
			exit(-1); // Fuck...
	}
	return buff;
}


