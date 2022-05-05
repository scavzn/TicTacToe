#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <signal.h>

#define KEY_PID 110
#define KEY_ARRAY 111

int shmpid, shmtabella, sempid, semtabella;
short *pid;

void fun(int sig) {
    semctl(sempid, 1, IPC_RMID);
    shmctl(shmpid ,IPC_RMID, NULL);
    semctl(semtabella, 1, IPC_RMID);
    shmctl(shmtabella ,IPC_RMID, NULL);    
    exit(0);	
}

int main() {

	signal(2, fun);

	shmpid = shmget(KEY_PID, sizeof(short), 0600 | IPC_CREAT);
	shmtabella = shmget(KEY_ARRAY, sizeof(int) * 9, 0600 | IPC_CREAT);

	short *pid = (short*) shmat(shmpid, NULL, 0);
	int *tabella = (int*) shmat(shmpid, NULL, 0);

	sempid = semget(KEY_PID, 1 ,IPC_CREAT | IPC_EXCL | 0600);
	semtabella = semget(KEY_ARRAY, 1 ,IPC_CREAT | IPC_EXCL | 0600);


	struct sembuf up;
	up.sem_num = 1;
	up.sem_op = 1;
	up.sem_flg = 0;

	struct sembuf down;
	down.sem_num = 1;
	down.sem_op = -1;
	down.sem_flg = 0;

	while (1) {
		//sleep(1);
		//semop(sem, &down, 0); 
		//printf("%d", *i); 
		//semop(sem, &up, 0);
	}
}

