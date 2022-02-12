#include <Arduino.h>
#include <WiFi.h>
#define PACKAGE
enum Response : uint8_t{
    R_Handshake, R_SettingReady, R_Check, R_Miss, R_Hit, R_Lost, R_Null
};

enum PackageType : uint8_t{
    Handshake, SettingReady,
    Fire, FireResponse,
    Lose
};

struct PackageHeader{
    PackageType type;
    uint8_t size;
    void send(WiFiClient& client){
        client.write((uint8_t)type);
        client.write((uint8_t)size);
    }
};
struct HandshakePackage{
    PackageHeader header;
    HandshakePackage(){
        header.type = Handshake;
        header.size = 0;
    }
    void send(WiFiClient& client){
        header.send(client);
    }
};
struct SettingReadyPackage{
    PackageHeader header;
    SettingReadyPackage(){
        header.type = SettingReady;
        header.size = 0;
    }
    void send(WiFiClient& client){
        header.send(client);
    }
};
struct FirePackage{
    PackageHeader header;
    uint8_t x;
    uint8_t y;
    FirePackage(uint8_t x, uint8_t y):x(x),y(y){
        header.type = Fire;
        header.size = 2U;
    }
    void send(WiFiClient& client){
        header.send(client);
        client.write(x);
        client.write(y);
    }
};
struct FireResposePackage{
    PackageHeader header;
    bool isHit;
    FireResposePackage(bool isHit):isHit(isHit){
        header.type = FireResponse;
        header.size = sizeof(bool);
    }
    void send(WiFiClient& client){
        header.send(client);
        client.write(isHit);
    }
};
struct LosePackage{
    PackageHeader header;
    LosePackage(){
        header.type = Lose;
        header.size = 0;
    }
    void send(WiFiClient& client){
        header.send(client);
    }
};
