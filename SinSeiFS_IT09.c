#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

static const char *dirPath = "/home/daniel/Downloads";
char atbash[1000] = "~!@#$%^&*(~!@#$%^&*()-=_+,./?>)-=_+,./?><;:[]}{\|\<;:[]}{|\0123457890123456789ABCDEFGHIJZYXWVUTSRQABCDEFGHIJKLMNOPQRSTPONMLKJIHGKLMNOPQRSTabcdefghijzyxwvutsrqabcdefghijklmnopqrstponmlkjihgklmnopqrst";
char rot[1000] = "~!@#$%^&*(~!@#$%^&*()-=_+,./?>)-=_+,./?><;:[]}{\|\<;:[]}{|\0123457890123456789ABCDEFGHIJNOPQRSTUVWABCDEFGHIJKLMNOPQRSTXYZABCDEFGKLMNOPQRSTabcdefghijnopqrstuvwabcdefghijklmnopqrstxyzabcdefgklmnopqrst";

void writeLog(char *nama, char *fpath)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char haha[1000];

	FILE *file;
	file = fopen("/home/daniel/SinSeiFS.log", "a");

	if (strcmp(nama, "RMDIR") == 0 || strcmp(nama, "UNLINK") == 0)
		sprintf(haha, "WARNING::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, fpath);
	else
		sprintf(haha, "INFO::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, fpath);

	fputs(haha, file);
	fclose(file);
	return;
}

char *key = "SISOP";
char *atoz = "AtoZ_";
char *rx = "RX_";
char *aisa = "A_is_a_";
int x = 0;

void writeLog2(char *nama, const char *from, const char *to)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char haha[1000];

	FILE *file;
	file = fopen("/home/daniel/SinSeiFS.log", "a");

	if (strcmp(nama, "RMDIR") == 0 || strcmp(nama, "UNLINK") == 0)
		sprintf(haha, "WARNING::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, from, to);
	else
		sprintf(haha, "INFO::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, from, to);

	fputs(haha, file);
	fclose(file);
	return;
}

int split_ext_id(char *path)
{
	int ada = 0;
	for(int i=strlen(path)-1; i>=0; i--){
		if (path[i] == '.'){
			if(ada == 1) return i;
			else ada = 1;
		}
	}
	return strlen(path);
}

int ext_id(char *path)
{
	for(int i=strlen(path)-1; i>=0; i--){
		if (path[i] == '.') return i;
	}
	return strlen(path);
}

int slash_id(char *path, int mentok)
{
	for(int i=0; i<strlen(path); i++){
		if (path[i] == '/') return i + 1;
	}
	return mentok;
}

void enc_Atbash(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("encrypt path Atbash: %s\n", path);
	
	int endid = split_ext_id(path);
	if(endid == strlen(path)) endid = ext_id(path);
	int startid = slash_id(path, 0);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp = path[i];
			if(isupper(path[i])) tmp -= 'A';
			else tmp -= 'a';
			tmp = 25 - tmp; //Atbash cipher
			if(isupper(path[i])) tmp += 'A';
			else tmp += 'a';
			path[i] = tmp;
		}
	}
}

void dec_Atbash(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("decrypt path Atbash: %s\n", path);
	
	int endid = split_ext_id(path);
	if(endid == strlen(path)) endid = ext_id(path);
	int startid = slash_id(path, endid);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp = path[i];
			if(isupper(path[i])) tmp -= 'A';
			else tmp -= 'a';
			tmp = 25 - tmp; //Atbash cipher
			if(isupper(path[i])) tmp += 'A';
			else tmp += 'a';
			path[i] = tmp;
		}
	}
}

void enc_Rot13(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("encrypt path ROT13: %s\n", path);
	
	int endid = split_ext_id(path);
	int startid = slash_id(path, 0);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp = path[i];
			if(isupper(path[i])) tmp -= 'A';
			else tmp -= 'a';
			tmp = (tmp + 13) % 26; //ROT13 cipher
			if(isupper(path[i])) tmp += 'A';
			else tmp += 'a';
			path[i] = tmp;
		}
	}
}

void dec_Rot13(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("decrypt path ROT13: %s\n", path);
	
	int endid = split_ext_id(path);
	int startid = slash_id(path, endid);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp = path[i];
			if(isupper(path[i])) tmp -= 'A';
			else tmp -= 'a';
			tmp = (tmp + 13) % 26; //ROT13 cipher
			if(isupper(path[i])) tmp += 'A';
			else tmp += 'a';
			path[i] = tmp;
		}
	}
}

