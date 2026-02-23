#include <stdio.h>
#include "thread.h"
#include "xtimer.h"
#include "periph/gpio.h"
#include "shell.h"

static char stack1[THREAD_STACKSIZE_DEFAULT];
static char stack2[THREAD_STACKSIZE_DEFAULT];
static char stack3[THREAD_STACKSIZE_DEFAULT];

static kernel_pid_t thread1_pid;
static kernel_pid_t thread2_pid;
static kernel_pid_t thread3_pid;

int T = 10000000;

static void *thread1_handler(void *arg)
{
    (void)arg;
    while (1)
    {
        LED0_TOGGLE;           // Toggle LED0 state
        xtimer_usleep(T); // (2*f1 = 0.1 Hz)
    }
    return NULL;
}

static void *thread2_handler(void *arg)
{
    (void)arg;
    while (1)
    {
        LED1_TOGGLE;           // Toggle LED1 state
        xtimer_usleep(2*T); // (2*f2 = 0.05 Hz)
    }
    return NULL;
}

static void *thread3_handler(void *arg)
{
    (void)arg;
    while (1)
    {
        LED2_TOGGLE;           // Toggle LED2 state
        xtimer_usleep(4*T); // (2*f3 = 0.025 Hz)
    }
    return NULL;
}

int main(void)
{
    /* Create threads */
    thread1_pid = thread_create(stack1, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN + 1, 0, thread1_handler, NULL, "thread1");
    thread2_pid = thread_create(stack2, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN + 1, 0, thread2_handler, NULL, "thread2");
    thread3_pid = thread_create(stack3, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN + 1, 0, thread3_handler, NULL, "thread3");

    /* Main loop */
    while (1)
    {
        xtimer_usleep(T/10); // Sleep for 1 second
    }
    return 0;
}