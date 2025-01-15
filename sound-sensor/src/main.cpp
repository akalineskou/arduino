#include "Beat.h"
#include "Directive.h"
#include "LedController.h"
#include "MicrophoneHelper.h"
#include "Rhythm.h"
#include "RhythmController.h"
#include "TimeDelay.h"

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
bool rhythmPlay = false;
const RhythmData* rhythmData = RhythmDatas[0];

TimeDelay idleRhythmTimeDelay(60 * 1000, true);
TimeDelay rhythmChangeTimeDelay(5 * 1000, true);

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
  if (!rhythmPlay && idleRhythmTimeDelay.delayPassed()) {
    rhythmPlay = true;

#if DEBUG
    Serial.println("Playing idle rhythm");
#endif
  }

  if (rhythmPlay) {
    if (rhythmBeatCount == 1 && rhythmChangeTimeDelay.delayPassed()) {
      const RhythmData* newRhythmData;
      do {
        newRhythmData = RhythmDatas[random(0, sizeof(RhythmDatas) / sizeof(const RhythmData*))];
      } while (newRhythmData->name == rhythmData->name);

      rhythmData = newRhythmData;

#if DEBUG
      Serial.printf("Changing idle rhythm: %s, BPM: %d\n", rhythmData->name, rhythmData->bpm);
#endif
    }

    rhythmController.play(rhythmData, rhythmBeatCount);
  }

  microphoneHelper.getBands(bands);

  for (const auto beat: {Dum, Tek}) {
    if (bands.is(beat)) {
      ledController.on(beat);

      // on each beat restart the time delay
      idleRhythmTimeDelay.restart();

      if (rhythmPlay) {
        // reset only once
#if DEBUG
        Serial.println("Stopping idle rhythm");
#endif

        rhythmPlay = false;
        rhythmBeatCount = 1;
      }
    } else if (!rhythmPlay) {
      ledController.off(beat);
    }
  }

#if CHART
  webSocketsServer.loop();

  webSocketsServer.broadcastTXT(bands.chartJson().c_str());
#endif
}