void enc_Vigenere(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("encrypt path Vigenere: %s\n", path);
	
	int endid = split_ext_id(path);
	int startid = slash_id(path, 0);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp1 = path[i];
			char tmp2 = key[(i-startid) % strlen(key)];
			if(isupper(path[i])){
				tmp1 -= 'A';
				tmp2 -= 'A';
				tmp1 = (tmp1 + tmp2) % 26; //Vigenere cipher
				tmp1 += 'A';
			}
			else{
				tmp1 -= 'a';
				tmp2 = tolower(tmp2) - 'a';
				tmp1 = (tmp1 + tmp2) % 26; //Vigenere cipher
				tmp1 += 'a';
			}
			path[i] = tmp1;
		}
	}
}

void dec_Vigenere(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("decrypt path Vigenere: %s\n", path);
	
	int endid = split_ext_id(path);
	int startid = slash_id(path, endid);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp1 = path[i];
			char tmp2 = key[(i-startid) % strlen(key)];
			if(isupper(path[i])){
				tmp1 -= 'A';
				tmp2 -= 'A';
				tmp1 = (tmp1 - tmp2 + 26) % 26; //Vigenere cipher
				tmp1 += 'A';
			}
			else{
				tmp1 -= 'a';
				tmp2 = tolower(tmp2) - 'a';
				tmp1 = (tmp1 - tmp2 + 26) % 26; //Vigenere cipher
				tmp1 += 'a';
			}
			path[i] = tmp1;
		}
	}
}

void enkripsi2(char *fpath)
{
	chdir(fpath);
	DIR *dp;
	struct dirent *dir;
	
	dp = opendir(".");
	if(dp == NULL) return;
	struct stat lol;
	char dirPath[1000];
	char filePath[1000];
	
    while ((dir = readdir(dp)) != NULL){
		printf("dirname %s\n", dir->d_name);
		printf("%s/%s\n", fpath, dir->d_name);
		if (stat(dir->d_name, &lol) < 0);
		else if (S_ISDIR(lol.st_mode)){
			if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0) continue;
			sprintf(dirPath,"%s/%s",fpath, dir->d_name);
			enkripsi2(dirPath);
			printf("dirpath %s\n", dirPath);
		}
		else{
			sprintf(filePath,"%s/%s",fpath,dir->d_name);
			FILE *input = fopen(filePath, "r");
			if (input == NULL) return;
			int bytes_read, count = 0;
			void *buffer = malloc(1024);
			while((bytes_read = fread(buffer, 1, 1024, input)) > 0){
				char newFilePath[1000];
				sprintf(newFilePath, "%s.%04d", filePath, count);
				count++;
				FILE *output = fopen(newFilePath, "w+");
				if(output == NULL) return;
				fwrite(buffer, 1, bytes_read, output);
				fclose(output);
				memset(buffer, 0, 1024);
			}
			fclose(input);
			printf("filepath %s\n", filePath);
			remove(filePath);
		}
	}
    closedir(dp);
}

void dekripsi2(char *dir)
{
	chdir(dir);
	DIR *dp;
	struct dirent *de;
	struct stat lol;
	dp = opendir(".");
	if (dp == NULL) return;
	
	char dirPath[1000];
	char filePath[1000];
	
	while ((de = readdir(dp)) != NULL){
		if (stat(de->d_name, &lol) < 0);
		else if (S_ISDIR(lol.st_mode)){
			if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
			sprintf(dirPath, "%s/%s", dir, de->d_name);
			dekripsi2(dirPath);
		}
		else{
			sprintf(filePath, "%s/%s", dir, de->d_name);
			filePath[strlen(filePath) - 5] = '\0';
			FILE *check = fopen(filePath, "r");
			if (check != NULL) return;
			FILE *file = fopen(filePath, "w");
			int count = 0;
			char topath[1000];
			sprintf(topath, "%s.%04d", filePath, count);
			void *buffer = malloc(1024);
			while (1){
				FILE *op = fopen(topath, "rb");
				if (op == NULL) break;
				size_t readSize = fread(buffer, 1, 1024, op);
				fwrite(buffer, 1, readSize, file);
				fclose(op);
				remove(topath);
				count++;
				sprintf(topath, "%s.%04d", filePath, count);
			}
			free(buffer);
			fclose(file);
		}
	}
	closedir(dp);
}

void getBinary(char *fname, char *bin, char *lowercase){
	int endid = ext_id(fname);
	int startid = slash_id(fname, 0);
	int i;
	
	for(i=startid; i<endid; i++){
		if(isupper(fname[i])){
			bin[i] = '1';
			lowercase[i] = fname[i] + 32;
		}
		else{
			bin[i] = '0';
			lowercase[i] = fname[i];
		}
	}
	bin[endid] = '\0';
	
	for(; i<strlen(fname); i++){
		lowercase[i] = fname[i];
	}
	lowercase[i] = '\0';
}

