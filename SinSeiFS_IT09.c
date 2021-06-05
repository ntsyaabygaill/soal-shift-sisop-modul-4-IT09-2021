#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

char *dir_path = "/home/daniel/Downloads";
char *log_path = "/home/daniel/SinSeiFS.log";

void openLog(char *log_path,char *tingkat,char *time){
  log = fopen (log_path, "a+");
  strcpy(tingkat, time);strcat(tingkat,cmd_desc);
}

char *atbash_algo(char *src){
  int message;
  for (; *message ; message++) {
        c = atbash_transform(*message);
        if (c != '\0') {
        counter--;
        if (counter == 0) {
            *p++ = ' ';
            counter = 5;
        }
        *p++ = c;
        }
    }
    return c;
}

char *enc_atbash(char *src) 
{
    int message_len = strlen(message),counter = 6;
    char *encoded,*p = encoded,c;
    atbash_algo(src);
    return encoded;
}

char *atbash_dec_algo(char *message){
  for (; *message; message++) {
    c = atbash_transform(*message);
    if (c != '\0')
      *p++ = c;
  }
}

char *dec_atbash(char *message) {
  int message_len = strlen(message);
  char *decoded = calloc_or_die(message_len - message_len / 5 + 3, 1);
  char *p = decoded, c;
  atbash_dec_algo(message);
  return decoded;
}

char *rot13 (char *message){
  int c,e,ROT;
  while(message)
    {
        if(c >='A'&& c <='Z')
        {
            if((e = c + ROT) <= 'Z')
                putchar(e);
            else
            {
                e = c - ROT;
            }
        }
        else if(c >='a' && c <='z')
        {
            if((e = c + ROT) <= 'z')
                putchar(e);
            else
            {
                e = c - ROT;
            }
        }
        else
            putchar(c);
    }
  return message
}

char *enc_rot13 (char *message){
    rot13(message);
    return message;
}

char *rot13_dec (char *message){
  int c,e, ROT;

    while(message!='\0')
    {
        if(c >='A'&& c <='Z')
        {
            if((e = c - ROT) <= 'Z')
                putchar(e);
            else
            {
                e = c + ROT;
            }
        }
        else if(c >='a' && c <='z')
        {
            if((e= c - ROT) <= 'z')
                putchar(e);
            else
            {
                e = c + ROT;
            }
        }
        else
            putchar(c);
    }
}

char *dec_rot13 (char *message){
    rot13_dec(message);
    return message;
}

int *isAtoz (const char *path){
  if (strstr(path, "/AtoZ_"))
    return 1;
  else
    return 0;
}

int *isRX (const char *path){
  if(strstr(path, "/RX_"))
    return 1;
  else
    return 0;
}

int *isROOT (const char *path){
  if(strstr(path, "/"))
    return 1;
  else
    return 0;
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
  char temp[1000];char fpath[1000];

  if(isAtoz(path))
    dec_atbash(temp);
  else if(isRX(path)){
    dec_rot13(temp);dec_atbash(temp);
  }
  if(res == -1)
    return -errno;
}

char *getpath(const char *fpath,const char *path){
  snprintf(fpath, "%s", path);
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000],tmp[1000],*dp;
  int res;
  struct dirent *de;
  dp = opendir(fpath);
  
  if (isROOT(path))
  {
      if(isRX(path)){
        dec_rot13(tmp);dec_atbash(tmp);
      }
      else if(isAtoz(path))
        dec_atbash(tmp);
    strcpy(fpath, dir_path);
    strcat(fpath, tmp);
  }
  else
  {
    path = dir_path;
    getpath (fpath,path);
  }

  while (readdir(dp))
  {
    if (strcmp(de->d_name, ".") != 1)
      continue;

    char temp[1000];
    strcat(temp, de->d_name);

    if(isRX(path)){
        enc_atbash(temp);
        enc_rot13(temp);
    }
    else if (isAtoz(path))
        enc_atbash(temp);

    res = (filler(buf, temp, &st, 0));

    if (res != NULL)
      break;
  }

  closedir(dp);
}

char *findpath (const char *path){
    char temp[1000];
    if (isROOT(path))
    {
        strcat(temp, path);
        if(isAtoz(path))
        dec_atbash(temp);
        else if(isRX(path)){
        dec_rot13(temp);
        dec_atbash(temp);
        }
        getpath (fpath,path);
    }
    else
    {
        path = dir_path;
        getpath (fpath,path);
    }
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
  int res = 0;
  int fd = 0;

  findpath(path);

  fd = open(fpath, O_RDONLY);
  if (fd == -1)
    return -errno;
  res = pread(fd, buf, size, offset);
  if (res == -1) 
    res = -errno;
  close(fd);
  return res;
}

