#include <Arduino.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <WiFi.h>
#include "AsyncTCP.h"

#ifndef CONFIG_H
#define CONFIG_H

#define SSID "4le3x_pwd"
#define PASSWORD "123456789"

#define SERVER_HOST_NAME "esp-tcp-server"

#define TCP_SERVER_PORT 8000
#define DNS_PORT 53

#endif // CONFIG_H

static DNSServer DNS;

static void handleData(void *arg, AsyncClient *client, void *data, size_t len)
{
	Serial.printf("\n data received from client %s \n", client->remoteIP().toString().c_str());
	Serial.write((uint8_t *)data, len);

	//our big json string test
	//String jsonString = "{\"testeteste\":5}";
	// reply to client
  String alexandre = "$ALX,600,0,0,0,0,0,5B";
	if (client->space() > strlen(alexandre.c_str()) && client->canSend())
	{
		client->add(alexandre.c_str(), strlen(alexandre.c_str()));
		client->send();
	}
}

static void handleError(void *arg, AsyncClient *client, int8_t error)
{
	Serial.printf("\n connection error %s from client %s \n", client->errorToString(error), client->remoteIP().toString().c_str());
}

static void handleDisconnect(void *arg, AsyncClient *client)
{
	Serial.printf("\n client %s disconnected \n", client->remoteIP().toString().c_str());
}

static void handleTimeOut(void *arg, AsyncClient *client, uint32_t time)
{
	Serial.printf("\n client ACK timeout ip: %s \n", client->remoteIP().toString().c_str());
}

static void handleNewClient(void *arg, AsyncClient *client)
{
	Serial.printf("\n new client has been connected to server, ip: %s", client->remoteIP().toString().c_str());
	// register events
	client->onData(&handleData, NULL);
	client->onError(&handleError, NULL);
	client->onDisconnect(&handleDisconnect, NULL);
	client->onTimeout(&handleTimeOut, NULL);
}

void setup()
{
	Serial.begin(9600);
	// create access point
	while (!WiFi.softAP(SSID, PASSWORD, 6, false, 15))
	{
		delay(500);
		Serial.print(".");
	}

	// start dns server
	if (!DNS.start(DNS_PORT, SERVER_HOST_NAME, WiFi.softAPIP()))
		Serial.printf("\n failed to start dns service \n");

	AsyncServer *server = new AsyncServer(TCP_SERVER_PORT); // start listening on tcp port 7050
	server->onClient(&handleNewClient, server);
	server->begin();
}

void loop()
{

	DNS.processNextRequest();
  
}