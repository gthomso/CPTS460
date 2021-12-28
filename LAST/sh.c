#include "ucode.c"

int IOredirect(char input[16][128]){
    printf("IOredirection detected\n");
}

int main()
{
    printf("You made it to the sh \n");
    //termStr for the " ";  tempName is testing for > or <
    char input[128], *tempStr, tokInput[16][128], *fname, cwd[256], *tempName; 
	while(1)
	{
        int pid, status, i=0;
        printf("sh & ");
        //int tempW=dup(1);
		gets(input);
        tempStr = input; //edit using the tempStr
        //tempName = input;
        //printf("input: %s\n", input);
        char * cmd = strtok(tempStr, tokInput[0], (char)32); //32 is the whitespace generated when executing
        printf("cmd: %s lenofcmd=%d\n", cmd, strlen(tokInput[0]));
        int strLen = strlen(tokInput[0]);
        //this is to separate and start at the area after the space
        tempStr+=strLen+1;
        printf("input still intact %s\n", input);
        //printf("tempStr[0] update %c\n",tempStr[0]);
        if (strcmp(input,"logout")==0){
            printf("logging pid: %d out\n", pid);
            exit(pid);
        }
        else if (strcmp(cmd,"cd")==0){
            // for some reason this isn't working not restoringproc
            strtok(tempStr, tokInput[1], (char)32);
            printf("cd %s\n", tokInput[1]);
            
            chdir(tokInput[1]);
            //continue; //maybe
        }
        int tempi=fork();
        if(tempi){
            printf("waiting for child to term in sh\n");
            pid = wait(&status);
        }
        else{
            //Looking for I/O redirection
            //int ior=0;
            
            /*for(i=1;*tempStr!=0;i++){
                strtok(tempStr,tokInput[i],(char)32);
                if (tokInput[i]=="," || tokInput[i]=="."){
                    ior=i;
                }
                int strLen = strlen(tokInput[0]);
                //this is to separate and start at the area after the space
                tempStr+=strLen+1;
            }
            if (ior!=0){
                //redirection has occured
                IOredirect(tokInput);
            }
            else{
                exec(input);
            }*/
            exec(input);
            exit(1);
            //printf("pid running %d\n", pid);
        }
	}
}