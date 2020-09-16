#include"eoPlatformRegister.h"
#include<stdio.h>
#include<stdlib.h>

int EOCL::ReadOCLCodeFromFile(const char * path, unsigned char **raw_data, int* pSize)
{
	int res = 0;
	FILE *fp = 0;
	unsigned char *data_file = 0;
	int read_size = 0, mem_size = 0;

	fp = fopen(path, "rb");
	if (!fp) {
		res = -1;
		goto exit;
	}
	fseek(fp, 0, SEEK_END);
	read_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	mem_size = read_size + 1;

	data_file = (unsigned char *)malloc(sizeof(unsigned char)* mem_size);
	if (!data_file) {
		res = -2;
		goto exit;
	}
	if (read_size != fread(data_file, sizeof(unsigned char), read_size, fp)) {
		res = -3;
		goto exit;
	}
	data_file[read_size] = '\0';
	*raw_data = data_file;
	data_file = NULL;
exit:
	if (fp) fclose(fp);
	if (data_file) {
		free(data_file);
		data_file = NULL;
	}
	if (NULL != pSize) {
		*pSize = mem_size;
	}
	return res;
}