#include "ucode.c"

int in, out, err; 
char username[128], password[128];

int main(int argc, char * argv[])
{
    close(0);
    close(1);
    int in = open(argv[1], O_RDONLY);
    int out = open(argv[1], O_WRONLY);
    int err = open(argv[1], O_WRONLY);
    char line[256]; //256
    char token[6][30];
    int success=0;
    settty(argv[1]);
    while(1)
    {
        printf("login: ");
        gets(username);
        printf("password: ");
        gets(password);

        printf(username);
        close(0);
        //opens the [asswprd in 1]
        int passFile = open("/etc/passwd", O_RDONLY);
        while(getline(line))
        {
            //printf(line);
            //printf("line4:%c\n", line[4]);
            int index=0, tokindex=0; //index is index for each member in token and token index is how many have been entered so far
            for(int i=0; tokindex<=6 && line[i]!='\0'; i++){
                if(line[i]==':'){
                    token[tokindex][index]='\0';
                    //printf("token %d is %s\n", tokindex, token[tokindex]);
                    index=0;
                    tokindex++;
                }
                else{
                    token[tokindex][index]=line[i];
                    //printf("hello: %s\n", token[tokindex]);
                    index++;
                }
            }
            if(strcmp((char*)token[0],(char*)username)==0 && strcmp((char*)token[1],(char*)password)==0){
                //printf("yay\n");
                success=1;
                break;
            }
        }
        if(success==1){
            //printf("step3\n");
            chuid(atoi(token[2]), atoi(token[3]));
            chdir(token[5]);
            //printf(token[5]);//delete later
            close(0);
            int in = open(argv[1], O_RDONLY);
            printf("loging in successful\n");
            exec("sh");
        }
        else{
            printf("wrong login stupid\n");
        }
        close(0);
        close(1);
    }
}