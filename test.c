#include "included.h"

struct _to {
    int x, y1, z;
    long long int y;
};

int main()
{
   printf("%d", sizeof(struct _to));
}