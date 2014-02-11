#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/stat.h>

#include <sqlite3.h>

#include <curses.h>

#include "reaction_time.h"

#define DOT_LOCAL ".local"
#define DB_PATH DOT_LOCAL "/reaction_time/"
#define DB_NAME DB_PATH "data.sqlite3"

#ifndef HOME
#define HOME "HOME"
#endif

static int has_table_callback(void *data, int argc, char **argv, char **colNames) {
	if (argc == 1) {
		*((int *) data) = 1;
	}

	return 0;
}

static int nop_callback(void *data, int argc, char **argv, char **colNames) {
	return 0;
}

int create_table(sqlite3 *db) {
	const char *create_data = 
		"CREATE TABLE data (" \
		"time TIMESTAMP,"     \
		"delay_time INT,"     \
		"reaction_time INT,"  \
		"key CHAR,"           \
		"errors INT);";
	char *errMsg;
	int yadda = 0, rc;

	printw("Creating table data\n");
	rc = sqlite3_exec(db, create_data, nop_callback, (void *) &yadda, &errMsg);

	if (rc) {
		printw("Failed to create table (%s)\n", errMsg);
		return 0;
	}

	return 1;
}

int check_table(sqlite3 *db) {
	const char *check_table = "SELECT name FROM sqlite_master WHERE type='table' AND name='data'";
	char *errMsg;
	int has_table = 0, rc;

	rc = sqlite3_exec(db, check_table, has_table_callback, (void *)&has_table, &errMsg);
	if (rc) {
		printw("%s\n", errMsg);
		return 0;
	}

	return has_table;
}

sqlite3 *opendb() {
	char path[1024];
	sqlite3 *db;
	int rc;

	sprintf(path, "%s/" DOT_LOCAL, getenv(HOME));
	mkdir(path, 0750);

	sprintf(path, "%s/" DB_PATH, getenv(HOME));
	mkdir(path, 0750);


	sprintf(path, "%s/" DB_NAME, getenv(HOME));
	rc = sqlite3_open(path, &db);

	if (rc) {
		printw("Failed to open database (%s)\n", sqlite3_errmsg(db));
		return NULL;
	}

	printw("Using database %s\n", path);

	if (!check_table(db)) {
		create_table(db);
	}

	return db;
}

int close_db(sqlite3 *db) {
	sqlite3_close(db);

	return 0;
}

int save_data(struct sample *samples) {
	char query[16 * 1024], timestamp[64], *errMsg;
	int i, rc;
	struct tm *lt;

	sqlite3 *db = opendb();

	time_t ticks = time(NULL);
	lt = localtime(&ticks);
	strftime(timestamp, 64, "%F %H:%M", lt);
	printw("Saving data at timestamp %s\n", timestamp);

	if (db)
		for (i = 0; i < samples_taken; i++) {
			struct sample *sample = &samples[i];
			int reaction_time = timeval_diff(&sample->start_time, &sample->hit_time);
			
			sprintf(query, "INSERT INTO data (time, delay_time, reaction_time, key, errors) VALUES ('%s', %i, %i, '%c', %i);",
				timestamp, sample->sleep_time, reaction_time, sample->key, sample->error_count);

			rc = sqlite3_exec(db, query, nop_callback, 0, &errMsg);
			if (rc) {
				printw("%s\n", errMsg);
				break;
			}
		}

	close_db(db);
	return 1;
}
