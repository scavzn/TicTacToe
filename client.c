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

int shmturn, shmtabella, shmready, semturn2, semturn, choice;
int* turn;
int* ready;
char* tabella;


void fun(int sig) {
    *ready = *ready - 1;
    exit(0);	
}

int main() {

	int player, choice;

	signal(2, fun);

	shmturn = shmget(KEY_TURN, sizeof(int), 0600 | IPC_CREAT);
	shmtabella = shmget(KEY_ARRAY, sizeof(char) * 9, 0600 | IPC_CREAT);
	shmready = shmget(KEY_READY, sizeof(int), 0600 | IPC_CREAT);

	turn = (int*) shmat(shmturn, NULL, 0);
	ready = (int*) shmat(shmready, NULL, 0);
	char *tabella = (char*) shmat(shmtabella, NULL, 0);

	semturn = semget(KEY_TURN, 1 ,IPC_CREAT | IPC_EXCL | 0600);
	semturn2 = semget(KEY_ARRAY, 1 ,IPC_CREAT | IPC_EXCL | 0600);

	struct sembuf up;
	up.sem_num = 1;
	up.sem_op = 1; 
	up.sem_flg = 0;

	struct sembuf down;
	down.sem_num = 1;
	down.sem_op = -1;
	down.sem_flg = 0;

	struct sembuf still;
	down.sem_num = 1;
	down.sem_op = 0;
	down.sem_flg = 0;	

	player = *ready;
    *ready += 1;

	printf("Current players %d", *ready);
	fflush(stdout);
	while (*ready != 2) {

	}
	printf("\n");


	while(1) {
		printf("\e[1;1H\e[2J");	
		printf("You are p[%d], turn[%d]", player, *turn);
		for(int i = 0; i < 9; i++) {
			if (i % 3 == 0) {
				for (int j = 0; j < 2; j++) {
					printf("\n");
				}
			}

			printf("%c  ", tabella[i]);  //[0,1,2,3,5,6,7,8,9]
		}

		printf("\n");

		switch(player) {

			case 0:

				switch(*turn) {

					case 0:

					scanf(" %d",&choice);
					printf("\n");
					tabella[choice - 1] = 'X';
					*turn = *turn + 1;
					semop(semturn2, &up, 1);
					break;

					case 1:
					//while(*turn == 1) {
					//}
					semop(semturn, &down, 1);

					break;

				}

				break;

			case 1:

				switch(*turn) {

					case 0:
					//while(*turn == 0) {
					//}
					semop(semturn2, &down, 1);
					break;

					case 1:
					scanf(" %d",&choice);
					printf("\n");
					tabella[choice - 1] = 'O';

					*turn = *turn - 1;
					semop(semturn, &up, 1);
					break;

				}

				break;
			}
		}
}
