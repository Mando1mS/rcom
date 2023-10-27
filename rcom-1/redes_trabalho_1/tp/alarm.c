// Alarm example
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]

#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#define FALSE 0
#define TRUE 1

int alarmEnabled = FALSE;
int alarmCount = 0;

// Alarm function handler
// user-defined function to handle alarms. May be changed. The
// important thing is that this function changes the value of an alarm
// flag and that it increases the number of alarms
void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;

    printf("Alarm #%d\n", alarmCount);
}

int main()
{
    // Set alarm function handler
    // install the OS func8on signal to be automa8cally
    // invoked when the 8mer expires, invoking in its turn
    // the user func8on alarmHandler
    (void)signal(SIGALRM, alarmHandler);

    //alarm(t); // ac8vate alarm in t seconds
    //alarm(0); // Disable pending alarms, if any    


    while (alarmCount < 4)
    {
        if (alarmEnabled == FALSE)
        {
            alarm(3); // Set alarm to be triggered in 3s
            alarmEnabled = TRUE;
        }
    }

    printf("Ending program\n");

    return 0;
}
