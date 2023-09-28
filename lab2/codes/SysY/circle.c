#include <stdio.h>
const float pi = 3.14;
int main() {
    float D = 2.0;
    printf("perimeter: %.2f\n", pi * D);
    printf("area: %.2f\n", pi * D * D / 4.0);
    return 0;
}