int bin_to_dec(char *bin){
	int tmp = 1, res = 0;
	for(int i=strlen(bin)-1; i>=0; i--){
		if(bin[i] == '1') res += tmp;
		tmp *= 2;
	}
	return res;
}

void encryptBinary(char *fpath)
{
	chdir(fpath);
	DIR *dp;
	struct dirent *dir;
	struct stat lol;
	dp = opendir(".");
	if(dp == NULL) return;
	
	char dirPath[1000];
	char filePath[1000];
	char filePathBinary[1000];
	
    while ((dir = readdir(dp)) != NULL){
		if (stat(dir->d_name, &lol) < 0);
		else if (S_ISDIR(lol.st_mode)){
			if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0) continue;
			sprintf(dirPath,"%s/%s",fpath, dir->d_name);
			encryptBinary(dirPath);
		}
		else{
			sprintf(filePath,"%s/%s",fpath, dir->d_name);
			char bin[1000], lowercase[1000];
			getBinary(dir->d_name, bin, lowercase);
			int dec = bin_to_dec(bin);
			sprintf(filePathBinary,"%s/%s.%d",fpath,lowercase,dec);
			rename(filePath, filePathBinary);
		}
	}
    closedir(dp);
}

int convertDec(char *ext){
	int dec = 0, pengali = 1;
	for(int i=strlen(ext)-1; i>=0; i--){
		dec += (ext[i]-'0')*pengali;
		pengali *= 10;
	}
	return dec;
}

void dec_to_bin(int dec, char *bin, int len){
	int idx = 0;
	while(dec){
		if(dec & 1) bin[idx] = '1';
		else bin[idx] = '0';
		idx++;
		dec /= 2;
	}
	while(idx < len){
		bin[idx] = '0';
		idx++;
	}
	bin[idx] = '\0';
	
	for(int i=0; i<idx/2; i++){
		char tmp = bin[i];
		bin[i] = bin[idx-1-i];
		bin[idx-1-i] = tmp;
	}
}

void getDecimal(char *fname, char *bin, char *normalcase){
	int endid = ext_id(fname);
	int startid = slash_id(fname, 0);
	int i;
	
	for(i=startid; i<endid; i++){
		if(bin[i-startid] == '1') normalcase[i-startid] = fname[i] - 32;
		else normalcase[i-startid] = fname[i];
	}
	
	for(; i<strlen(fname); i++){
		normalcase[i-startid] = fname[i];
	}
	normalcase[i-startid] = '\0';
}

