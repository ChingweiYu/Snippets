#include <stdio.h>

int main( void )
{
    int     i;

    for ( i = 4; i >= 0; --i )
        printf( "%5s\n", "*****" + i );
    return 0;
}

