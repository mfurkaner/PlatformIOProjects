#include <Arduino.h>
#include <WiFi.h>
#include <U8g2lib.h>
#include <string>


#include "hash.h"
#ifndef PACKAGE
    #include "package.h"
#endif

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED;
#define CONNECTION

#define PORT 25
#define WIFI_TIMEOUT 10000 //ms


enum Connection{
    _Gateway, _Client
};

enum ConnectionStanding{
    NotConnected, Connected
};

class TCPConnection{
    String ssid = "AMIRALBATTI";
    IPAddress clientIP = IPAddress(192, 168, 137, 78);
    IPAddress gatewayIP = IPAddress(192, 168, 137, 5);
    IPAddress subnet = IPAddress(255, 255, 255, 0);
    WiFiServer server;
    WiFiClient client;
    Connection connectionType;
    ConnectionStanding standing = NotConnected;
    bool serverStarted = false;

public:
    uint8_t hitX;
    uint8_t hitY;
    void setConnectionType(Connection type){ connectionType = type;}
    Connection getConnectionType(){ return connectionType; }
    bool printNearbyNetworks(){
        bool isHere = false;
        int16_t num_of_networks = WiFi.scanNetworks();
        OLED.clearBuffer();
        OLED.drawFrame(0,0,OLED.getWidth(),OLED.getHeight());
        OLED.setFont(u8g2_font_profont12_tf);
        OLED.setCursor(15, 2*OLED.getFontAscent());
        OLED.print("Nearby networks :");
        OLED.setFont(u8g2_font_profont10_tf);
        for(int i = 0 ; i < num_of_networks && i < 3 ; i++){
            OLED.setCursor(10, (i+2.5)*2*OLED.getFontAscent());
            OLED.print(String(i+1) + ") " + WiFi.SSID(i).substring(0,19));
            if (WiFi.SSID(i) == ssid)  isHere = true;      
        }
        OLED.sendBuffer();
        return isHere;
    }

    bool setUpConnection(){
        long int ssid_hash =  hash_str(ssid.c_str()) ;
        switch (connectionType)
        {
        case _Client:
            print_clientWaitingConnection();
            if( WiFi.status() != WL_CONNECTED ){
                if (!serverStarted){
                    WiFi.disconnect();  
                    WiFi.mode(WIFI_STA);
                    WiFi.config(clientIP, gatewayIP, subnet);      
                    WiFi.begin(ssid.c_str(), String( ssid_hash ).c_str()); 
                    serverStarted = true;  
                }            
                else{
                    WiFi.reconnect();
                }           
                WiFi.waitStatusBits(WL_CONNECTED, WIFI_TIMEOUT/4);
            }
            else if( client.connect(gatewayIP, PORT, WIFI_TIMEOUT) ) { 
                print_verifying(); 
                verifyCommunication(); 
                standing = Connected; 
            }
            break;
        case _Gateway:
            print_gatewayWaitingConnection();
            if ( !serverStarted ){
                WiFi.disconnect();
                WiFi.mode(WIFI_AP);
                WiFi.softAPConfig(gatewayIP, gatewayIP, subnet);      
                WiFi.softAP(ssid.c_str(), String( ssid_hash ).c_str() );   
                server = WiFiServer(PORT, 2);
                server.setNoDelay(true);
                delay(1000);
                server.begin(PORT);
                serverStarted = true;
                delay(2000);
            }
            else if ( server.hasClient() ) { print_verifying(); client = server.available(); verifyCommunication(); standing = Connected; }
            break;
        default:
            break;
        }
        return standing == Connected;
    }

    void print_clientWaitingConnection(){
        OLED.clearBuffer();
        OLED.drawFrame(0,0,OLED.getWidth(),OLED.getHeight());
        OLED.setFont(u8g2_font_profont10_tf);
        OLED.setCursor(15, 5*OLED.getFontAscent());
        OLED.print("Connecting to server");
        OLED.sendBuffer();
    }

    void print_gatewayWaitingConnection(){
        OLED.clearBuffer();
        OLED.drawFrame(0,0,OLED.getWidth(),OLED.getHeight());
        OLED.setFont(u8g2_font_profont10_tf);
        OLED.setCursor(15, 5*OLED.getFontAscent());
        OLED.print("Waiting for a client");
        OLED.sendBuffer();
    }

    void print_verifying(){
        OLED.clearBuffer();
        OLED.drawFrame(0,0,OLED.getWidth(),OLED.getHeight());
        OLED.setFont(u8g2_font_profont10_tf);
        OLED.setCursor(15, 5*OLED.getFontAscent());
        OLED.print("Verifying connection");
        OLED.sendBuffer();
        delay(500);
    }

    void read(int m){
        uint8_t bytes[m];
        while(client.available() < m);
        client.read(bytes, m);
        OLED.clearBuffer();
        OLED.setFont(u8g2_font_profont10_tf);
        OLED.setCursor(25, 2*OLED.getFontAscent());
        OLED.print("1: " + String(bytes[0]));
        OLED.setCursor(25, 4*OLED.getFontAscent());
        OLED.print("2 : " + String(bytes[1]));
        OLED.setCursor(25, 6*OLED.getFontAscent());
        OLED.print("3 : " +String(bytes[2]));
        OLED.setCursor(25, 8*OLED.getFontAscent());
        OLED.print("4 : " +String(bytes[3]));
        OLED.sendBuffer();
    }

    PackageHeader waitHeader(){
        uint8_t bytes[2];
        while(client.available() < 2);
        client.read(bytes, 2);
        PackageType type = (PackageType)bytes[0];
        PackageHeader header;
        header.type = type;
        header.size = bytes[1];
        return header;
    }


    Response handleIncoming(){
        PackageHeader header = waitHeader();
        switch (header.type)
        {
        case Handshake:
            return R_Handshake;
        break;
        case SettingReady:
            return R_SettingReady;
        break;
        case Fire:
            uint8_t bytes[2];
            while(client.available() < 2);
            client.read(bytes, 2);
            hitX = bytes[0];
            hitY = bytes[1];
            return R_Check;
        break;
        case FireResponse:
            uint8_t byte[1];
            while(client.available() < 1);
            client.read(byte, 1);
            return byte[0] ? R_Hit : R_Miss;
        break;
        case Lose:
            return R_Lost;
        break;
        default:
        break;
        }
        return R_Null;
    }

    void notifySettingReady(){
        SettingReadyPackage package;
        package.send(client);
    }
    void notifyFiring(uint8_t x, uint8_t y){
        FirePackage package(x, y);
        package.send(client);
    }
    void notifyFireRespose(bool isHit){
        FireResposePackage package(isHit);
        package.send(client);
    }
    void notifyLosing(){
        LosePackage package;
        package.send(client);
    }
    void verifyCommunication(){
        HandshakePackage package;
        package.send(client);
        while(handleIncoming() != R_Handshake);
    }
    void waitForEnemySettings(){
        while(handleIncoming() != R_SettingReady);
    }

    bool checkConnection(){
        bool connectionFlag = ( connectionType == _Client ) ? WiFi.status() == WL_CONNECTED : server.hasClient() ;
        return connectionFlag && client.connected();}
    bool available(){return client.available();}
};