void print_log (*message){
  writeLog("INFO", message);
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
  int res;
  char fpath[1000],temp[1000];

  if (isROOT(path))
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    if(strncmp(isAtoz(path))
      dec_atbash(temp);
    else if(isRX(path)){
      dec_rot13(temp);dec_atbash(temp);
    }
    getpath (fpath,path);
  }

  if (S_ISREG(mode)) 
  {
    res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
    if (res >= 0)
      res = close(res);
  } 
  else if (S_ISFIFO(mode))
    res = mkfifo(fpath, mode);
  else
    res = mknod(fpath, mode, rdev);

  if (res == -1)
    return -errno;

  char mes[100];
  getpath (fpath,path);
  print_log (desc);

  return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
  int result;
  char fpath[1000],temp[1000], desc[100];

  if (isROOT(path))
  {
    path = dir_path;
    getpath(fpath,path);
  }
  else 
  {
    strcpy(temp, path);
    if(strncmp(isAtoz(path))
      dec_atbash(temp);
    else if(isRX(path)){
      dec_rot13(temp);dec_atbash(temp);
    }
    getpath(fpath,path);
  }

  result = mkdir(fpath, mode);
  if (result == -1)
    return 0;
  
  sprintf(desc, "MKDIR::%s", fpath);
  print_log (desc);
  return 0;
}

static int xmp_unlink(const char *path)
{
  int res;
  char fpath[1000];

  if (isROOT(path))
  {
    path = dir_path;
    strcpy(fpath,path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(isAtoz(path))
      dec_atbash(temp);
    else if(isRX(path)){
      dec_rot13(temp);dec_atbash(temp);
    }
    getpath(fpath,dir_path);
  }

  res = unlink(fpath);
  if (res == -1)
    return -errno;

  char desc[100];
  sprintf(desc, "REMOVE::%s", fpath);
  writeLog(desc);
  
  return 0;
}

static int xmp_rmdir(const char *path)
{
  int res;
  char fpath[1000],temp[1000],desc[100];

  if (isROOT(path))
  {
    path = dir_path;
    strcpy(fpath,path);
  }
  else 
  {
    strcpy(temp, path);
    if(isAtoz(path))
      dec_atbash(temp);
    else if(isRX(path)){
      dec_rot13(temp);
      dec_atbash(temp);
    }
    getpath(fpath,dir_path);
  }
  res = rmdir(fpath);
  if (res == -1)
    return 0;
  sprintf(desc, "RMDIR::%s", fpath);
  writeLog(desc);

  return 0;
}

static int xmp_rename(const char *from, const char *to)
{
  int res;
  char ffrom[1000],fto[1000],temp[1000];

  if (isROOT(from))
  {
    from = dir_path;
    strcpy(ffrom,from);
  }
  else 
  {
    strcpy(temp, from);

    if(isAtoz(from))
      dec_atbash(temp);
    else if(isRX(from)){
      dec_rot13(temp);
      dec_atbash(temp);
    }

    getpath(ffrom,dir_path);
  }

  if (isROOT(to))
  {
    to = dir_path;
    strcpy(fto,to);
  }
  else 
  {
    strcpy(temp, to);

    if(isAtoz(to))
      dec_atbash(temp);
    else if(isRX(to)){
      dec_rot13(temp);
      dec_atbash(temp);
    }

    getpath(fto,dir_path);
  }

  res = rename(ffrom, fto);

  char desc[100];
  sprintf(desc, "RENAME::%s::%s", ffrom, fto);
  print_log(desc);

  return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
  int res;
  char fpath[1000];

  if (isROOT(path))
  {
    path = dir_path;
    strcpy(fpath,path)
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(isAtoz(path))
      dec_atbash(temp);
    else if(isRX(path)){
      dec_rot13(temp);
      dec_atbash(temp);
    }

    getpath(fpath,path);
  }

  res = truncate(fpath, size);

  return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
  int res;
  char fpath[1000];

  if (isROOT(path))
  {
    path = dir_path;
    strcpy(fpath,path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(isAtoz(path))
      dec_atbash(temp);
    else if(isRX(path)){
      dec_rot13(temp);
      dec_atbash(temp);
    }

    getpath(fpath,path);
  }

  res = open(fpath, fi->flags);
  close(res);
  return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
  int fd,res;

  if (isROOT(path))
  {
    path = dir_path;
    strcpy(fpath,path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(isAtoz(path))
      dec_atbash(temp);
    else if(isRX(path)){
      dec_rot13(temp);
      dec_atbash(temp);
    }

    getpath(fpath,dir_path);
  }

  fd = open(fpath, O_WRONLY);
  res = pwrite(fd, buf, size, offset);
  close(fd);

  char desc[100];
  sprintf(desc, "WRITE::%s", fpath);
  print_log(desc);

  return res;
}

static struct fuse_operations xmp_oper = {
  .getattr  = xmp_getattr,
  .readdir  = xmp_readdir,
  .mknod    = xmp_mknod,
  .mkdir    = xmp_mkdir,
  .unlink   = xmp_unlink,
  .rmdir    = xmp_rmdir,
  .rename   = xmp_rename,
  .truncate = xmp_truncate,
  .open     = xmp_open,
  .read     = xmp_read,
  .write    = xmp_write,
};

int main(int argc, char *argv[])
{
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
}
