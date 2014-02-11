#ifndef REACTION_TIME
#define REACTION_TIME

struct sample {
	int sleep_time;
	char key;
	int error_count;
	struct timeval start_time;
	struct timeval hit_time;
};

#ifndef USE_SAMPLES
static const int samples_taken = 10;
#else
static const int samples_taken = USE_SAMPLES;
#endif

int save_data(struct sample *);
int timeval_diff(struct timeval *, struct timeval *);

#endif
