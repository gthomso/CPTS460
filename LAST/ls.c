#include "ucode.c"

//from 299
//learn what these do later
#define S_ISDIR(mode) ((mode >> 12) == 0b0100)
#define S_ISREG(mode) ((mode >> 12) == 0b1000)
#define S_ISLNK(mode) ((mode >> 12) == 0b1100)

char buffer[1024];
char cwd[256];
char temp[256];
//learn ^^
char * t1 = "xwrxwrxwr-------";
char * t2 = "----------------";
STAT sp;

void ls_file(char * fname)
{
	int r;
    char sbuf[4096];
    stat(fname, &sp);
    //printf("sp.st_mode: %x", sp.st_mode);
	if(S_ISDIR(sp.st_mode))
        mputc('d');
	else if(S_ISREG(sp.st_mode))
		//printf("-");
        mputc('-');
	else
		mputc('l');
	for(int i = 8; i >= 0; i--){
        //printf("whyyyy\n");
        if(sp.st_mode & (1 << i)){
            //printf("%c",t1[i]);
            mputc(t1[i]);
        }
        else{
            //printf("%c",t2[i]);
            mputc(t2[i]);
        }
    }
	printf(" %d %d %d %d",(int)sp.st_nlink, (int)sp.st_gid, (int)sp.st_uid, (int)sp.st_size); //%4d
    //no time
	printf("%s\n",fname);
	if(S_ISLNK(sp.st_mode))
	{
		readlink(fname, sbuf);
        printf(" -> %s", sbuf);
	}
	printf("\n");
}

//got to here in the book

void ls_dir(char * dname) //ls a dir
{
    char name[256];
    DIR *dp;
    
	int dir = open(dname, O_RDONLY);
   
	read(dir, buffer, 1024);
	char * cp = buffer;
	dp = (DIR*) buffer;
	while(cp < 1024 + buffer)
	{
        //printf("lol");
		strncpy(temp, dp->name, dp->name_len);
		temp[dp->name_len] = '\0';
        //printf("ls of %s: ",temp);
		ls_file(temp);
		cp += dp->rec_len;
		dp = (DIR*)cp;	
	}
}

int main(int argc, char * argv[])
{
    int r;
    char *s;
    char filename[1024], cwd[1024];
    s=argv[1];
    if(argc==1){
        s="./";
    }
    r=stat(s, &sp);
    if(r<0){ //stat syscall
        exit(1);
    }
    strcpy(filename, s);
    //builds the full filename
    if(s[0]!='/'){
        getcwd(cwd);
        strcpy(filename,cwd);
        strcat(filename,"/");
        strcat(filename,s);
    }
	if(S_ISDIR(sp.st_mode))
		ls_dir(filename); //lists DIR
	else{
		ls_file(filename); //lists 
	}
    return 0;
}
