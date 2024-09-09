#include <stdio.h>
#include <stdlib.h>

int main() {
    int result;

    printf("Running bin1 (write test)...\n");
    result = system("./test/bin1.c");
    if (result != 0) {
        printf("Error: bin1 (write test) failed.\n");
        return 1;
    }

    printf("Running bin2 (read test)...\n");
    result = system("./test/bin2");
    if (result != 0) {
        printf("Error: bin2 (read test) failed.\n");
        return 1;
    }

    printf("All tests passed successfully.\n");
    return 0;
}