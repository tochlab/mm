#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

int main(int argc, char** argv) {
    uint32_t start_addr;
    uint32_t count;
    void *mptr;
    FILE *outFd;
    int memFd;

    outFd = stdout;

    if( argc < 3 ) {
        printf("Usage: %s address count\n", argv[0]);
        printf("\tAccess is ALWAYS 32bit!\n");
        exit(1);
    }

    start_addr = strtoul(argv[1], 0, 0);
    count = strtoul(argv[2], 0, 0);

    memFd = open("/dev/mem", O_RDWR | O_SYNC);

    if( memFd == -1 ) {
        perror("open");
        exit(1);
    }

    mptr = mmap(0, count * sizeof(uint32_t), PROT_READ | PROT_WRITE, MAP_SHARED, memFd, start_addr & ~MAP_MASK );
    if( mptr == MAP_FAILED ) {
        perror("mmap");
        close(memFd);
        exit(1);
    }

    uint32_t *virt32 = (uint32_t *) (mptr + (start_addr & MAP_MASK));
    for(int c = 0;c<count;c++) {
        uint32_t val = virt32[c];
        fwrite(&val, sizeof(uint32_t), 1, outFd);
    }

    munmap(mptr, count * sizeof(uint32_t));
    close(memFd);

    return 0;
}