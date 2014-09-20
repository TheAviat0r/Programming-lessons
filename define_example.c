#include <stdio.h>
#define PRAISE " Oh my god, what a beautiful name!"

int main()
{
   char name[50];

   printf("What is your name?\n");
   scanf("%s", &name);

   printf("Hello, %s . %s\n",name, PRAISE);
   return 0;
}

