# soal-shift-sisop-modul-2-IT09-2021

Nama Anggota | NRP
------------------- | --------------		
Daniel Evan | 05311940000016
Natasya Abygail N | 05111940000020
Muhammad Naufal Imantyasto | 05111940000041

## Soal 1
Di suatu jurusan, terdapat admin lab baru yang super duper gabut, ia bernama Sin. Sin baru menjadi admin di lab tersebut selama 1 bulan. Selama sebulan tersebut ia bertemu orang-orang hebat di lab tersebut, salah satunya yaitu Sei. Sei dan Sin akhirnya berteman baik. Karena belakangan ini sedang ramai tentang kasus keamanan data, mereka berniat membuat filesystem dengan metode encode yang mutakhir. Berikut adalah filesystem rancangan Sin dan Sei :

```txt
NOTE : 
Semua file yang berada pada direktori harus ter-encode menggunakan Atbash cipher(mirror).
Misalkan terdapat file bernama kucinglucu123.jpg pada direktori DATA_PENTING
“AtoZ_folder/DATA_PENTING/kucinglucu123.jpg” → “AtoZ_folder/WZGZ_KVMGRMT/pfxrmtofxf123.jpg”

Note : filesystem berfungsi normal layaknya linux pada umumnya, Mount source (root) filesystem adalah directory /home/[USER]/Downloads, dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di-encode.

Referensi : https://www.dcode.fr/atbash-cipher
```

a. Jika sebuah direktori dibuat dengan awalan `AtoZ_`, maka direktori tersebut akan menjadi direktori ter-encode.

b. Jika sebuah direktori di-rename dengan awalan `AtoZ_`, maka direktori tersebut akan menjadi direktori ter-encode.

c. Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.

d. Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : **/home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]**

e. Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

#
### Penyelesaian Soal 1
Dibawah ini adalah library yang digunakan :

```C
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
```
Selanjutnya, pada Soal 1 kita diminta untuk melakukan enkripsi maupun dekripsi nama file dan folder dengan Atbash cipher. Untuk mendapatkan nama file dan folder akan dilakukan looping untuk mengecek dimana posisi awal berupa slash (/) dan posisi akhir berupa titik (.), sebagai contoh pada path `AtoZ_folder/DATA_PENTING/hotsauce123.jpg` yang akan diolah adalah `DATA_PENTING/hotsauce123` sehingga menjadi `WZGZ_KVMGRMT/slghzfxv123` sedangkan karakter yang lainnya tidak berubah dan menghasilkan path akhir `AtoZ_folder/WZGZ_KVMGRMT/slghzfxv123.jpg`. Maka kita butuh 3 fungsi untuk mendapatkan index penanda awal dan akhir enkripsi dan dekripsi:

`slash_id` : digunakan untuk **mengembalikan index slash**

```C
int slash_id(char *path, int mentok)
{
	for(int i=0; i<strlen(path); i++){
		if (path[i] == '/') return i + 1;
	}
	return mentok;
}
```
`ext_id` : digunakan untuk **mengembalikan index file extension**
```C
int ext_id(char *path)
{
	for(int i=strlen(path)-1; i>=0; i--){
		if (path[i] == '.') return i;
	}
	return strlen(path);
}
```
`split_ext_id` : digunakan untuk **mengembalikan index file extension pada file yang displit**
```C
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
```
Selanjutnya adalah proses **enkripsi** dan **dekripsi** menggunakan Atbash chiper yang akan dibuatkan function tersendiri.

```C
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
```

Pemanggilan function dekripsi dilakukan pada tiap utility functions **getattr**, **mkdir**, **rename**, **rmdir**, **create**, dan fungsi-fungsi lain yang esensial dalam proses sinkronisasi FUSE dan mount folder. Fungsi dekripsi dan enkripsi dilakukan di utility function readdir karena FUSE akan melakukan dekripsi di mount folder lalu enkripsi di FUSE saat readdir. Pemanggilannya dilakukan dengan pengecekan apakah string `AtoZ_` terdapat di string path di masing-masing utility function dengan menggunakan fungsi `strstr()`. Jika ya, maka fungsi enkripsi dan dekripsi akan dipanggil untuk string tersebut dengan `AtoZ_` sebagai starting point string yang diteruskan.

## Soal 2

Selain itu Sei mengusulkan untuk membuat metode enkripsi tambahan agar data pada komputer mereka semakin aman. Berikut rancangan metode enkripsi tambahan yang dirancang oleh Sei

a. Jika sebuah direktori dibuat dengan awalan `RX_[Nama]`, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai kasus nomor 1 dengan algoritma tambahan ROT13 (Atbash + ROT13).

