#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <MQTTClient.h>
#include <wiringPi.h>

#include "bmp180.h"


/*
 * BLYNK MQTT SETTINGS
 */

// Use the SAME address that worked in Part III
#define ADDRESS "ssl://ny3.blynk.cloud:8883"

#define CLIENTID "RaspberryPiSensorLED"

#define USERNAME "device"

// Replace this with YOUR Blynk Auth Token
#define PASSWORD "YOUR_BLYNK_AUTH_TOKEN"


/* Blynk datastream topics 
TODO: define tempture, pressure, Altitude, and LED (3 lines of code)
Hint: read LAB7_Part3.c
*/


/*
 * MQTT SETTINGS
 */

#define QOS     1
#define TIMEOUT 10000L


/*
 * LED SETTINGS
 *
 * WiringPi pin 0
 * = BCM GPIO17
 * = Raspberry Pi physical pin 11
 */

#define LedPin 0


volatile int running = 1;


/*
 * TODO: Implement `led_on()` and `led_off()` functions to control the LED. 
 `led_on()` should set `LedPin` to HIGH and print “LED is ON,” 
 while `led_off()` should set `LedPin` to LOW and print “LED is OFF.”
 */


/*************************************************
 * CTRL+C HANDLER
 *************************************************/

void handle_signal(int signal)
{
    running = 0;
}


/*
 * MQTT MESSAGE CALLBACK
 *
 * Receives Blynk LED commands
 
 */

int messageArrived(
    void *context,
    char *topicName,
    int topicLen,
    MQTTClient_message *message)
{
    char payload[64];

    int len = message->payloadlen;

    if (len >= sizeof(payload))
    {
        len = sizeof(payload) - 1;
    }

    memcpy(
        payload,
        message->payload,
        len
    );

    payload[len] = '\0';


    printf("\n==============================\n");
    printf("MQTT message received\n");
    printf("Topic: %s\n", topicName);
    printf("Message: %s\n", payload);


    /*
     * Blynk Switch:
     *
     * 1 = ON
     * 0 = OFF
     */

    if (
        strcmp(payload, "1") == 0 ||
        strcmp(payload, "on") == 0
    )
    {
        led_on();
    }

    else if (
        strcmp(payload, "0") == 0 ||
        strcmp(payload, "off") == 0
    )
    {
        led_off();
    }

    else
    {
        printf(
            "Unknown LED command: %s\n",
            payload
        );
    }


    MQTTClient_freeMessage(
        &message
    );

    MQTTClient_free(
        topicName
    );

    return 1;
}


/*
 * FUNCTION TO PUBLISH SENSOR VALUE
 */

int publishValue(
    MQTTClient client,
    const char *topic,
    const char *value)
{
    MQTTClient_message message =
        MQTTClient_message_initializer;

    MQTTClient_deliveryToken token;

    int rc;


    message.payload =
        (void *)value;

    message.payloadlen =
        strlen(value);

    message.qos =
        QOS;

    message.retained =
        0;


    rc = MQTTClient_publishMessage(
        client,
        topic,
        &message,
        &token
    );


    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Failed to publish %s\n",
            topic
        );

        return rc;
    }


    rc = MQTTClient_waitForCompletion(
        client,
        token,
        TIMEOUT
    );


    return rc;
}


/*************************************************
 * MAIN PROGRAM
 *************************************************/

