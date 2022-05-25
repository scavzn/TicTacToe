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
#define KEY_SEM1 170
#define KEY_SEM2 171
#define KEY_WARRAY 150
#define KEY_EVENT 200

int shmturn, shmtabella, shmWinArr, shmready, semturn2, semturn, shmevent;
int* turn;
int* ready;
char* tabella;
int* wtabella;
int* event;

///////////
void printLogo() {
  FILE *fptr;

  fptr = fopen("art.txt", "r");

  char c = fgetc(fptr);
  while (c != EOF)
  {
      printf ("%c", c);
      c = fgetc(fptr);
  }

  fclose(fptr);
}
/////////

int checkWin(){
  //win condition
  if(wtabella[0] + wtabella[1] + wtabella[2] == 15 || wtabella[3] + wtabella[4] + wtabella[5] == 15 || wtabella[6] + wtabella[7] + wtabella[8] == 15 || wtabella[0] + wtabella[3] + wtabella[6] == 15 || wtabella[1] + wtabella[4] + wtabella[7] == 15 || wtabella[2] + wtabella[5] + wtabella[8] == 15 || wtabella[0] + wtabella[4] + wtabella[8] == 15 || wtabella[2] + wtabella[4] + wtabella[6] == 15){
		return 3;
  }else if(wtabella[0] + wtabella[1] + wtabella[2] == 30 || wtabella[3] + wtabella[4] + wtabella[5] == 30 || wtabella[6] + wtabella[7] + wtabella[8] == 30 || wtabella[0] + wtabella[3] + wtabella[6] == 30 || wtabella[1] + wtabella[4] + wtabella[7] == 30 || wtabella[2] + wtabella[5] + wtabella[8] == 30 || wtabella[0] + wtabella[4] + wtabella[8] == 30 || wtabella[2] + wtabella[4] + wtabella[6] == 30){
		return 4;
  }  //draw condition
  int sum = 0;
  for(int j = 0; j < 9; j++){
      if(tabella[j] == 'X' || tabella[j] == 'O'){
        sum++;
      }
  }
  if(sum == 9){
    return 5;
  }
	return 0;
}

void terminate() {
		printf("\n");
		semctl(semturn, 1, IPC_RMID);
		shmctl(shmturn ,IPC_RMID, NULL);
		semctl(semturn2, 1, IPC_RMID);
		shmctl(shmtabella ,IPC_RMID, NULL);
		shmctl(shmready ,IPC_RMID, NULL);
		shmctl(shmWinArr ,IPC_RMID, NULL);
		shmctl(shmevent ,IPC_RMID, NULL);
		exit(0);
}

void lineclear() {
	printf("\33[2K\r");
}

void fun(int sig) {
	terminate();
}

int main() {
	int animIndex = 0;
	char loadAnim[] = {'/', '-', '\\', '|'};
	signal(2, fun);

	shmturn = shmget(KEY_TURN, sizeof(int), 0600 | IPC_CREAT);
	shmWinArr = shmget(KEY_WARRAY, sizeof(int) * 9, 0600 | IPC_CREAT);
	shmtabella = shmget(KEY_ARRAY, sizeof(char) * 9, 0600 | IPC_CREAT);
	shmready = shmget(KEY_READY, sizeof(int), 0600 | IPC_CREAT);
	shmevent = shmget(KEY_EVENT, sizeof(int), 0600 | IPC_CREAT);

	turn = (int*) shmat(shmturn, NULL, 0);
	ready = (int*) shmat(shmready, NULL, 0);
	tabella = (char*) shmat(shmtabella, NULL, 0);
	wtabella = (int*) shmat(shmWinArr, NULL, 0);
	event = (int*) shmat(shmevent, NULL, 0);

	semturn = semget(KEY_SEM1, 1 , 0600);
	semturn2 = semget(KEY_SEM2, 1 , 0600);

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
	*event = -1;

	for(int i = 0; i < 9; i++) {
		tabella[i] = i + 49; //ascii bullet 43
		wtabella[i] = 0;
	}

	//write(1, "Waiting for players...", strlen("Waiting for players..."));

	//printf("Waiting for players...");
	//fflush(stdout);

	while (*event == -1) {
		printLogo();
		printf("Creato da M. Ambrosi e M. Ouassou\n");
		usleep(1000 * 200);
		system("clear");
	}
	switch(*event) {
		case 69:
			terminate();
		case 135:
			terminate();
		default:
			break;
	}

	while (*ready != 2) {
		//lineclear();
		//printf("\33[2K\r"); //VT100 escape codes
		printLogo();
		printf("[%c] Waiting for players", loadAnim[animIndex]);
		animIndex++;
		if (animIndex > 3) {
			animIndex = 0;
		}
		fflush(stdout);
		usleep(1000 * 200);
		system("clear");
		if (*event == 135) {
			terminate();
		}
	}

	while(checkWin() <= 0) {
		//lineclear();
		if (*event == 135) {
			sleep(2);
			terminate();
		}
		if (*event == 136) {
			while (*event != 41) {
			}
			main();
		}
		printLogo();
		printf("\nTurn Player[%d]", *turn);
		fflush(stdout);
		usleep(1000 * 200);
		system("clear");
	}
	*ready = checkWin();
	switch(*ready) {
		case 3:
			printf("\nplayer X win");
			break;
		case 4:
		 printf("\nplayer O win");
			break;
		case 5:
			printf("\nDraw");
			break;
	}
	while(!(*event == 30)) {
	}
	main();
}
