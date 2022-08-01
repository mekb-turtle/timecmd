#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
int usage(char *argv0) {
	fprintf(stderr, "\
Usage: %s [hour:min:cmd]...\n\
	hour: 00-23, 24 hour time\n\
	min: 00-59\n\
	cmd: command to be ran by system(), which uses the default shell\n\
	-o --once : run only once and exit, instead of looping\n\
	-a --after : allow command to be ran anytime after instead of only on the minute\n\
", argv0);
	return 2;
}
char *cmds[24*60]; // list of all commands for each minute
size_t last_time;   // last one that was executed
bool done_once = 0;
bool is_one = 0;
struct tm *get_time() {
	struct timeval timev;
	gettimeofday(&timev, NULL);
	struct tm *time = localtime(&(timev.tv_sec));
	return time;
}
size_t get_last_time(bool after) {
	struct tm *current_tm = get_time();
	size_t last = current_tm->tm_hour * 60 + current_tm->tm_min; // get minute
	while (!cmds[last]) {
		if (!after) return SIZE_MAX;
		if (last == 0) last = 24*60; // loop back to yesterday
		--last; // go back a minute
	}
	return last;
}
void run_cmd(bool after) {
	size_t last = get_last_time(after);
	if ((!done_once || last != last_time) && last != SIZE_MAX) { // so we don't run the same command again for no reason
		done_once = 1;
		if (cmds[last]) {
			printf("Running %02li:%02li commands: %s\n", last/60, last%60, cmds[last]);
			system(cmds[last]);
		}	
	}
	last_time = last;
}
void sleep_(unsigned long ms) {
	struct timespec r = { ms/1e3, (ms%1000)*1e6 }; // 1 millisecond = 1e6 nanoseonds
	nanosleep(&r, NULL);
}
int main(int argc, char *argv[]) {
#define INVALID return usage(argv[0])
#define NUMERIC(x) (x>='0'&&x<='9')
	bool once_flag = 0;
	bool after_flag = 0;
	bool flag_done = 0;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-' && argv[i][1] != '\0' && !flag_done) {
			if (argv[i][1] == '-' && argv[i][2] == '\0') flag_done = 1; // -- denotes end of flags
			else {
				if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--once") == 0) {
					if (once_flag) INVALID;
					once_flag = 1;
				} else
				if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--after") == 0) {
					if (after_flag) INVALID;
					after_flag = 1;
				} else
				INVALID;
			}
		} else {
			if (!NUMERIC(argv[i][0])) INVALID; // check 1st char is a digit
			if (!NUMERIC(argv[i][1])) INVALID; // check 2nd char is a digit
			if (argv[i][2] != ':')    INVALID; // check 3rd char is a colon
			if (!NUMERIC(argv[i][3])) INVALID; // check 4th char is a digit
			if (!NUMERIC(argv[i][4])) INVALID; // check 5th char is a digit
			if (argv[i][5] != ':')    INVALID; // check 6th char is a colon
			if (argv[i][6] == '\0')   INVALID; // check there is text afterwards
			uint8_t hours   = (argv[i][0]-'0') * 10 + (argv[i][1]-'0'); // '0'-'0' == 0, '1'-'0' == 1, etc
			uint8_t minutes = (argv[i][3]-'0') * 10 + (argv[i][4]-'0');
			if (hours   >= 24) INVALID; // there cannot be more than 24 hours in a day
			if (minutes >= 60) INVALID; // there cannot be more than 60 minutes in an hour
			size_t j = hours * 60 + minutes;
			if (cmds[j]) INVALID; // check it's not a duplicate
			cmds[j] = argv[i] + 6; // command starts at 6th char
			is_one = 1;
		}
	}
	if (!is_one) INVALID; // check at least one exists
	run_cmd(after_flag);
	if (!once_flag) {
		while (1) {
			sleep_(5000);
			run_cmd(after_flag);
		}
	}
	return 0;
}
