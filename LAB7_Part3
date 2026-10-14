#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>

/*
 * Blynk MQTT server.
 * Use the regional server assigned to your Blynk account.
 */
#define ADDRESS "ssl://ny3.blynk.cloud:8883"

/*
 * Blynk MQTT authentication
 */
#define USERNAME "device"

/*
 * Replace with your own Blynk Device Auth Token.
 */
#define PASSWORD "YOUR_BLYNK_AUTH_TOKEN"

/*
 * MQTT client ID
 */
#define CLIENTID "RaspberryPiMQTTTest"

/*
 * Blynk Message datastream
 */
#define PUB_TOPIC "ds/Message"

#define QOS 1
#define TIMEOUT 10000L


int main()
{
    MQTTClient client;

    MQTTClient_connectOptions conn_opts =
        MQTTClient_connectOptions_initializer;

    MQTTClient_SSLOptions ssl_opts =
        MQTTClient_SSLOptions_initializer;

    MQTTClient_message pubmsg =
        MQTTClient_message_initializer;

    MQTTClient_deliveryToken token;

    int rc;


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
     * TLS / SSL settings
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
            "Connection failed. Error code: %d\n",
            rc
        );

        MQTTClient_destroy(&client);

        return EXIT_FAILURE;
    }


    printf("Connected to Blynk successfully!\n");


    /*
     * Test message
     */
    char *message =
        "MQTT connection successful!";


    pubmsg.payload = message;

    pubmsg.payloadlen =
        strlen(message);

    pubmsg.qos = QOS;

    pubmsg.retained = 0;


    /*
     * Publish test message
     */
    printf(
        "Publishing test message to Blynk...\n"
    );


    rc = MQTTClient_publishMessage(
        client,
        PUB_TOPIC,
        &pubmsg,
        &token
    );


    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Publish failed. Error: %d\n",
            rc
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


    rc = MQTTClient_waitForCompletion(
        client,
        token,
        TIMEOUT
    );


    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf(
            "Message delivery failed. Error: %d\n",
            rc
        );
    }
    else
    {
        printf(
            "Message published successfully!\n"
        );
    }


    /*
     * Disconnect
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
