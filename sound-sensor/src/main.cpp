#include "Bands.h"
#include "Beat.h"
#include "Directive.h"
#include "LedController.h"
#include "MicrophoneHelper.h"
#include "Rhythm.h"
#include "RhythmController.h"
#include "TimeDelay.h"

#if APP_CHART
  #include <WebSocketsServer.h>
  #include <WiFi.h>

  #include "Secrets.h"

WebSocketsServer webSocketsServer(81);
#endif

MicrophoneHelper microphoneHelper(12, 11, 10);
LedController ledController(6, 5);
RhythmController rhythmController(ledController);

int rhythmBeatCount = 1;
bool idling = false;
const RhythmData* rhythmData = RhythmDatas[0];

TimeDelay idleRhythmTimeDelay(60 * 1000, true);
TimeDelay rhythmChangeTimeDelay(10 * 1000, true);

Bands bands{};

void setup() {
  Serial.begin(115200);

#if APP_DEBUG
  delay(1000);
#endif

  // if analog input pin 0 is unconnected, random analog noise will be generated
  randomSeed(analogRead(0));

  ledController.setup();

#if APP_CHART
  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFiClass::status() != WL_CONNECTED) {
    delay(100);
  }

  #if APP_DEBUG
  Serial.printf("Connected to %s (%s).\n", wifiSSID, WiFi.localIP().toString().c_str());
  #endif

  webSocketsServer.begin();
#endif

#if APP_DEBUG
  Serial.println("Setup done.");
#endif
}

void loop() {
#if APP_RHYTHM_PLAY
  if (!idling && idleRhythmTimeDelay.delayPassed()) {
    idling = true;

  #if DEBUG
    Serial.println("Playing idle rhythm");
  #endif
  }
#endif

  if (idling) {
    if (rhythmBeatCount == 1 && rhythmChangeTimeDelay.delayPassed()) {
#if !RHYTHM_OVERRIDE
      const RhythmData* newRhythmData;
      do {
        newRhythmData = RhythmDatas[random(0, sizeof(RhythmDatas) / sizeof(const RhythmData*))];
      } while (newRhythmData->name == rhythmData->name);

      rhythmData = newRhythmData;
#else
      rhythmData = RhythmDatas[RHYTHM_OVERRIDE - 1];

      Serial.printf("Overriding rhythm with: %s\n", rhythmData->name);
#endif

#if APP_DEBUG
      Serial.printf("Changing idle rhythm: %s, BPM: %d\n", rhythmData->name, rhythmData->bpm);
#endif
    }

    rhythmController.play(rhythmData, rhythmBeatCount);
  }

  microphoneHelper.getBands(bands, idling);

  const int beatInt = bands.getBeat(idling);
  if (beatInt != -1) {
    ledController.on(static_cast<Beat>(beatInt));

    // on each beat restart the time delay
    idleRhythmTimeDelay.restart();

    if (idling) {
#if APP_DEBUG
      Serial.println("Stopping idle rhythm");
#endif

      // reset only once
      idling = false;
      rhythmBeatCount = 1;
    }
  } else if (!idling) {
    ledController.off(Dum);
    ledController.off(Tek);
  }

#if APP_CHART
  webSocketsServer.loop();

  webSocketsServer.broadcastTXT(bands.chartJson().c_str());
#endif
}
