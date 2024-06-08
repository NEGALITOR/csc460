// Bob Allen
// Playing with pointers
//
#include <stdio.h>

int main()
{
int num = 460;
int *numPtr;
int a[3];
int *b;
a[0] = 1;
a[1] = 2;
a[2] = 3;

printf("array a[0] is: %d\n",a[0]);
printf("array a[0] is: %d\n",*a);

b = a; 
printf("b[0] is: %d\n",b[0]);
printf("b points to: %d\n",*b);
b++;
printf("After b++, b points to: %d\n",*b);



numPtr = &num;

printf("num is: %d\t and numPtr is: %d\n",num,numPtr);
printf("num is: %d\t and numPtr points to: %d\n",num,*numPtr);

// change num but use numPtr to do it.
*numPtr = 321;

printf("num is: %d\t and numPtr is: %d\n",num,numPtr);
printf("num is: %d\t and numPtr points to: %d\n",num,*numPtr);

}








