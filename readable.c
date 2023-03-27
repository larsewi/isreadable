/* Licensed under the MIT License
 * Copyright (c) 2023 Lars Erik Wik
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "readable.h"

static pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t CONDITION = PTHREAD_COND_INITIALIZER;

static void CleanupHandler(void *data) {
  const int fd = *((int *)data);
  if (fd >= 0) {
    close(fd);
  }
}

static void *TryRead(void *data) {
  int ret = pthread_mutex_lock(&MUTEX);
  if (ret != 0) {
    fprintf(stderr, "Failed to lock mutex: %s\n", strerror(ret));
    return (void *)READABLE_ERROR;
  }

  int fd = -1;
  bool success;
  pthread_cleanup_push(&CleanupHandler, &fd);

  ret = pthread_mutex_unlock(&MUTEX);
  if (ret != 0) {
    fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(ret));
    return (void *)READABLE_ERROR;
  }

  char buffer[1];
  const char *const filename = data;
  fd = open(filename, O_RDONLY);
  success = ((fd > 0) && (read(fd, buffer, sizeof(buffer)) >= 0));
  pthread_cleanup_pop(1);

  ret = pthread_cond_signal(&CONDITION);
  if (ret != 0) {
    fprintf(stderr, "Failed to signal waiting thread: %s\n", strerror(ret));
    return (void *)READABLE_ERROR;
  }

  return (void *)((success) ? READABLE_SUCCESS : READABLE_FAILURE);
}

int Readable(const char *const filename, const long timeout) {
  // Calculate absolute expiration time from timeout interval in milliseconds.
  struct timespec ts = {0};
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    perror("Failed to get system's real time");
    return READABLE_ERROR;
  }
  ts.tv_sec += ((timeout * 1000000) + ts.tv_nsec) / 1000000000;
  ts.tv_nsec = ((timeout * 1000000) + ts.tv_nsec) % 1000000000;

  int ret = pthread_mutex_lock(&MUTEX);
  if (ret != 0) {
    fprintf(stderr, "Failed to lock mutex: %s\n", strerror(ret));
    return READABLE_ERROR;
  }

  pthread_t thread;
  ret = pthread_create(&thread, NULL, &TryRead, (void *)filename);
  if (ret != 0) {
    fprintf(stderr, "Failed to create thread: %s\n", strerror(ret));
    return READABLE_ERROR;
  }

  ret = pthread_cond_timedwait(&CONDITION, &MUTEX, &ts);
  const bool time_expired = (ret == ETIMEDOUT);

  if (time_expired) {
    ret = pthread_cancel(thread);
    if (ret != 0) {
      fprintf(stderr, "Failed to cancel thread: %s\n", strerror(ret));
      return READABLE_ERROR;
    }
  } else if (ret != 0) {
    fprintf(stderr, "Failed to wait for condition: %s\n", strerror(ret));
    return READABLE_ERROR;
  }

  void *status;
  ret = pthread_join(thread, &status);
  if (ret != 0) {
    fprintf(stderr, "Failed to join thread: %s\n", strerror(ret));
    return READABLE_ERROR;
  }

  ret = pthread_mutex_unlock(&MUTEX);
  if (ret != 0) {
    fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(ret));
    return READABLE_ERROR;
  }

  return (time_expired) ? READABLE_FAILURE : (int)status;
}
