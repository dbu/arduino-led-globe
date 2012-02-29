#ifndef StateBlink_h
#define StateBlink_h

class CStateBlink : public IState
{
private:
    byte phase;
    byte age;

public:
    CStateBlink(CRGB color)
    {
    }
    byte getType() 
    {
        return STATE_BLINK;
    }
    /**
     * @return constant telling which phase this state is in, one of the PHASE_ constants
     */
    byte getPhase()
    {
        return phase;
    }
  
    void setPhase(byte phase)
    {
        if (phase == PHASE_INTRO) {
            phase = PHASE_MAIN;
            age = 0;
        } else if (phase == PHASE_FADEOUT) {
            phase = PHASE_DONE;
        }
        CStateBlink::phase = phase;
    }

    byte live() 
    {
        if (++age == 6) age = 0;
    }
    
    void drawBackground(CLedMatrix &m, IState* old_state)
    {
        byte pixels = STRIPS*LENGTH;
        memset(leds, 0, pixels * 3);
        byte shift = age % 2;
        byte pos = age / 2;
        for(int i = 0; i < pixels; i++ ) {
            if (shift && i == pixels-2) {
                switch((i+pos) % 3) {
                  case 0: leds[pixels-1].r = 100; leds[0].r = 100; break;
                  case 1: leds[pixels-1].b = 100; leds[0].b = 100; break;
                  case 2: leds[pixels-1].g = 100; leds[0].g = 100; break;
                }
            } else {
                switch((i+pos) % 3) {
                  case 0: leds[shift+i++].r = 100; leds[shift+i].r = 100; break;
                  case 1: leds[shift+i++].b = 100; leds[shift+i].b = 100; break;
                  case 2: leds[shift+i++].g = 100; leds[shift+i].g = 100; break;
                }
            }
        }
    }
    
    void drawParticles(CLedMatrix &m)
    {
    }
};

#endif
