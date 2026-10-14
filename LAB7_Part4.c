
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <MQTTClient.h>
#include "bmp180.h"
#include <wiringPi.h>

/*
 * Use the SAME Blynk server that worked in Part III.
 * If your Part III ADDRESS is different, copy that address here.
 */
#define ADDRESS     "ssl://ny3.blynk.cloud:8883"

#define CLIENTID    "RaspberryPiBMP180"

#define USERNAME    "device"

/*
 * Put YOUR Blynk Auth Token here.
 * Use the same token that worked in Part III.
 */
#define PASSWORD "YOUR_BLYNK_AUTH_TOKEN"

/* Blynk datastream topics 
TODO: define tempture, pressure, and Altitude (3 lines of code)
Hint: read LAB7_Part3.c
*/

#define QOS         1
#define TIMEOUT     10000L


/*
 * Helper function for publishing one value to Blynk
 */
int publishValue(
    MQTTClient client,
    const char *topic,
    const char *value)
{
    MQTTClient_message pub_msg =
        MQTTClient_message_initializer;

    MQTTClient_deliveryToken token;

    int rc;

    pub_msg.payload = (void *)value;
    pub_msg.payloadlen = strlen(value);
    pub_msg.qos = QOS;
    pub_msg.retained = 0;

    rc = MQTTClient_publishMessage(
        client,
        topic,
        &pub_msg,
        &token
    );

    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Failed to publish to %s. Error: %d\n",
            topic,
            rc
        );

        return rc;
    }

    rc = MQTTClient_waitForCompletion(
        client,
        token,
        TIMEOUT
    );

    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Delivery confirmation failed for %s\n",
            topic
        );
    }

    return rc;
}


int main()
{
    int rc;

    MQTTClient client;

    MQTTClient_connectOptions conn_opts =
        MQTTClient_connectOptions_initializer;

    MQTTClient_SSLOptions ssl_opts =
        MQTTClient_SSLOptions_initializer;


    /*
     * Create MQTT client
     */
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


    /*
     * Blynk MQTT connection settings
     */
    conn_opts.keepAliveInterval = 45;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;


    /*
     * TLS settings
     */
    ssl_opts.enableServerCertAuth = 1;

    ssl_opts.trustStore =
        "/etc/ssl/certs/ca-certificates.crt";

    conn_opts.ssl = &ssl_opts;


    /*
     * Connect to Blynk
     */
    printf("Connecting to Blynk MQTT...\n");

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

        MQTTClient_destroy(&client);

        return EXIT_FAILURE;
    }

    printf("Connected to Blynk successfully!\n");


    /*
     * Initialize BMP180
     *
     */
    char *i2c_device = "/dev/i2c-1";

    int address = 0x77;

    void *bmp =
        bmp180_init(
            address,
            i2c_device
        );


    if (bmp == NULL)
    {
        printf("Failed to initialize BMP180 sensor\n");

        MQTTClient_disconnect(
            client,
            1000
        );

        MQTTClient_destroy(
            &client
        );

        return EXIT_FAILURE;
    }


    printf("BMP180 initialized successfully!\n");
    printf("Publishing BMP180 readings to Blynk...\n\n");


    /*
     * Same basic structure as the original lab:
     * take 100 readings, one every 2 seconds.
     */
    int i;

    for (i = 0; i < 100; i++)
    {
        /*
         * Read BMP180
         */
        float temperature =
            bmp180_temperature(bmp);

        long pressure =
            bmp180_pressure(bmp);

        float altitude =
            bmp180_altitude(bmp);


        /*
         * Convert readings to strings
         * because MQTT payloads are sent as bytes/text.
         */
        char tempMessage[32];
        char pressureMessage[32];
        char altitudeMessage[32];

        /*
            TODO: Convert readings to strings for temperature, pressure, and altitude. 
            Use tempMessage, pressureMessage, and altitudeMessage for your output.
            Hint: use snprintf();
        */


        /*
         * Print readings of temperature, pressure, and altitude to Raspberry Pi terminal. 
             Hint:use printf();
         
         */
        printf("------------------------------\n");



        /*
         * TODO: Publish tempMessage, pressureMessage, altitudeMessage to its Blynk datastream.
         Hint: use publishValue();
         
         */


        printf("Published to Blynk!\n\n");


        /*
         * Wait 2 seconds
         */
        sleep(2);
    }


    /*
     * Clean up BMP180
     */
    bmp180_close(bmp);


    /*
     * Disconnect MQTT
     */
    MQTTClient_disconnect(
        client,
        1000
    );

    MQTTClient_destroy(
        &client
    );


    printf("Disconnected.\n");

    return EXIT_SUCCESS;
}
