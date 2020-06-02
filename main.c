#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define OP_HALT 0x0
#define OP_INC 0x1
#define OP_DEC 0x2
#define OP_MUL2 0x3
#define OP_DIV2 0x4
#define OP_ADD7 0x5
#define OP_NEG 0x6
int interp_switch(unsigned char *code, int initval)
{
    int pc = 0;
    int val = initval;

    while (1) {
        switch (code[pc++]) {
            case OP_HALT:
                //fprintf(stderr, "debug: %d\n", pc);
                return val;

            case OP_INC:
                val++;
                break;

            case OP_DEC:
                val--;
                break;

            case OP_MUL2:
                val *= 2;
                break;

            case OP_DIV2:
                val /= 2;
                break;

            case OP_ADD7:
                val += 7;
                break;

            case OP_NEG:
                val = -val;
                break;

            default:
                return val;
        }
    }
}

int interp_cgoto(unsigned char *code, int initval)
{
    /* The indices of labels in the dispatch_table are the relevant opcodes
     */

/* This is gcc extension for goto label jump */
    static void *dispatch_table[] = {&&do_halt, &&do_inc,  &&do_dec, &&do_mul2,
                                     &&do_div2, &&do_add7, &&do_neg
                                    };

#define DISPATCH() goto *dispatch_table[code[pc++]]

    int pc = 0;
    int val = initval;

    DISPATCH();

    while (1) {
do_halt:
        //fprintf(stderr, "debug: %d\n", pc);
        return val;
do_inc:
        val++;
        DISPATCH();
do_dec:
        val--;
        DISPATCH();
do_mul2:
        val *= 2;
        DISPATCH();
do_div2:
        val /= 2;
        DISPATCH();
do_add7:
        val += 7;
        DISPATCH();
do_neg:
        val = -val;
        DISPATCH();
    }
}

#define TOTAL_NUM   (100000)
int main(int argc, const char *argv[])
{
    unsigned char *code = malloc(TOTAL_NUM);

    if (!code) {
        fprintf(stderr, "malloc fail!\n");
        return -1;
    }

    int fd = open("/dev/urandom", O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "open fail!\n");
        return -1;
    }

    ssize_t result = read(fd, code, TOTAL_NUM);
    if (result < 0) {
        fprintf(stderr, "read fail!\n");
        return -1;
    }
    else if (result != TOTAL_NUM) {
        fprintf(stderr, "read less fail!\n");
        return -1;
    }

    int i;
    for (i = 0; i < TOTAL_NUM; i++) {
       code[i] = code[i] % 6 + 1;
    }

    code[TOTAL_NUM - 1] = 0;
    i = 10000;
    while (i > 0) {
        interp_switch(code, 0);
        //interp_cgoto(code, 0);
        i--;
    }
    return 0;
}
