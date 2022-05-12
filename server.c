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

int shmturn, shmtabella, shmready, semturn2, semturn;
int* turn;
int* ready;

void lineclear() {
	printf("\33[2K\r");
}

void fun(int sig) {
	printf("\n");
    semctl(semturn, 1, IPC_RMID);
    shmctl(shmturn ,IPC_RMID, NULL);
    semctl(semturn2, 1, IPC_RMID);
    shmctl(shmtabella ,IPC_RMID, NULL);    
	shmctl(shmready ,IPC_RMID, NULL);    
    exit(0);
}

int main() {
	int animIndex = 0;
	char loadAnim[] = {'/', '-', '\\', '|'};
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

	*ready = 0;
	*turn = 0;
	
	for(int i = 0; i < 9; i++) {
		tabella[i] = 43; //ascii bullet
	}
	
	//write(1, "Waiting for players...", strlen("Waiting for players..."));

	//printf("Waiting for players...");
	//fflush(stdout);
	while (*ready != 2) {
		lineclear();
		//printf("\33[2K\r"); //VT100 escape codes
		usleep(1000 * 200);
		printf("[%c] Waiting for players", loadAnim[animIndex]);
		animIndex++;
		if (animIndex > 3) {
			animIndex = 0;
		}
		fflush(stdout);
	}
	system("clear");

	while(1) {	
		lineclear();
		//printf("\033[0m");
		usleep(1000 * 200);
		printf("Turn Player[%d]", *turn);
		fflush(stdout);
	}
	return 0;
}

