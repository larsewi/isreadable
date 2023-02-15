#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

struct ThreadData {
    pthread_attr_t attr;
    pthread_t thread;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    bool result;
};

static void *ThreadRoutine(void *data) {
    struct ThreadData *thread_data = data;

    puts("THREAD: Locking mutex");
    int ret = pthread_mutex_lock(&thread_data->mutex);
    if (ret != 0) {
        printf("THREAD: Failed to lock mutex: %s\n", strerror(ret));
        return NULL;
    }

    puts("THREAD: Unlocking mutex");
    ret = pthread_mutex_unlock(&thread_data->mutex);
    if (ret != 0) {
        printf("THREAD: Failed to unlock mutex: %s\n", strerror(ret));
        return NULL;
    }

    puts("THREAD: Blocking for 3 sec");
    sleep(3);
    thread_data->result = true;

    puts("THREAD: Signalling main thread");
    ret = pthread_cond_signal(&thread_data->cond);
    if (ret != 0) {
        printf("THREAD: Failed to signal main thread: %s\n", strerror(ret));
        return NULL;
    }

    puts("THREAD: Returning from thread routine");
    return NULL;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        puts("Missing argument");
        return EXIT_FAILURE;
    }
    const int timeout = atoi(argv[1]);

    struct ThreadData thread_data;
    thread_data.result = false;

    puts("MAIN: Initializing mutex");
    int ret = pthread_mutex_init(&thread_data.mutex, NULL);
    if (ret != 0) {
        printf("MAIN: Failed to initialize mutex: %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    puts("MAIN: Initializing condition");
    ret = pthread_cond_init(&thread_data.cond, NULL);
    if (ret != 0) {
        printf("MAIN: Failed to initialize condition: %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    puts("MAIN: Locking mutex");
    ret = pthread_mutex_lock(&thread_data.mutex);
    if (ret != 0) {
        printf("MAIN: Failed to lock mutex: %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    puts("MAIN: Initializing thread attributes");
    ret = pthread_attr_init(&thread_data.attr);
    if (ret != 0) {
        printf("Failed to initialize thread attributes\n");
        return EXIT_FAILURE;
    }

    puts("MAIN: Setting thread detach state");
    ret = pthread_attr_setdetachstate(&thread_data.attr, PTHREAD_CREATE_DETACHED);
    if (ret != 0) {
        printf("MAIN: Failed to set thread detach state: %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    puts("MAIN: Creating thread");
    ret = pthread_create(&thread_data.thread, &thread_data.attr, &ThreadRoutine, &thread_data);
    if (ret != 0) {
        printf("MAIN: Failed to create thread: %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    puts("MAIN: Destroying thread attributes");
    ret = pthread_attr_destroy(&thread_data.attr);
    if (ret != 0) {
        printf("MAIN: Failed to destroy thread attributes: %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    puts("MAIN: Calculating timeout interval");
    struct timespec ts = {0};
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        printf("MAIN: Failed to get current time: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    ts.tv_sec += timeout;

    printf("MAIN: Waiting for thread to finish (timeout: %d sec)\n", timeout);
    ret = pthread_cond_timedwait(&thread_data.cond, &thread_data.mutex, &ts);
    switch (ret) {
        case 0:
            printf("MAIN: Thread finished in time; computation evaluated to %s\n", (thread_data.result) ? "true" : "false");

            puts("MAIN: Unlocking mutex");
            ret = pthread_mutex_unlock(&thread_data.mutex);
            if (ret != 0) {
                printf("MAIN: Failed to unlock mutex: %s\n", strerror(ret));
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;

        case ETIMEDOUT:
            puts("MAIN: Thread did not finish in time");
            return EXIT_SUCCESS;

        default:
            printf("MAIN: Failed to wait for thread to finish: %s", strerror(ret));
            return EXIT_FAILURE;
    }
}
