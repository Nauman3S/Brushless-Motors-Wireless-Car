#include "escHandler.h"
#include "mqttHandler.h"

void setup()
{

    Serial.begin(115200);
    setupESC();
    setupGPRS();
}

void loop()
{

    loopGPRS();
}