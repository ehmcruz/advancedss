#include <stdio.h>
#include <string.h>

static char buf[50];
static char cmd[50];

int main(int argc, char **argv)
{
	char *b, *c;
	int pid;
	
	do {
		printf("root@localhost: ");
		gets(buf);
	
		c = cmd;	
		for (b=buf; *b && *b != ' '; b++) {
			*c = *b;
			c++;
		}
		*c = 0;
		
		if (!strcmp(cmd, "run")) {
			printf("cmd run\n");
			pid = fork();
			if (pid == -1) {
				printf("error loading new process!\n");
			}
			else if (pid == 0) { // child
				printf("I am the child\n");
				while(1);
			}
			else { // parent
				printf("I am the parent, childs pid is %u\n", pid);
				while(1);
			}
		}
		else {
			printf("unknown command %s\n",  cmd);
		}
	} while(1);
	return 0;
}
