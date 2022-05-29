/* 
Author(s): Ambrosi Marco, Ouassou Mehdi
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define DIM 1500
#define TEMPO 15
#define KEY_TURN 115
#define KEY_SEM1 170
#define KEY_SEM2 171
#define KEY_ARRAY 111
#define KEY_WARRAY 150
#define KEY_READY 112
#define KEY_EVENT 200

//event(136) è vittoria forfeit per il giocatore 1
//event(138) è vittoria per tempo scaduto per giocatore 2
//event(139) è vittoria per tempo scaduto per giocatore 1
//event(69) invia segnale al server di terminare
//event(1-2-3-4) gestisce modalità di gioco

struct sembuf up;
struct sembuf down;
int shmturn, shmtabella, shmWinArr, shmevent, shmready, semturn2, semturn, shmevent, player;
int* turn;
int* ready;
char* tabella;
int* wtabella;
int* event;

///////////
void printLogo() {
  char str[DIM];
  int id = open("art2.txt", 0644, O_RDONLY);
  int n = read(id, str, DIM);
  int index = 0;
  str[n] = 0;
  printf("\033[1;31m");//scrivo in rosso
  while (str[index] != 0)
  {
      printf ("%c", str[index]);
      index++;
  }
  printf("\033[0m");//reset del colore
}
/////////

void alarme(int sig){
  if (player == 0) {
    semop(semturn, &down, 1);
    *event = 138;
    semop(semturn, &up, 1);
  }
  else if (player == 1) {
    semop(semturn, &down, 1);
    *event = 139;
    semop(semturn, &up, 1);
    exit(0);
  }
}

void draw (char s) {
  if(s == 'X'){
    printf("\033[1;31m");
    printf(" X ");
    printf("\033[0m");
  }else if(s == 'O'){
    printf("\033[1;37m");
    printf(" O ");
    printf("\033[0m");
  }
  else{
    printf(" %c ", s);
  }
}

void grid(){
  int index = 0;
  printf("   |   |\n");
  for(int i = 0; i < 2; i++){
    draw(tabella[index]);
    printf("|");
    index++;
    draw(tabella[index]);
    printf("|");
    index++;
    draw(tabella[index]);
    index++;

    printf("\n___|___|___\n");
    printf("   |   |\n");
  }
  draw(tabella[6]);
  printf("|");
  draw(tabella[7]);
  printf("|");
  draw(tabella[8]);

  printf("\n   |   |\n");
}

void menu() {
  system("clear");
  printLogo();
  printf("\nChoose 1-4");
  printf("\n\n1) P1 vs CPU");
  printf("\n2) P1 vs P2");
  printf("\n3) Rules");
  printf("\n4) Exit");
  printf("\nChoice: ");
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
      printf("\n-The game is played on a grid 3x3 grid.\n-You are X, the opponent is O.\n-Players take turns in inserting their respective mark in the empty squares.\n-The first player to get 3 of their marks in a row (vertically, horizontally or diagonally) wins.\n-When all 9 squares are full, the game ends in a tie.\n-Insert your mark by typing a number from 1 to 9.\n\n\nPRESS ENTER TO RETURN TO THE MENU...\n");
      getchar();
      getchar();
      system("clear");
      menu();
      break;
    case 4:
      semop(semturn, &down, 1);
      *event = 69;
      semop(semturn, &up, 1);
      exit(0);
      break;
    default:
      system("clear");
      printf("\nNOT A NUMBER BETWEEN 1-4!\n");
      getchar();
      getchar();
      system("clear");
      menu();
      break;

  }
}

void ctrlC(int sig) {
    if (player == 0) {
      printf("\nPlayer 'O' wins\n");
      semop(semturn, &down, 1);
      *event = 135;
      semop(semturn, &up, 1);
    }
    else if (player == 1) {
      semop(semturn, &down, 1);
      *event = 136;
      semop(semturn, &up, 1);
    }
    exit(0);
}

int main() {
  signal(SIGALRM, alarme);
  int animIndex = 0;
  char loadAnim[] = {'/', '-', '\\', '|'};
  int magicCube[9] = {8, 1, 6, 3, 5, 7, 4, 9, 2};//per win condition
	int choice;

	signal(2, ctrlC);

	shmturn = shmget(KEY_TURN, sizeof(int), 0600);
  if (shmturn == -1) {
    printf("Server needed first\n");
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
	//semturn2 = semget(KEY_SEM2, 1 , 0600);

	struct sembuf up;
	up.sem_num = 1;
	up.sem_op = 1;
	up.sem_flg = 0;

	struct sembuf down;
	down.sem_num = 1;
	down.sem_op = -1;
	down.sem_flg = 0;

	player = *ready;

  if (player >= 1 && !(*event == 2)) {
    printf("Not accessible yet.\n");
    exit(0);
  }
  semop(semturn, &down, 1);
  *ready += 1;
  semop(semturn, &up, 1);

  if (player == 0) {
    menu();
  }

  if(*event == 3){//contro cpu
    printf("\n");
        while(1) {
      	    printf("You're are P[%d], P[%d] turn\n", player, *turn);
            grid();

      	   printf("\n");

           if (*event == 138) {
               system("clear");
               printf("Lose by timeout\n");
               printf("Player 'O' wins");
               printf("\nPress ENTER to continue...");
               getchar();
               getchar();
               semop(semturn, &down, 1);
               *event = 41;
               semop(semturn, &up, 1);
               sleep(1);
               system("clear");
               main();
               //char loadAnim[] = {'/', '-', '\\', '|'};
             }
           sleep(1);

           if(*ready >= 3){
             break;
           }
      		 switch(*turn) {
      			 case 0:
               while(1){
                 printf("\nInsert position (1-9): ");
                 alarm(TEMPO);
                 scanf("%d", &choice);
                 alarm(0);
                 if((choice <= 9) && (choice >= 1)){
                   if((tabella[choice - 1] == 'O') || (tabella[choice - 1] == 'X')){
                       continue;
                   }
                   break;
                 }
               }
               semop(semturn, &down, 1);
      				 tabella[choice - 1] = 'X';
               wtabella[choice - 1] = magicCube[choice - 1] * 1;
      				 *turn = *turn + 1;
               semop(semturn, &up, 1);
      				 break;

      			case 1://turno bot
              while(1){
                srand(time(0));
                choice = (rand() % 9) + 1;
                if((choice <= 9) && (choice >= 1)){
                  if((tabella[choice - 1] == 'O') || (tabella[choice - 1] == 'X')){
                      continue;
                  }
                  break;
                }

              }
              semop(semturn, &down, 1);
              tabella[choice - 1] = 'O';
              wtabella[choice - 1] = magicCube[choice - 1] * 2;
              *turn = *turn - 1;
              semop(semturn, &up, 1);
              break;
            }
            usleep(1000 * 200);
            system("clear");
         }
  } else if (*event == 2){
    system("x-terminal-emulator -e ./client");//apro terminale del giocatore 2 in automatico
    semop(semturn, &down, 1);
    *event = 91;
    semop(semturn, &up, 1);
    printf("\n");
    while(1) {
          printf("You're are P[%d], P[%d] turn\n", player, *turn);
          grid();

         printf("\n");

         sleep(1);

          if (*event == 135) {//player 2 vince
            printf("Win by forfeit\n");
            exit(0);
          } else if (*event == 136) {
            printf("Win by forfeit");
            printf("\nPress ENTER to continue...");
            getchar();
            getchar();
            semop(semturn, &down, 1);
            *event = 41;
            semop(semturn, &up, 1);
            sleep(1);
            system("clear");
            main();
            //char loadAnim[] = {'/', '-', '\\', '|'};
          }
          else if (*event == 138) {
            if (player == 0) {
              system("clear");
              printf("Lose by timeout\n");
              printf("Player 'O' wins");
              printf("\nPress ENTER to continue...");
              getchar();
              getchar();
              semop(semturn, &down, 1);
              *event = 41;
              semop(semturn, &up, 1);
              sleep(1);
              system("clear");
              main();
              //char loadAnim[] = {'/', '-', '\\', '|'};
            }
          else {
            exit(0);
          }
        }
        else if (*event == 139) {
          if (player == 0) {
            system("clear");
            printf("Win by timeout\n");
            printf("Player 'X' wins");
            printf("\nPress ENTER to continue...");
            getchar();
            getchar();
            semop(semturn, &down, 1);
            *event = 41;
            semop(semturn, &up, 1);
            sleep(1);
            system("clear");
            main();
            //char loadAnim[] = {'/', '-', '\\', '|'};
          }
        else {
          exit(0);
        }
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
                    printf("\nInsert position (1-9): ");
                    alarm(TEMPO);
                    scanf("%d", &choice);
                    alarm(0);
                    if((choice <= 9) && (choice >= 1)){
                      if((tabella[choice - 1] == 'O') || (tabella[choice - 1] == 'X')){
                          continue;
                      }
                      break;
                    }

                  }
                  semop(semturn, &down, 1);
        					tabella[choice - 1] = 'X';
                  wtabella[choice - 1] = magicCube[choice - 1] * 1;
        					*turn = *turn + 1;
                  semop(semturn, &up, 1);
        					break;

      					case 1:
                  printf("Wait for the opponent's choice...");
                  fflush(stdout);
        					while((*turn == 1) && (*event < 135)) {}
        					break;

      				}
      				break;

      			case 1:

      				switch(*turn) {

      					case 0:
                  printf("Wait for the opponent's choice...");
                  fflush(stdout);
        					while( (*turn == 0) && (*event < 134)) {}
        					break;

      					case 1:
                  while(1){
                    printf("\nInsert position (1-9): ");
                    alarm(TEMPO);
                    scanf("%d", &choice);
                    alarm(0);
                    if((choice <= 9) && (choice >= 1)){
                      if((tabella[choice - 1] == 'O') || (tabella[choice - 1] == 'X')){
                        continue;
                      }
                      break;
                    }

                  }
                  semop(semturn, &down, 1);
        					tabella[choice - 1] = 'O';
                  wtabella[choice - 1] = magicCube[choice - 1] * 2;
        					*turn = *turn - 1;
                  semop(semturn, &up, 1);
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
        printf("\nPlayer [X] wins");
        break;
      case 4:
       printf("\nPlayer [O] wins");
        break;
      case 5:
        printf("\nDraw");
        break;
    }

    printf("\nPress ENTER to continue...");
    getchar();
    getchar();
    semop(semturn, &down, 1);
    *event = 30;
    semop(semturn, &up, 1);
    sleep(1);
    system("clear");
    main();
    return 0;
}
