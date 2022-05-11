#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <signal.h>

#define KEY_TURN 115
#define KEY_ARRAY 111
#define KEY_READY 112

int shmturn, shmtabella, shmready, semturn, semtabella;
int* turn;
int* ready;

void fun(int sig) {
    *ready = *ready - 1;
    exit(0);	
}

int main() {

	int choice;
	char sign;
	signal(2, fun);

	shmturn = shmget(KEY_TURN, sizeof(int), 0600 | IPC_CREAT);
	shmtabella = shmget(KEY_ARRAY, sizeof(char) * 9, 0600 | IPC_CREAT);
	shmready = shmget(KEY_READY, sizeof(int), 0600 | IPC_CREAT);

	turn = (int*) shmat(shmturn, NULL, 0);
	ready = (int*) shmat(shmready, NULL, 0);
	char *tabella = (char*) shmat(shmtabella, NULL, 0);

	semturn = semget(KEY_TURN, 1 ,IPC_CREAT | IPC_EXCL | 0600);
	semtabella = semget(KEY_ARRAY, 1 ,IPC_CREAT | IPC_EXCL | 0600);

	struct sembuf up;
	up.sem_num = 1;
	up.sem_op = 1; 
	up.sem_flg = 0;

	struct sembuf down;
	down.sem_num = 1;
	down.sem_op = -1;
	down.sem_flg = 0;
	
    *ready += 1;

	printf("Current players %d", *ready);
	fflush(stdout);

	while (*ready != 2) {

	}


	while(1) {
		
	for(int i = 0; i < 9; i++) {
		if (i % 3 == 0) {
			for (int j = 0; j < 2; j++) {
				printf("\n");
			}
		}
		printf("%c  ", tabella[i]);  //[0,1,2,3,5,6,7,8,9]
	}

		scanf("%d",&choice);
		scanf("%c",&sign);
		tabella[choice - 1] = sign;
	}
}

