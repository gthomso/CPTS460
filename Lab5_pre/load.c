int load(char *filename, PROC *p){
    GD * gp;
    char buf[1024];
    int iblk;
    INODE * ip;
    DIR * dp;
    char * temp;
    int ino;
    printf("loading %s: ", filename);
    getblock(2,buf);
    gp = (GD *)buf;
    iblk = (u16)gp->bg_inode_table;
    getblock(iblk, buf);
    ip=(INODE *)buf+1;
    for (int j=0; j<12; j++){
        if(ip->i_block[j]){
            char tempBuf[1024];
            getblock(ip->i_block[j], tempBuf); //might need to be new char buf
            dp=(DIR *)tempBuf;
            temp=tempBuf;
            while(temp<&tempBuf[1024]){ //before end of whats been recorded
                printf("%s ",dp->name);
                dp->name[dp->name_len]=0;
                if(strcmp(dp->name,"bin")==0){
                     ino=(dp->inode)-1;
                     //printf("ino:%d\n",&ino);
                     break;
                }
                temp+=dp->rec_len;
                dp=(DIR *)temp;
            }
        }
        if(ino){
            break;
        }
    }
    //printf("step4\n");
    getblock((iblk + (ino/8)), buf); //mallmans alg to get the right ip
    //printf("step2\n");
    ip = (INODE *)buf + (ino % 8);
    //printf("step1\n");
    //This is for searching for filename in bin
    for (int j=0; j<12; j++){
        if(ip->i_block[j]){
            char tempBuf[1024];
            getblock(ip->i_block[j], tempBuf); //might need to be new char buf
            dp=(DIR *)tempBuf;
            temp=tempBuf;
            while(temp<&tempBuf[1024]){ //before end of whats been recorded
                printf("%s ",dp->name);
                dp->name[dp->name_len]=0;
                if(strcmp(dp->name,filename)==0){
                     ino=(dp->inode)-1;
                     //printf("ino:%d\n",&ino);
                     break;
                }
                temp+=dp->rec_len;
                dp=(DIR *)temp;
            }
        }
        if(ino){
            break;
        }
    }
    getblock((iblk + (ino/8)), buf); //mallmans algorithm to get to the right block
    ip = (INODE *)buf + (ino % 8);
    char *addr = (char *)(0x800000 + (p->pid - 1)*0x100000); //PA
    //end
    for (int i=0;i<12;i++){
        //printf("step3\n");
        if (ip->i_block[i]==0){
            break;
        }
        getblock(ip->i_block[i], addr);
        addr+=1024;
        printf("addr: %d\t", &addr);
    }
    printf("%s loaded\n", filename);
    return p->pid;
}

//found a online source that helped me with this function