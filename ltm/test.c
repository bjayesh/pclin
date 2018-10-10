#include <stdio.h>
#include <tommath.h>
 
int main(void) {
    mp_int a,b,c;
if ((mp_init(&a)) != MP_OKAY) {
printf("Error initializing the number. ");
//mp_error_to_string(result));
return EXIT_FAILURE;    
}
    mp_set(&a, 5);
//    printf("value is %d", &a.dp);
    return 0;
}
