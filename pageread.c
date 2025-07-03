/* SPDX-License-Identifier: MIT */
/**
 * @file pageread.c
 *
 * Simple program to read from any location in memory pages.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <version.h>

#define DEVMEM_FILE "/dev/mem"
#define PAGE_SIZE 4096UL
#define PAGE_MASK (PAGE_SIZE - 1)
#define NPAGES 1
#define NBYTES 1

static void print_usage(const char *progname)
{
    fprintf(stderr, "\nUsage: %s [option(s)]\n"
        "\t -a|--addr=<value>     : HPA address to start reading pages from\n"
        "\t[-p|--pages=<value>]   : Number of pages to span (default: 1)\n"
        "\t[-b|--bytes=<value>]   : Number of bytes to read from each page (default: 1)\n"
        "\t[-d|--dump]            : Dump read data to the console (default: Data are not dumped)\n"
        "\t[-c|--cached]          : Use cached mappings (default: Memory access is not cached)\n"
        "\t[-h|--help]            : Print this help message",
        progname
    );
}

int main(int argc, char **argv)
{
    int retval = 0;
    int fd;
    void *map_base;
    size_t map_size;
    char *p;
    unsigned long long hpa = 0;
    long npages = NPAGES;
    long nbytes = NBYTES;
    bool dump = false;
    bool cached = false;

    fprintf(stdout, "%s - version: %s\n", argv[0], PROJECT_VER);

    static const struct option long_options[] = {
        {"addr",      required_argument, 0, 'a'},
        {"pages",     required_argument, 0, 'p'},
        {"bytes",     required_argument, 0, 'b'},
        {"dump",      no_argument,       0, 'd'},
        {"cached",    no_argument,       0, 'c'},
        {"help",      no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    for (;;) {
        int c = getopt_long(argc, argv, "a:p:b:dch", long_options, 0);
        if (c == -1)
            break;

        switch (c) {
            case 'a':
                hpa = strtoull(optarg, 0, 0);
                break;

            case 'p':
                npages = strtol(optarg, 0, 0);
                break;

            case 'b':
                nbytes = strtol(optarg, 0, 0);
                break;

            case 'd':
                dump = true;
                break;

            case 'c':
                cached = true;
                break;

            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
                break;

            default:
                /* do nothing */
                break;
        }
    }

    if (hpa == 0) {
        fprintf(stderr, "Invalid hpa address\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((hpa & PAGE_MASK) != 0) {
        fprintf(stderr, "hpa address must be page aligned\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (npages == 0) {
        fprintf(stderr, "Invalid number of pages to scan\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (nbytes == 0) {
        fprintf(stderr, "Invalid number of bytes to read from the page\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (cached) {
        if ((fd = open(DEVMEM_FILE, O_RDONLY | O_SYNC)) == -1) {
            fprintf(stderr, "open(%s, O_RDONLY | O_SYNC) failed with code %d (%s)\n",
                DEVMEM_FILE, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        if ((fd = open(DEVMEM_FILE, O_RDONLY | O_SYNC | O_DSYNC)) == -1) {
            fprintf(stderr, "open(%s, O_RDONLY | O_SYNC | O_DSYNC) failed with code %d (%s)\n",
                DEVMEM_FILE, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stdout, "%s opened\n", DEVMEM_FILE);

    map_size = npages * PAGE_SIZE;
    map_base = mmap(0, map_size, PROT_READ, MAP_SHARED, fd, hpa & ~PAGE_MASK);
    if (map_base == (void *) -1) {
        fprintf(stderr, "mmap(0, 0x%zx, PROT_READ, MAP_SHARED, fd, 0x%llx) failed with code %d (%s)\n",
            map_size, hpa, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    p = (char*)map_base;

    for (long i = 0; i < npages; i++) {
        if (dump)
            printf("page: %ld\n", i);
        for (int j = 0; j < nbytes; j++) {
            unsigned char c = p[i * PAGE_SIZE + j];
            if (dump) {
                if ((j > 0) && ((j % 16) == 0))
                printf("%02x \n", c);
            }
            retval += c;
        }
    }

    if (munmap(map_base, map_size) == -1) {
        fprintf(stderr, "munmap(%p, 0x%zx) failed with code %d (%s)\n",
            map_base, map_size, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    close(fd);
    fprintf(stdout, "%ld pages touched (%ld bytes in each page)\n", npages, nbytes);

    return retval;
}

