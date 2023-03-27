#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "readable.h"

int main(int argc, char *argv[]) {
  time_t timeout = 10;

  int opt;
  char *endptr;
  while ((opt = getopt(argc, argv, "+ht:")) != -1) {
    switch (opt) {
    case 'h':
      printf("%s [-h] [-t TIMEOUT] [FILENAME ...]\n", argv[0]);
      return EXIT_SUCCESS;

    case 't':
      errno = 0;
      timeout = strtol(optarg, &endptr, 10);
      if (errno != 0 || endptr == optarg) {
        fprintf(stderr, "Failed to parse integer '%s'\n", optarg);
        return EXIT_FAILURE;
      }
      break;

    default:
      return EXIT_FAILURE;
    }
  }

  for (int i = optind; i < argc; i++) {
    const char *const filename = argv[i];
    switch (Readable(filename, timeout)) {
    case READABLE_SUCCESS:
      printf("File '%s' is readable.\n", filename);
      break;

    case READABLE_FAILURE:
      printf("File '%s' is not readable.\n", filename);
      break;

    default:
      fprintf(stderr, "An unexpected error occured while checking file '%s'\n",
              filename);
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