b. Jika sebuah direktori di-rename dengan awalan `RX_[Nama]`, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai dengan kasus nomor 1 dengan algoritma tambahan Vigenere Cipher dengan key `SISOP` (Case-sensitive, Atbash + Vigenere).

c. Apabila direktori yang terencode di-rename (Dihilangkan `RX_` nya), maka folder menjadi tidak terencode dan isi direktori tersebut akan terdecode berdasar nama aslinya.

d. Setiap pembuatan direktori terencode (mkdir atau rename) akan tercatat ke sebuah log file beserta methodnya (apakah itu mkdir atau rename).

e. Pada metode enkripsi ini, file-file pada direktori asli akan menjadi terpecah menjadi file-file kecil sebesar 1024 bytes, sementara jika diakses melalui filesystem rancangan Sin dan Sei akan menjadi normal. Sebagai contoh, Suatu_File.txt berukuran 3 kiloBytes pada directory asli akan menjadi 3 file kecil yakni:

```txt
Suatu_File.txt.0000
Suatu_File.txt.0001
Suatu_File.txt.0002
```

Ketika diakses melalui filesystem hanya akan muncul Suatu_File.txt
#
### Penyelesaian Soal 2

Dimulai dengan pengecekan apakah direktori direname dengan menambahkan `RX_` atau menghilangkan `RX_` dengan fungsi `strstr()`. Proses ini dilakukan pada utility functions **RENAME**

```C
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
	
	...

	return 0;
}
```
Jika terdeteksi `RX_` pada path tujuan maka direktori direname dengan menambahkan `RX_` dan akan dilanjutkan dengan memecahkan file pada fungsi `enkripsi2`.
```C
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
```
Kemudian, untuk melakukan proses enkripsi dan dekripsi ROT13, dibuatlah function tersendiri dengan nama `enc_Rot13`.
```C
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
```
Pemanggilan function dekripsi dilakukan pada tiap utility functions **getattr**, **mkdir**, **rename**, **rmdir**, **create**, dan fungsi-fungsi lain yang esensial dalam proses sinkronisasi FUSE dan mount folder. Fungsi dekripsi dan enkripsi dilakukan di utility function readdir karena FUSE akan melakukan dekripsi di mount folder lalu enkripsi di FUSE saat readdir. Pemanggilannya dilakukan dengan pengecekan apakah string `RX_` terdapat di string path di masing-masing utility function dengan menggunakan fungsi `strstr()`. Jika ya, maka fungsi enkripsi dan dekripsi akan dipanggil untuk string tersebut dengan `RX_` sebagai starting point string yang diteruskan.

## Soal 3

Karena Sin masih super duper gabut akhirnya dia menambahkan sebuah fitur lagi pada filesystem mereka.

a. Jika sebuah direktori dibuat dengan awalan `A_is_a_`, maka direktori tersebut akan menjadi sebuah direktori spesial.

b. Jika sebuah direktori di-rename dengan memberi awalan `A_is_a_`, maka direktori tersebut akan menjadi sebuah direktori spesial.

c. Apabila direktori yang terenkripsi di-rename dengan menghapus `A_is_a_` pada bagian awal nama folder maka direktori tersebut menjadi direktori normal.

d. Direktori spesial adalah direktori yang mengembalikan enkripsi/encoding pada direktori `AtoZ_` maupun `RX_` namun masing-masing aturan mereka tetap berjalan pada direktori di dalamnya (sifat recursive  `AtoZ_` dan `RX_` tetap berjalan pada subdirektori).

e. Pada direktori spesial semua nama file (tidak termasuk ekstensi) pada fuse akan berubah menjadi lowercase insensitive dan diberi ekstensi baru berupa nilai desimal dari biner perbedaan namanya.

Contohnya jika pada direktori asli nama filenya adalah `FiLe_CoNtoH.txt` maka pada fuse akan menjadi `file_contoh.txt.1321`. 1321 berasal dari biner 10100101001.

#
### Penyelesaian Soal 3
Dimulai dengan pengecekan apakah direktori direname dengan menambahkan `A_is_a_` atau menghilangkan `A_is_a_` dengan fungsi `strstr()`. Proses ini dilakukan pada utility functions **RENAME**

```C
static int xmp_rename(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];
	
	...

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = rename(frompath, topath);
	if (res == -1) return -errno;

	writeLog2("RENAME", frompath, topath);
	
	...
	
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
```
Jika terdeteksi `A_is_a_` pada path tujuan berarti direktori direname dengan menambahkan `A_is_a_`. Kemudian dilanjutkan dengan mengubah nama file menjadi **lowercase** dan menambahkan nilai desimalnya sebagai ekstensi yang baru pada fungsi `encryptBinary`.

