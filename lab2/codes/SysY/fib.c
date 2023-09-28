#include <stdio.h>
const int n = 5;
int main() {
    int f[5] = {1, 1, 0, 0, 0};
    int i = 0;
    while (i < n) {
        if (i < 2) {
            i = i + 1;
        } 
        else {
            f[i] = f[i - 1] + f[i - 2];
            i = i + 1;
        }
    }
    printf("f[5] = %d\n", f[4]);
    return 0;
}
