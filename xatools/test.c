#include <stdio.h>
#include <stdlib.h>
#include "libxautil/llz_print.h"


int main(int argc, char *argv[])
{

    LLZ_PRINT_INIT(LLZ_PRINT_ENABLE,
            LLZ_PRINT_FILE_ENABLE,LLZ_PRINT_STDOUT_ENABLE,LLZ_PRINT_STDERR_ENABLE,
            LLZ_PRINT_PID_ENABLE,
            "./log/", "testlog", "TS", 3);


    LLZ_PRINT_DBG("TEST: now test debug info\n");
    printf("hello world, hehe! \n");

    LLZ_PRINT_UNINIT();
    return 0;  
}
