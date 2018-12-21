#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BBB b();

void a()
{

}

void b()
{

}

int main (int argc, char **argv)
{
	int n, i;
	FILE *fp;

	a();	
	BBB
	
	n = atoi(argv[1]);
	fp = fopen(argv[2], "w");
	
	fputs("break main\nrun\ninfo registers\n", fp);
	
	for (i=0; i<n; i++) {
		fputs("step\ninfo registers\n", fp);
	}
	
	fclose(fp);
	
	return 0;
}
