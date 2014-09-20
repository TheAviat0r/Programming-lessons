#include <stdio.h>
#define LIMIT 65
#define NAME "MEGATHINK, INC."
#define ADDRESS "10 Megabuck Plaza"
#define PLACE "Megapolis, CA 94904"
//==============================================
void starbar();
void space(int number);
int main( )
{
    starbar( );
    space(15);
    printf("%s\n", NAME);
    printf(" %s\n", ADDRESS);
    printf("%s", PLACE);
    space(15);
    printf("\n");
    starbar( );
    return 0;
}
/* далее следует функция starbar( ) */
//!=============================================
void starbar( )
{ int count;
for (count = 1; count <= LIMIT; count++)
putchar('*');
putchar('\n');
}
//!=============================================
void space(int number)
{
    for (int i=0; i <= number; ++i)
        putchar(' ');
}


