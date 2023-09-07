// 引用头文件
#include <stdio.h>
#include <time.h>
// 宏定义
#define MAX 10000
int main()
{
    int i, n, f;
    scanf("%d", &n);
    i = 2;
    f = 1;
    clock_t start, end;
    double time;
    start = clock();
    // 死代码
    if (i < 1)
    {
        printf("i小于1\n");
    }
    while (i <= n)
    {
        f = f * i;
        i = i + 1;
    }
    if (f > MAX)
    {
        printf("输出超限\n");
    }
    else
    {
        printf("%d\n", f);
    }
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("程序运行时间为：%f\n", time);
    return 0;
}