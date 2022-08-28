#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>

#define AUTHOR_SPECIFIED 1

typedef struct CarolInitialHeader {
  char magic[5];
  char version_str;
  unsigned short flags;
} CarolInitialHeader;

typedef struct CarolPackageData {
  char* pkgname;
  char* pkgauthor;
  char* pkgver;
} CarolPackageData;

typedef enum CarolEntryType {
  EXECUTABLE = 1,
  DIRECTORY,
  RESOURCE
} CarolEntryType;

typedef struct CarolEntry {
  uint8_t type;
  uint32_t size;
  char* name;
} CarolEntry;

char magic[5] = {'C','A','R','O','L'};

CarolPackageData* carol_init_package_data() {
  CarolPackageData* tmp = malloc(sizeof(CarolPackageData));
  return tmp;
}

void carol_destroy_package_data(CarolPackageData* data) {
  free(data->pkgname);
  free(data->pkgver);
  free(data->pkgauthor);
  free(data);
}

void carol_read_package_data(FILE* file, unsigned short flags, CarolPackageData* data) {
  unsigned short pkgnamesize = 0;
  unsigned short pkgauthorsize = 0;
  unsigned short pkgversize = 0;
  fread(&pkgnamesize, 2, 1, file);
  
  data->pkgname = malloc(pkgnamesize+1);
  fread(data->pkgname, 1, pkgnamesize, file);
  data->pkgname[pkgnamesize] = 0;

  if(flags & AUTHOR_SPECIFIED) {
  	fread(&pkgauthorsize, 2, 1, file);
  	data->pkgauthor = malloc(pkgauthorsize+1);
    data->pkgauthor[pkgauthorsize] = 0;
  	
    fread(data->pkgauthor, 1, pkgauthorsize, file);
  }
  
  fread(&pkgversize, 2, 1, file);
  data->pkgver = malloc(pkgversize+1);
  data->pkgver[pkgversize] = 0;
  
  fread(data->pkgver, 1, pkgversize, file);
}

CarolEntry* carol_read_entry(FILE* file) {
  CarolEntry* tmp = calloc(1, sizeof(CarolEntry));
  fread(&tmp->type, 1, 1, file);
  if(tmp->type==RESOURCE) {
    fread(&tmp->size, 4, 1, file);
  }
  if(tmp->type!=EXECUTABLE) {
    unsigned short fnl = 0;
    fread(&fnl, 2, 1, file);
    
    tmp->name = malloc(fnl+1);
    memset(tmp->name, 0, fnl+1);
    fread(tmp->name, 1, fnl, file);
  }
  if(tmp->type!=DIRECTORY && tmp->type!=RESOURCE) {
    fread(&tmp->size, 4, 1, file);
  }
  
  return tmp;
}

char* generate_tmp() {
  char sym[] = "HelloMyFriend_WannaCookie_";
  size_t sle = sizeof(sym)-1;
  
  char* final = malloc(16);
  memset(final, 0, 16);
  for(int i=0; i<15; i++) {
    final[i] = sym[rand()%sle];
    
  }
  return final;
}

void do_unpack_archive(FILE *fp, char* fname) {
  mkdir(fname, 0777);
  unsigned int oldpos = ftell(fp);
  fseek(fp, 0, SEEK_END);
  unsigned int filesize = ftell(fp);
  fseek(fp, oldpos, SEEK_SET);
  
  char* execfile = malloc((strlen(fname)*2)+6);
  strcpy(execfile, fname);
  strcat(execfile, "/");
  strcat(execfile, fname);
  
  while(ftell(fp)<filesize) {
    CarolEntry* entry = carol_read_entry(fp);
    
    if(entry->type==EXECUTABLE) {
      char* fdat = malloc(entry->size);
      fread(fdat, 1, entry->size, fp);
      
      FILE* fwr = fopen(execfile, "w");
      if(!fwr) {
        printf("Write: Error occured! (%s) <%s>\n", execfile, strerror(errno));
        exit(1);
      }
      fwrite(fdat, 1, entry->size, fwr);
      fclose(fwr);
      
      chmod(execfile, 0777);
      
      free(fdat);
    }else if(entry->type==DIRECTORY) {
      char* totfn = malloc(strlen(fname)+1+strlen(entry->name)+1);
      memset(totfn, 0, strlen(fname)+1+strlen(entry->name)+1);
      strcpy(totfn, fname);
      strcat(totfn, "/");
      strcat(totfn, entry->name);
      
      mkdir(totfn, 0777);
      
      free(totfn);
    }else if(entry->type==RESOURCE) {
      char* newfn = malloc(strlen(fname)+1+strlen(entry->name)+1);
      memset(newfn, 0, strlen(fname)+1+strlen(entry->name)+1);
      strcpy(newfn, fname);
      strcat(newfn, "/");
      strcat(newfn, entry->name);
      
      char* fdat = malloc(entry->size);
      fread(fdat, 1, entry->size, fp);
      
      FILE* fwx = fopen(newfn, "w");
      fwrite(fdat, 1, entry->size, fwx);
      fclose(fwx);
      
      free(fdat);
      free(newfn);
    }
    
    if(entry->name) {free(entry->name);}
    free(entry);
  }
  
  free(execfile);
}

void pretty_log(const char* fmt, ...) {
  va_list pkx;
  va_start(pkx, fmt);
  printf("[\033[34;1mLOG\033[0m] ");
  vprintf(fmt, pkx);
  va_end(pkx);
}

int remove_f(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
  int rv = remove(fpath);
  if (rv) perror(fpath);
  return rv;
}

int main(int argc, char** argv) {
  srand(time(0));
  char* tmpname = generate_tmp();
  
  printf("HyperPkg by NDRAEY (c) 2022\n\n");
  if(argc<=1) {
    printf("error: too many arguments\n");
    printf("Usage: carolhyper package\n");
    exit(1);
  }
  char* package = argv[argc-1];
  
  FILE* fp = fopen(package, "rb");
  if(!fp) {
    printf("Could not open file: %s (fopen() failed)\n", package);
    exit(1);
  }
  
  CarolInitialHeader* pkgflags = malloc(sizeof(CarolInitialHeader*));
  fread(pkgflags, sizeof(CarolInitialHeader), 1, fp);
  
  if(memcmp(pkgflags->magic, magic, 5)!=0) {
    printf("error: invalid file magic!\n");
    exit(1);
  }
  
  CarolPackageData* pkgdata = carol_init_package_data();
  carol_read_package_data(fp, pkgflags->flags, pkgdata);

  pretty_log("Running package %s (%s)\n", pkgdata->pkgname, pkgdata->pkgver);
  
  do_unpack_archive(fp, tmpname);
  chdir(tmpname);
  
  char* runnablepath = malloc(strlen(tmpname)+3);
  memset(runnablepath, 0, strlen(tmpname)+3);
  strcpy(runnablepath, "./");
  strcat(runnablepath, tmpname);
  
  int cpid = fork();
  
  if(cpid==0) {
    char *cargv[2] = {runnablepath, NULL};
    execv(tmpname, cargv);
  }
  
  waitpid(cpid, 0, 0);
  
  free(runnablepath);
  
  chdir("..");
    
  nftw(tmpname, remove_f, 256, FTW_DEPTH | FTW_PHYS);
  
  carol_destroy_package_data(pkgdata);
  fclose(fp);
  free(tmpname);
  return 0;
}
