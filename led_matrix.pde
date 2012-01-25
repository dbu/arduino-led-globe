/**
 * 5V (black) = 5 volts
 * CK (red or green)   = SPI clock, pin 13
 * SD (green or red) = SPI data (master to slave, MOSI), pin 11
 * GND (blue) = Ground
 *
 *
 * various states, one is used as main state.
 *
 * each state has 3 phases: intro, main, fadeout
 *
 * fade over between states: while current_state is in phase intro, just draw particles.
 * once its in main, draw both particle and background.
 */

//#define DEBUG
//#include <DebugUtils.h>

#include <FastSPI_LED.h>
#include <LedMatrix.h>
#include "State.h"

/*** configuration ***/
// strip length
#define LENGTH 16
// number of strips
#define STRIPS 10

#define SENSOR_PIN A0

/*** general variables ***/
CLedMatrix LedMatrix;
struct CRGB *leds;

#define STATE_FLARE 1
#define STATE_FLARE_UP 2
#define STATE_GLIMMER 4
#define STATE_COLOR 99

#define PHASE_INTRO 1
#define PHASE_MAIN 2
#define PHASE_FADEOUT 3
#define PHASE_DONE 4
#define PHASE_EXTRA 5

IState* current_state, *extra_state, *old_state, *old_extra_state;


unsigned long state_start;
unsigned int state_duration = 45*1000; //could be controlled
unsigned long frame_start;
/** how long one frame should take */
byte frame_duration = 30;


/*** flares *****/
#include "StateFlare.h"
#include "StateFlareup.h"

#define MAIN_FLARES_COUNT 7
#define EXTRA_FLARES_COUNT 5

CParticleFlare MainFlares[MAIN_FLARES_COUNT];
CParticleFlare ExtraFlares[EXTRA_FLARES_COUNT];

CStateFlareup StateFlareup((CRGB) {255, 0, 46}, MainFlares, MAIN_FLARES_COUNT);
CStateFlareup StateFlareupExtra((CRGB) {255, 207, 0}, ExtraFlares, EXTRA_FLARES_COUNT);

CStateFlare StateFlare((CRGB) {0, 0, 255}, MainFlares, MAIN_FLARES_COUNT);
CStateFlare StateFlareExtra((CRGB) {50, 0, 255}, ExtraFlares, EXTRA_FLARES_COUNT);

/*** glimmers ***/
#include "StateGlimmer.h"

#define MAIN_GLIMMERS_COUNT (LENGTH*STRIPS/6)
#define EXTRA_GLIMMERS_COUNT (LENGTH*STRIPS/4)

SParticleGlimmer MainGlimmers[MAIN_GLIMMERS_COUNT];
SParticleGlimmer ExtraGlimmers[EXTRA_GLIMMERS_COUNT];

CStateGlimmer StateGlimmer((CRGB) {255, 255, 255}, MainGlimmers, MAIN_GLIMMERS_COUNT);
CStateGlimmer StateGlimmerExtra((CRGB) {255, 0, 255}, ExtraGlimmers, EXTRA_GLIMMERS_COUNT);

CStateGlimmer StateGreen((CRGB) {12, 210, 16}, MainGlimmers, 0);
CStateGlimmer StateGreenExtra((CRGB) {15, 235, 210}, MainGlimmers, MAIN_GLIMMERS_COUNT);

/*** static color ***/
#include "StateColor.h"
CStateColor StateColor((CRGB) {0, 255, 0});

void setup()
{
//    Serial.begin(9600);

    LedMatrix.init(LENGTH, STRIPS, true);
    FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2801);

    // needed in addition to template. <2 fucks up
    FastSPI_LED.setDataRate(2);

    FastSPI_LED.init();
    FastSPI_LED.start();

    leds = (struct CRGB*)FastSPI_LED.getRGBData();
    memset(leds, 0, LENGTH*STRIPS*sizeof(CRGB));

    state_start = millis();

    current_state = & StateGreen;
    current_state->setPhase(PHASE_INTRO);

    extra_state = & StateGreenExtra;
    extra_state->setPhase(PHASE_INTRO);
    
    //Serial.print("freeMemory()=");
    //Serial.println(freeMemory());
}

void loop() {
    frame_start = millis();
    readDuration();
    if (NULL != old_state) {
        if (PHASE_DONE == old_state->live()) {
            old_state = NULL;
        }
    }
    if (NULL != old_extra_state) {
        if (PHASE_DONE == old_extra_state->live()) {
            old_extra_state = NULL;
        }
    }
    current_state->live();
    current_state->drawBackground(LedMatrix, old_state);

    if (NULL != old_state) {
        old_state->drawParticles(LedMatrix);
    }
    if (NULL != old_extra_state) {
        old_extra_state->drawParticles(LedMatrix);
    }
    
    if (current_state->getPhase() == PHASE_MAIN || current_state->getPhase() == PHASE_FADEOUT) {
        if (NULL != extra_state) {
            extra_state->live();
            extra_state->drawParticles(LedMatrix);
        }
    }

    current_state->drawParticles(LedMatrix);

    FastSPI_LED.show();
    
    stateCheck();

    if (millis() - frame_start < frame_duration) {
        delay(frame_duration - (millis() - frame_start));
    }
}

/**
 * check how long we are running and change state if necessary
 */
void stateCheck()
{
     if (current_state->getPhase() == PHASE_MAIN && millis() - state_start > state_duration) {
        current_state->setPhase(PHASE_FADEOUT);
        if (extra_state != NULL) {
            extra_state->setPhase(PHASE_FADEOUT);
        }
        state_start = millis();
        if (&StateGreen == current_state) {
            old_state = current_state;
            old_extra_state = extra_state;
            current_state = &StateFlareup;
            extra_state = &StateFlareupExtra;
        } else if (&StateFlareup == current_state) {
            old_state = current_state;
            old_extra_state = extra_state;
            extra_state = &StateGlimmerExtra;
            current_state = &StateGlimmer;
        } else if (&StateGlimmer == current_state) {
            old_state = current_state;
            old_extra_state = extra_state;
            current_state = &StateFlare;
            extra_state = &StateFlareExtra;
        } else if (&StateFlare == current_state) {
            old_state = current_state;
            old_extra_state = extra_state;
            current_state = &StateGreen;
            extra_state = &StateGreenExtra;
        }
        current_state->setPhase(PHASE_INTRO);
        if (extra_state != NULL) extra_state->setPhase(PHASE_EXTRA);
        
    }
    if (NULL != old_state && old_state->getPhase() == PHASE_DONE) {
        old_state = NULL;
    }
    if (NULL != old_extra_state && old_extra_state->getPhase() == PHASE_DONE) {
        old_extra_state = NULL;
    }


}

/**
 * cheap shuffling of the values in this array
 */
void shuffle(byte* arr, byte len)
{
    for (byte i = 0; i < len; i++) {
        byte change = i + byte(random(0, len - i)); //this might be stupid but we don't need high quality shuffle
        byte h = arr[i];
        arr[i] = arr[change];
        arr[change] = h;
    }
}

/**
 * Read the potentiometer to know duration
 */
void readDuration()
{
    // analog pin measures 0 - 1023 => map to 2 - 80
    frame_duration = 2 + analogRead(SENSOR_PIN) / 12.8;
}

/** seems we can mess up function stack, try to print each loop something 
void heartbeat()
{
//    DEBUG_PRINT("Babump");
}
*/
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int freeMemory() {
  int free_memory;

  if((int)__brkval == 0)
     free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}

