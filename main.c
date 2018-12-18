/*
 * Utility to print running total of VmPeak and VmSize of a program
 * https://locklessinc.com/articles/memory_usage/
 */
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PATH_MAX 2048

static int usage(char *me)
{
	fprintf(stderr, "usage: %s pid\n", me);
	fprintf(stderr, "Run program, and print VmPeak, VmSize, VmRSS and VmHWM (in KiB) to stderr\n");

	return 0;
}

static int main_loop(char *pidstatus)
{
	time_t rawtime;
	struct tm *timeinfo;

	char *line;
	char *vmsize;
	char *vmpeak;
	char *vmrss;
	char *vmhwm;
	
	size_t len;
	
	FILE *f;

	vmsize = NULL;
	vmpeak = NULL;
	vmrss = NULL;
	vmhwm = NULL;
	line = malloc(128);
	len = 128;
	
	f = fopen(pidstatus, "r");
	if (!f)
		return 1;
	
	/* Read memory size data from /proc/pid/status */
	while (!vmsize || !vmpeak || !vmrss || !vmhwm) {
		if (getline(&line, &len, f) == -1) {
			/* Some of the information isn't there, die */
			return 1;
		}
		
		/* Find VmPeak */
		if (!strncmp(line, "VmPeak:", 7)) {
			vmpeak = strdup(&line[7]);
		}
		
		/* Find VmSize */
		else if (!strncmp(line, "VmSize:", 7)) {
			vmsize = strdup(&line[7]);
		}
		
		/* Find VmRSS */
		else if (!strncmp(line, "VmRSS:", 6)) {
			vmrss = strdup(&line[7]);
		}
		
		/* Find VmHWM */
		else if (!strncmp(line, "VmHWM:", 6)) {
			vmhwm = strdup(&line[7]);
		}
	}
	free(line);
	
	fclose(f);

	/* Get rid of " kB\n"*/
	len = strlen(vmsize);
	vmsize[len - 4] = 0;
	len = strlen(vmpeak);
	vmpeak[len - 4] = 0;
	len = strlen(vmrss);
	vmrss[len - 4] = 0;
	len = strlen(vmhwm);
	vmhwm[len - 4] = 0;
	
	/* Output results to stderr */
	time(&rawtime);
	timeinfo = localtime (&rawtime);

	fprintf(stderr, "[%.24s] %s\t%s\t%s\t%s\n",
		asctime(timeinfo), vmsize, vmpeak, vmrss, vmhwm);
	
	free(vmpeak);
	free(vmsize);
	free(vmrss);
	free(vmhwm);

	/* Success */
	return 0;
}

int main(int argc, char **argv)
{
	char buf[PATH_MAX] = { 0 };
	pid_t pid = 0;
	
	if (argc < 2)
		return usage(argv[0]);

	pid = atoi(argv[1]);

	snprintf(buf, PATH_MAX, "/proc/%d/status", pid);
	
	/* Continual scan of proc */
	while (!main_loop(buf)) {
		/* Wait for 1 sec */
		usleep(1000000);
		sleep(1);
	}
	
	return 1;
}