```C
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
```
Namun, jika terdeteksi `A_is_a_` pada path asal dan tidak terdeteksi `A_is_a_` pada path tujuan maka direktori direname dengan menghilangkan `A_is_a_`. Dan dilanjutkan dengan mengubah nama file menjadi semula dengan bantuan nilai desimalnya pada fungsi `decryptBinary`.

```C
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
```
## Soal 4
Untuk memudahkan dalam memonitor kegiatan pada filesystem mereka Sin dan Sei membuat sebuah log system dengan spesifikasi sebagai berikut.

a. Log system yang akan terbentuk bernama `SinSeiFS.log` pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.

b. Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.

c. Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.

d. Sisanya, akan dicatat pada level INFO.

e. Format untuk logging yaitu:

```txt
[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]

Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),
MM : 2 digit menit, SS : 2 digit detik, CMD : System Call yang terpanggil, DESC : informasi dan parameter tambahan

INFO::28052021-10:00:00:CREATE::/test.txt
INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt
```

#
### Penyelesaian Soal 4
Pada soal ini kita diminta untuk membuat **log system** yang bertujuan untuk `memudahkan dalam memonitor kegiatan pada file system`. Disini kita membuat dua fungsi dalam pembuatan log system ini yaitu fungsi `writeLog` dan `writeLog2` perbedaannya terdapat pada DESC (informasi dan parameter tambahan) yang perlu dicantumkan dalam format untuk loggingnya. Dalam menuliskan log system sesuai format yang ada kita perlu mencari waktu sekarang untuk nanti dicantumkan dalam log systemnya. Dalam fungsi `writeLog` kita juga memasukkan parameter `char *nama` yang mana adalah System Call dan `char *fpath` adalah deskripsi mengenai file yang ada.

```C
void writeLog(char *nama, char *fpath)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
```
Selanjutnya kita akan melakukan inisialisasi sebuah array of char atau string `haha` yang nantinya bertugas untuk menyimpan perintah **system call** yang telah dijalankan pada filesystem dan mencatatnya dalam file `SinSeiFS.log`. Lalu kita perlu membuka file `SinSeiFS.log` pada direktori home pengguna dengan mode **a (append)** agar nanti bisa dituliskan log yang baru dan jika file belum ada maka akan dibuat file yang baru.
```C
char haha[1000];

	FILE *file;
	file = fopen("/home/daniel/SinSeiFS.log", "a");
```
Langkah Selanjutnya kita bisa melakukan pengecekan pada syscall yang ada pada parameter. Jika syscall adalah `RMDIR` atau `UNLINK` maka log levelnya akan dicatat `WARNING`. Namun jika tidak, maka log levelnya akan dicatat `INFO`. Dan pada log juga akan dicatat waktu pada saat ini beserta keterangan-keterangan lainnya.

```C
if (strcmp(nama, "RMDIR") == 0 || strcmp(nama, "UNLINK") == 0)
		sprintf(haha, "WARNING::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, fpath);
	else
		sprintf(haha, "INFO::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, fpath);
```
Dan yang terakhir kita akan menuliskan log yang ada kedalam file `SinSeiFS.log` dan setelah itu filenya akan ditutup.
```C
fputs(haha, file);
	fclose(file);
	return;
}
```
Lalu,untuk fungsi yang kedua yaitu `writeLog2` sebenarnya kurang lebih sama dengan fungsi `writeLog` yang dijelaskan sebelumnya, namun perbedaan keduanya terdapat pada **parameter yang diberikan dan pada pencatatannya**. Untuk parameternya ada `char *nama` yang merupakan syscall, `const char *from` adalah keterangan file sebelum dilakukannya perintah system call yang dijalankan oleh file system dan `const char *to` keterangan file setelah dilakukannya perintah system call yang dijalankan oleh file system. Lalu pada pencatatannya kurang lebih sama dengan yang sebelumnya namun ditambahkan keterangan sesuai dengan parameter yang diberikan.
```C
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
```
Kemudian untuk implementasinya kita akan memasukkan fungsi-fungsi ini kedalam setiap fungsi system call yang ada.

## Kendala
Untuk kendala yang dialami dalam mengerjakan modul 4 yaitu,
1. Kami masih tidak terlalu paham dengan konsep fuse yang digunakan.
2. Ada beberapa kode yang seharusnya berpengaruh terhadap suatu fungsi tapi saat diubah, fungsi tidak berubah.

