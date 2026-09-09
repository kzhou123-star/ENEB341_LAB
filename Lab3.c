#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

int main()
{
    int handle;
    int dataAvailable;
    int bytesRead;
    char buf[1000];

    /* TODO: In the if statement, initialize the GPIO library and check if it has properly initialized*/
    if ()
    {
        printf("initialization failed\n");
        return 1;
    }

    printf("initialization ok\n");

    /* TODO: handle should open the serial device using the correct serial device address
     * HINT: use ls -l /dev/serial* in the terminal to find the device and make sure to set the baud rate correctly*/
    handle = ; 

    if (handle < 0)
    {
        printf("failed to open\n");
        gpioTerminate();
        return 1;
    }

    printf("opened ok\n");

    while (1)
    {
        /* TODO: check if there is data available */
        dataAvailable = ;

        if (dataAvailable > 0)
        {
            if (dataAvailable > 999)
                dataAvailable = 999;
            
            /* TODO: Read the data */
            bytesRead = ;

            if (bytesRead > 0)
            {
                /* TODO: 
                Mark the end of the data with a null.
                Print the data 
                Display the data
                */
            }
        }

        gpioDelay(10000);
    }

    serClose(handle);
    gpioTerminate();

    return 0;
}

