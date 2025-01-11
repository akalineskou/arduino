#include "Beat.h"
#include "Directive.h"
#include "LedController.h"
#include "MicrophoneHelper.h"
#include "RhythmController.h"

#if CHART
  #include <WebSocketsServer.h>
  #include <WiFi.h>

  #include "Secrets.h"

WebSocketsServer webSocketsServer(81);
#endif

MicrophoneHelper microphoneHelper(12, 11, 10);
LedController ledController(6, 5);
RhythmController rhythmController(ledController);

int rhythmBeatCount = 1;
bool rhythmPlayed = false;
Rhythm rhythm;
int rhythmBpm;

Bands bands{};

void setup() {
  Serial.begin(115200);

#if DEBUG
  delay(1000);
#endif

  // if analog input pin 0 is unconnected, random analog noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs. randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  microphoneHelper.setup();
  ledController.setup();

#if DEBUG
  Serial.println("Setup done");
#endif

  rhythm = static_cast<Rhythm>(random(0, sizeof(Rhythms) / sizeof(char*)));

#if CHART
  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFiClass::status() != WL_CONNECTED) {
    delay(100);
  }

  #if DEBUG
  Serial.printf("Connected to %s (%s).\n", wifiSSID, WiFi.localIP().toString().c_str());
  #endif

  webSocketsServer.begin();
#endif
}

void loop() {
  if (!rhythmPlayed) {
#if DEBUG
    if (rhythmBeatCount == 1) {
      Serial.printf("Rhythm: %s\n", Rhythms[rhythm]);
    }
#endif
    rhythmController.play(rhythm, 110, rhythmBeatCount);

    if (rhythmBeatCount == 1) {
      rhythmPlayed = true;
    }
  }

  microphoneHelper.getBands(bands);

  for (const auto beat: {Dum, Tek}) {
    if (bands.is(beat)) {
      ledController.on(beat);
    } else {
      ledController.off(beat);
    }
  }

#if CHART
  webSocketsServer.loop();

  webSocketsServer.broadcastTXT(bands.chartJson().c_str());
#endif
}