void decryptBinary(char *fpath)
{
	chdir(fpath);
	DIR *dp;
	struct dirent *dir;
	struct stat lol;
	dp = opendir(".");
	if(dp == NULL) return;
	
	char dirPath[1000];
	char filePath[1000];
	char filePathDecimal[1000];
	
    while ((dir = readdir(dp)) != NULL){
		if (stat(dir->d_name, &lol) < 0);
		else if (S_ISDIR(lol.st_mode)){
			if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0) continue;
			sprintf(dirPath,"%s/%s",fpath, dir->d_name);
			decryptBinary(dirPath);
		}
		else{
			sprintf(filePath,"%s/%s",fpath, dir->d_name);
			char fname[1000], bin[1000], normalcase[1000], clearPath[1000];
			
			strcpy(fname, dir->d_name);
			char *ext = strrchr(fname, '.');
			int dec = convertDec(ext+1);
			for(int i=0; i<strlen(fname)-strlen(ext); i++) clearPath[i] = fname[i];
			
			char *ext2 = strrchr(clearPath, '.');
			dec_to_bin(dec, bin, strlen(clearPath)-strlen(ext2));
			getDecimal(clearPath, bin, normalcase);
			sprintf(filePathDecimal,"%s/%s",fpath,normalcase);
			rename(filePath, filePathDecimal);
		}
	}
    closedir(dp);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = lstat(fpath, stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	//int rot13 = 0;
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime)){
			dec_Rot13(b);
			dec_Atbash(b);
		//	rot13 = 1;
		//}
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	if (x != 24) x++;
	else writeLog("READDIR", fpath);

	int res = 0;
	DIR *dp;
	struct dirent *de;

	(void)offset;
	(void)fi;

	dp = opendir(fpath);
	//printf("lalalal %s\n", fpath);
	if (dp == NULL) return -errno;

	while ((de = readdir(dp)) != NULL){
		if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

		//printf("before: %s\n", de->d_name);
		//printf("a: %s\n", a);
		if (a != NULL) enc_Atbash(de->d_name);
		//printf("b: %s\n", b);
		if (b != NULL){
			//if(rot13)
			enc_Atbash(de->d_name);
			enc_Rot13(de->d_name);
			//else
			//enc_Vigenere(de->d_name);
		}
		//printf("after: %s\n", de->d_name);

		res = (filler(buf, de->d_name, &st, 0));
		if (res != 0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	int res = 0;
	int fd = 0;

	(void)fi;
	writeLog("READ", fpath);

	fd = open(fpath, O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = mkdir(fpath, mode);
	writeLog("MKDIR", fpath);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = rmdir(fpath);
	writeLog("RMDIR", fpath);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];
	
	char *a = strstr(to, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(from, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(from, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}
	
	char *c = strstr(to, rx);
	if (c != NULL){
		//struct stat stats;
		//stat(from, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(c);
		dec_Atbash(c);
		//else
		//dec_Vigenere(c);
	}

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = rename(frompath, topath);
	if (res == -1) return -errno;

	writeLog2("RENAME", frompath, topath);
	
	if (c != NULL){
		enkripsi2(topath);
		writeLog2("ENCRYPT2", from, to);
	}

	if (b != NULL && c == NULL){
		dekripsi2(topath);
		writeLog2("DECRYPT2", from, to);
	}
	
	if (strstr(to, aisa) != NULL){
		encryptBinary(topath);
		writeLog2("ENCRYPT3", from, to);
	}
	
	if (strstr(from, aisa) != NULL && strstr(to, aisa) == NULL){
		decryptBinary(topath);
		writeLog2("DECRYPT3", from, to);
	}

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	writeLog("TRUNC", fpath);
	res = truncate(fpath, size);
	
	if (res == -1) return -errno;
	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	(void)fi;
	writeLog("WRITE", fpath);

	fd = open(fpath, O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	(void)fi;
	writeLog("CREATE", fpath);
	
	res = creat(fpath, mode);	
	if (res == -1) return -errno;

	close(res);
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = access(fpath, mask);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = open(fpath, fi->flags);

	writeLog("OPEN", fpath);
	if (res == -1) return -errno;

	close(res);
	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = unlink(fpath);
	writeLog("UNLINK", fpath);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = readlink(fpath, buf, size - 1);
	writeLog("READLINK", fpath);
	if (res == -1) return -errno;

	buf[res] = '\0';
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	writeLog("MKNOD", fpath);
	
	/* On Linux this could just be 'mknod(path, mode, rdev)' but this is more portable */
	if (S_ISREG(mode)){
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) res = close(res);
	}
	else if (S_ISFIFO(mode)) res = mkfifo(fpath, mode);
	else res = mknod(fpath, mode, rdev);
	
	if (res == -1) return -errno;
	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];
	
	char *a = strstr(to, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(from, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(from, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}
	
	char *c = strstr(to, rx);
	if (c != NULL){
		//struct stat stats;
		//stat(from, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(c);
		dec_Atbash(c);
		//else
		//dec_Vigenere(c);
	}

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = symlink(frompath, topath);
	writeLog2("SYMLINK", frompath, topath);
	
	if (res == -1) return -errno;
	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];
	
	char *a = strstr(to, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(from, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(from, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}
	
	char *c = strstr(to, rx);
	if (c != NULL){
		//struct stat stats;
		//stat(from, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(c);
		dec_Atbash(c);
		//else
		//dec_Vigenere(c);
	}

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = link(frompath, topath);
	writeLog2("LINK", frompath, topath);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = chmod(fpath, mode);
	writeLog("CHMOD", fpath);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = lchown(fpath, uid, gid);
	writeLog("CHOWN", fpath);
	
	if (res == -1) return -errno;
	return 0;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) dec_Atbash(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		//struct stat stats;
		//stat(path, &stats);
		//if(ctime(&stats.st_ctime) == ctime(&stats.st_mtime))
		dec_Rot13(b);
		dec_Atbash(b);
		//else
		//dec_Vigenere(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = statvfs(fpath, stbuf);
	writeLog("STATFS", fpath);
	
	if (res == -1) return -errno;
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.truncate = xmp_truncate,
	.write = xmp_write,
	.create = xmp_create,
	.utimens = xmp_utimens,
	.access = xmp_access,
	.open = xmp_open,
	.unlink = xmp_unlink,
	.readlink = xmp_readlink,
	.mknod = xmp_mknod,
	.symlink = xmp_symlink,
	.link = xmp_link,
	.chmod = xmp_chmod,
	.chown = xmp_chown,
	.statfs = xmp_statfs,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
