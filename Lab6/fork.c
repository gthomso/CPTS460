int fork()
{
	int i;
	char *PA, *CA;
	char *PA1, *CA1;
	char *PA2, *CA2;
	char *PA3, *CA3;
	PROC *p = getproc(); //might need to be adjusted
	if (p==0){
        printf("fork failed\n");return -1;
    }
	p->ppid = running->pid;
	p->parent = running;
	p->status = READY;
	p->priority = 1;
	p->sibling = running->child;
	running->child = p;
	uPtable(p);
	PA = (char*)((unsigned int)running->pgdir[2048] & 0xFFFF0000);
	CA = (char*)((unsigned int)p->pgdir[2048] & 0xFFFF0000);
	memcpy(CA, PA, 0x100000);printf("well");
	//printf("pgdir[2048]: running->pgdir %x, p->pgdir %x\n", running->pgdir[2048], p->pgdir[2048]);
	PA1 = (char*)((unsigned int)running->pgdir[2049] & 0xFFFF0000);
	CA1 = (char*)((unsigned int)p->pgdir[2049] & 0xFFFF0000);
	//printf("pgdir[2049]: running->pgdir %x, p->pgdir %x\n", running->pgdir[2049], p->pgdir[2049]);
	//geti();
	memcpy(CA1, PA1, 0x100000);geti();
	PA2 = (char*)((unsigned int)running->pgdir[2050] & 0xFFFF0000);
	CA2 = (char*)((unsigned int)p->pgdir[2050] & 0xFFFF0000);
	memcpy(CA2, PA2, 0x100000);
	PA3 = (char*)((unsigned int)running->pgdir[2051] & 0xFFFF0000);
	CA3 = (char*)((unsigned int)p->pgdir[2051] & 0xFFFF0000);
	memcpy(CA3, PA3, 0x100000);
	
	/*PA = (char*)((unsigned int)running->pgdir[2048] & 0xFFFF0000);
	CA = (char*)((unsigned int)p->pgdir[2048] & 0xFFFF0000);
	memcpy(CA, PA, 0x100000);printf("well");*/
	
	for(i = 1; i <= 14; i++) //14
		p->kstack[SSIZE - i] = running->kstack[SSIZE - i];
	p->kstack[SSIZE - 14] = 0;
	p->kstack[SSIZE - 15] = (int)goUmode;
	p->ksp = &(p->kstack[SSIZE - 28]);
	p->usp = running->usp;
	p->cpsr = running->cpsr;
	enqueue(&readyQueue, p);
	return p->pid;
}