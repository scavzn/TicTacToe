
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
#define KEY_SEM1 170
#define KEY_SEM2 171
#define KEY_ARRAY 111
#define KEY_WARRAY 150
#define KEY_READY 112
#define KEY_EVENT 200

int shmturn, shmtabella, shmWinArr, shmevent, shmready, semturn2, semturn, shmevent, player;
int* turn;
int* ready;
char* tabella;
int* wtabella;
int* event;

void menu() {
  system("clear");
  printf("Benvenuto player 1");
  printf("\nCosa vorrebbe fare? (1-4)");
  printf("\n\n1) P1 vs CPU");
  printf("\n2) P1 vs P2");
  printf("\n3) Regole TRIS");
  printf("\n4) Terminare");
  printf("\nScelta: ");
  int cho;
  scanf("%d", &cho);
  switch(cho) {
    case 1:
      *event = 3;
      break;
    case 2:
      *event = 2;
      break;
    case 3:
      system("clear");
      printf("\nregole\n");
      getchar();
      getchar();
      system("clear");
      menu();
      break;
    case 4:
      *event = 69;
      exit(0);
      break;
    default:
      system("clear");
      printf("\nNon e' un numero fra 1-4!\n");
      getchar();
      getchar();
      system("clear");
      menu();
      break;

  }
}
void fun(int sig) {
    if (player == 0) {
      *event = 135;
    }
    else if (player == 1) {
      *event = 136;
    }
    exit(0);
}

int main() {
  int magicCube[9] = {8, 1, 6, 3, 5, 7, 4, 9, 2};//per win condition
	int choice;

	signal(2, fun);

	shmturn = shmget(KEY_TURN, sizeof(int), 0600);
  if (shmturn == -1) {
    printf("Eseguire server prima\n");
    exit(0);
  }
	shmtabella = shmget(KEY_ARRAY, sizeof(char) * 9, 0600);
  shmWinArr = shmget(KEY_WARRAY, sizeof(int) * 9, 0600);
	shmready = shmget(KEY_READY, sizeof(int), 0600);
  shmevent = shmget(KEY_EVENT, sizeof(int), 0600);

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

	struct sembuf still;
	down.sem_num = 1;
	down.sem_op = 0;
	down.sem_flg = 0;

	player = *ready;

  if (player >= 1 && !(*event == 2)) {
    printf("Not accessible yet.\n");
    exit(0);
  }
  *ready += 1;

  if (player == 0) {
    menu();
  }

  if(*event == 3){//contro cpu

  } else if (*event == 2){
    while (*ready != 2) {

    }
    printf("\n");
    while(1) {

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

      if (*event == 135) {
        printf("Vittoria a tavolino\n");
        exit(0);
      }
      else if (*event == 136) {
        printf("Vittoria a tavolino");
        printf("\nPremi invio per continuare...");
        getchar();
        getchar();
        *event = 41;
        sleep(1);
        system("clear");
        main();
      }

      sleep(1);

      if(*ready >= 3){
        break;
      }

  		switch(player) {

  			case 0:

  				switch(*turn) {
  					case 0:
              while(1){
                printf("insersci posizione (da 1 a 9): ");
                scanf("%d", &choice);
                if((choice <= 9) && (choice >= 1)){
                  if((tabella[choice - 1] == 'O') || (tabella[choice - 1] == 'X')){
                      continue;
                  }
                  break;
                }

              }
    					tabella[choice - 1] = 'X';
              wtabella[choice - 1] = magicCube[choice - 1] * 1;
    					*turn = *turn + 1;
    					break;

  					case 1:
    					while((*turn == 1) && (*event > 135)) {
    					}
    					break;

  				}
  				break;

  			case 1:

  				switch(*turn) {

  					case 0:
  					while( (*turn == 0) && (*event > 135)) {
  					}
  					break;

  					case 1:
              while(1){
                printf("insersci posizione (da 1 a 9): ");
                scanf("%d", &choice);
                if((choice <= 9) && (choice >= 1)){
                  if((tabella[choice - 1] == 'O') || (tabella[choice - 1] == 'X')){
                    continue;
                  }
                  break;
                }

              }
    					tabella[choice - 1] = 'O';
              wtabella[choice - 1] = magicCube[choice - 1] * 2;
    					*turn = *turn - 1;
    					break;
  				}
  				break;
  			}
        usleep(1000 * 200);
        system("clear");
  		}

  }

    if (player == 1) {
      exit(0);
    }
    switch(*ready) {
      case 3:
        printf("\nGiocatore [X] vince");
        break;
      case 4:
       printf("\nGiocatore [O] vince");
        break;
      case 5:
        printf("\nPareggio");
        break;
    }

    printf("\nPremi invio per continuare...");
    getchar();
    getchar();
    *event = 30;
    sleep(1);
    system("clear");
    main();
}
