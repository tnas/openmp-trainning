/*****************************************************
 * FILE: rock-paper-scissor.c
 * DESCRIPTION:
 * Este programa implementa o jogo "pedra-papel-tesoura"
 * onde os jogadores são threads. Como entrada tem-se
 * a quantidade de rodadas que serão jogadas. Como saida
 * o placar final do jogo e exibido. Apenas dois
 * jogadores sao permitidos.
 *
 * AUTHOR: Thiago Nascimento Rodrigues
 * LAST REVISED: 09/02/15
 ****************************************************/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define NUM_PLAYERS 2
#define NUM_OPTIONS 3

int main (int argc, char* argv[]) {

	int num_rounds, play_round[NUM_PLAYERS], score[NUM_PLAYERS + 1] = {0, 0, 0};
	register int round, i;
	enum option {rock, paper, scissor};
	int win_rule[3][2] = {rock, scissor, paper, rock, scissor, paper};

	num_rounds = atoi(argv[1]);

	// Randomic seed
	srand(time(NULL));

	#pragma omp parallel num_threads(NUM_PLAYERS) private(round)
	{
		int player_id = omp_get_thread_num();

		for (round = 1; round <= num_rounds; ++round) {

			#pragma omp barrier
			#pragma omp single
			printf("-------------- Match %d\n", round);

			int selected_option = rand() % NUM_OPTIONS;

			switch(selected_option) {
				case rock:
					printf("Player %d has chosen ROCK.\n", player_id);
					break;
				case paper:
					printf("Player %d has chosen PAPER.\n", player_id);
					break;
				case scissor:
					printf("Player %d has chosen SCISSOR.\n", player_id);
			}

			play_round[player_id] = selected_option;

			#pragma omp barrier
			#pragma omp master
			{
				if (play_round[0] == play_round[1]) {
					score[2]++;
					printf("The match has finished in draw.\n");
				}
				else {
					int player0_win = 0;

					for (i = 0; i < 3; ++i) {
						if (play_round[0] == win_rule[i][0] && play_round[1] == win_rule[i][1]) {
							printf("Player 0 win!\n");
							score[0]++;
							player0_win = 1;
							break;
						}
					}

					if (!player0_win) {
						printf("Player 1 win!\n");
						score[1]++;
					}
				}
			}

		}
	}

	printf("--------------------------------\n");
	printf("Final Score:\n");
	printf("Player 0: %d\n", score[0]);
	printf("Player 1: %d\n", score[1]);
	printf("Draws: %d\n", score[2]);
	printf("--------------------------------\n");
	return 0;
}

