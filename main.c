#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t CONDITION = PTHREAD_COND_INITIALIZER;

static void *TryRead(void *data) {
  const char *const filename = data;

  int ret = pthread_mutex_lock(&MUTEX);
  if (ret != 0) {
    fprintf(stderr, "Failed to lock mutex: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_mutex_unlock(&MUTEX);
  if (ret != 0) {
    fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  char buffer;
  bool success = false;
  const int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    // Failed to open file.
  } else if (read(fd, &buffer, sizeof(buffer)) < 0) {
    // Failed to read file.
    close(fd);
  } else {
    // Successfully read file.
    success = true;
    close(fd);
  }

  ret = pthread_cond_signal(&CONDITION);
  if (ret != 0) {
    fprintf(stderr, "Failed to signal waiting thread: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  return (void *)success;
}

/**
 * @brief Determine if a file is readable.
 * @param[in] filename path to file.
 * @param[in] timeout milliseconds to wait.
 * @return true if readable.
 */
bool IsReadable(const char *const filename, const long timeout) {
  // Calculate expiration time.
  struct timespec ts = {0};
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    perror("Failed to get system's real time");
  }
  ts.tv_sec += ((timeout * 1000000) + ts.tv_nsec) / 1000000000;
  ts.tv_nsec = ((timeout * 1000000) + ts.tv_nsec) % 1000000000;

  int ret = pthread_mutex_lock(&MUTEX);
  if (ret != 0) {
    fprintf(stderr, "Failed to lock mutex: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  pthread_t thread;
  ret = pthread_create(&thread, NULL, &TryRead, (void *)filename);
  if (ret != 0) {
    fprintf(stderr, "Failed to create thread: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_cond_timedwait(&CONDITION, &MUTEX, &ts);
  const bool time_expired = (ret == ETIMEDOUT);
  if (ret != 0 && !time_expired) {
    fprintf(stderr, "Failed to wait for condition: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_cancel(thread);
  if (ret != 0) {
    fprintf(stderr, "Failed to cancel thread: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  void *status;
  ret = pthread_join(thread, &status);
  if (ret != 0) {
    fprintf(stderr, "Failed to join thread: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_mutex_unlock(&MUTEX);
  if (ret != 0) {
    fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  return !time_expired && (bool)status;
}

int main(int argc, char *argv[]) {
  time_t timeout = 10;

  int opt;
  while ((opt = getopt(argc, argv, "+ht:")) != -1) {
    switch (opt) {
    case 'h':
      printf("%s [-h] [-t TIMEOUT] [FILENAME ...]\n", argv[0]);
      return EXIT_SUCCESS;
    case 't':
      char *endptr;
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
    printf("%s %s readable.\n", filename,
           (timeout > 0 && IsReadable(filename, timeout)) ? "is" : "not");
  }
}
