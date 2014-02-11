#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <curses.h>
#include <sqlite3.h>

#include "reaction_time.h"

#ifdef USE_NEWLINE
#define NEW_LINE "\n"
#else
#define NEW_LINE
#endif

static void flushbuffer() {
	int c;

	nodelay(stdscr, TRUE);
	do {
		c = getch();
	} while (c == '\n' && !feof(stdin));
	nodelay(stdscr, FALSE);
}

static int collect_data(struct sample *sample) {
	char key = ERR;

	sample->sleep_time = rand() % 2000 + 1000; // ms
	sample->key = 49 + rand() % 7;

	usleep(1000 * sample->sleep_time);
	flushbuffer();
	printw("%c", sample->key);

	gettimeofday(&sample->start_time, NULL);
	key = getch();
	gettimeofday(&sample->hit_time, NULL);

	if (sample->key == key) {
		return 1;
	}

	sample->error_count++;

	return 0;
}

int timeval_diff(struct timeval *start, struct timeval *end) {
	return (end->tv_sec - start->tv_sec) * 1000 + (end->tv_usec - start->tv_usec) / 1000;
}

static void print_data(struct sample *samples) {
	int i;

	int mean = 0;

	printw("sample#,key,sleep_time(ms),reaction_time(ms),#errors\n");

	for (i = 0; i < samples_taken; i++) {
		int elapsed_time = timeval_diff(&samples[i].start_time, &samples[i].hit_time);
		printw("%i,%c,%i,%i,%i\n", i, samples[i].key, samples[i].sleep_time, elapsed_time, samples[i].error_count);
		mean += elapsed_time;
	}

	printw("mean reaction time = %f\n", mean / (float) samples_taken);
}

int main(int argc, char **argv) {
	int i = 0;
	struct sample samples[samples_taken];

	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, FALSE);

	srand(time(NULL));

	printw("Press a key to start\n");
	getch();

	for (i = 0; i < samples_taken; i++)
		samples[i].error_count = 0;

	i = 0;
	while (i < samples_taken) {
		if (collect_data(&samples[i]))
			i++;
	}

#ifndef USE_NEWLINE
	printw("\n");
#endif

	print_data(samples);

	if(!save_data(samples)) {
		printw("Failed to save data to database");
	}

	printw("Press any key to exit...");
	getch();

	endwin();

	return 0;
}