int main()
{
    int rc;


    /*************************************************
     * INITIALIZE LED
     *************************************************/

    if (wiringPiSetup() == -1)
    {
        printf(
            "WiringPi setup failed!\n"
        );

        return EXIT_FAILURE;
    }


    pinMode(
        LedPin,
        OUTPUT
    );


    // LED starts OFF
    digitalWrite(
        LedPin,
        LOW
    );


    printf(
        "GPIO17 configured for LED.\n"
    );


    /*************************************************
     * CTRL+C
     *************************************************/

    signal(
        SIGINT,
        handle_signal
    );


    /*************************************************
     * CREATE MQTT CLIENT
     *************************************************/

    MQTTClient client;


    MQTTClient_connectOptions conn_opts =
        MQTTClient_connectOptions_initializer;


    MQTTClient_SSLOptions ssl_opts =
        MQTTClient_SSLOptions_initializer;


    printf(
        "Creating MQTT client...\n"
    );


    rc = MQTTClient_create(
        &client,
        ADDRESS,
        CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE,
        NULL
    );


    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Failed to create MQTT client. Error: %d\n",
            rc
        );

        return EXIT_FAILURE;
    }


    /*************************************************
     * SET MQTT CALLBACK
     *************************************************/

    rc = MQTTClient_setCallbacks(
        client,
        NULL,
        NULL,
        messageArrived,
        NULL
    );


    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Failed to set MQTT callback.\n"
        );

        MQTTClient_destroy(
            &client
        );

        return EXIT_FAILURE;
    }


    /*************************************************
     * BLYNK CONNECTION SETTINGS
     *************************************************/

    conn_opts.keepAliveInterval =
        45;

    conn_opts.cleansession =
        1;

    conn_opts.username =
        USERNAME;

    conn_opts.password =
        PASSWORD;

    conn_opts.MQTTVersion =
        MQTTVERSION_3_1_1;


    /*************************************************
     * TLS SETTINGS
     *************************************************/

    ssl_opts.enableServerCertAuth =
        1;


    ssl_opts.trustStore =
        "/etc/ssl/certs/ca-certificates.crt";


    conn_opts.ssl =
        &ssl_opts;


    /*************************************************
     * CONNECT TO BLYNK
     *************************************************/

    printf(
        "Connecting to Blynk MQTT...\n"
    );


    rc = MQTTClient_connect(
        client,
        &conn_opts
    );


    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Failed to connect to Blynk. Error: %d\n",
            rc
        );

        MQTTClient_destroy(
            &client
        );

        return EXIT_FAILURE;
    }


    printf(
        "Connected to Blynk successfully!\n"
    );


    /*
     * TODO: TODO: Subscribe to `LED_TOPIC` so the Raspberry Pi can receive LED control commands. 
     * No value needs to be published because this step is only listening for incoming messages.
     * Hint: use printf();
     * Then, subscribe the MQTT client to `LED_TOPIC` using the specified `QOS`, 
     * and store the return code in `rc` to check whether the subscription succeeds.
     * Hint: use MQTTClient_subscribe();
     */


    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Failed to subscribe to LED topic.\n"
        );

        MQTTClient_disconnect(
            client,
            1000
        );

        MQTTClient_destroy(
            &client
        );

        return EXIT_FAILURE;
    }


    printf(
        "Subscribed successfully!\n"
    );


    /*************************************************
     * INITIALIZE BMP180
     *************************************************/

    char *i2c_device =
        "/dev/i2c-1";


    int address =
        0x77;


    void *bmp =
        bmp180_init(
            address,
            i2c_device
        );


    if (bmp == NULL)
    {
        printf(
            "Failed to initialize BMP180 sensor.\n"
        );

        MQTTClient_disconnect(
            client,
            1000
        );

        MQTTClient_destroy(
            &client
        );

        return EXIT_FAILURE;
    }


    printf(
        "BMP180 initialized successfully!\n"
    );


    printf(
        "\n====================================\n"
    );

    printf(
        "PART V PROGRAM RUNNING\n"
    );

    printf(
        "BMP180 -> Blynk\n"
    );

    printf(
        "Blynk LED Switch -> Physical LED\n"
    );

    printf(
        "Press Ctrl+C to stop.\n"
    );

    printf(
        "====================================\n\n"
    );


    /*************************************************
     * MAIN LOOP
     *************************************************/

    while (running)
    {
        /*
         * Read BMP180
         */

        float temperature =
            bmp180_temperature(
                bmp
            );


        long pressure =
            bmp180_pressure(
                bmp
            );


        float altitude =
            bmp180_altitude(
                bmp
            );


        /*********************************************
         * Convert sensor values to strings
         *********************************************/

        char tempString[32];

        char pressureString[32];

        char altitudeString[32];


        /*
            TODO: Convert readings to strings for temperature, pressure, and altitude. 
            Use tempMessage, pressureMessage, and altitudeMessage for your output.
            Hint: use snprintf();
        */


        /*********************************************
         * Display readings
         *********************************************/

        printf(
            "\n------------------------------\n"
        );


        /*
         * TODO: Publish tempMessage, pressureMessage, altitudeMessage to its Blynk datastream.
         Hint: use publishValue();
         
         */


        /*********************************************
         * Publish to Blynk
         *********************************************/

        publishValue(
            client,
            TEMP_TOPIC,
            tempString
        );


        publishValue(
            client,
            PRESSURE_TOPIC,
            pressureString
        );


        publishValue(
            client,
            ALTITUDE_TOPIC,
            altitudeString
        );


        printf(
            "Sensor values published to Blynk.\n"
        );


        /*
         * Wait 2 seconds before next sensor reading.
         *
         * MQTT callback can still receive the
         * Blynk LED switch messages.
         */

        sleep(2);
    }


    /*************************************************
     * SHUTDOWN
     *************************************************/

    printf(
        "\nStopping program...\n"
    );


    // Always turn LED OFF before exiting
    led_off();


    /*
     * We are not calling bmp180_close() here
     * because the original supplied BMP180 driver
     * did not contain that implementation.
     * Linux will release the process resources
     * when the program exits.
     */


    MQTTClient_disconnect(
        client,
        1000
    );


    MQTTClient_destroy(
        &client
    );


    printf(
        "Disconnected from Blynk.\n"
    );


    return EXIT_SUCCESS;
}


