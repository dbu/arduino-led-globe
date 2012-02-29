#ifndef StateBlink_h
#define StateBlink_h

class CStateBlink : public IState
{
private:
    byte phase;
    byte age;
    boolean step;
    CRGB color;

public:
    CStateBlink(CRGB color)
    {
        CStateBlink::color = color;
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
        if (++age >= 4) {
            age = 0;
            step = ! step;
        }
        return phase;
    }
    
    void drawBackground(CLedMatrix &m, IState* old_state)
    {
        byte pixels = STRIPS*LENGTH;
        memset(leds, 0, pixels * 3);
        byte shift = step % 8;
        byte pos = age / 8;
        for(int s = 0; s < STRIPS; s++ ) {
            for (int l = 0; l < LENGTH; l++) {
                if ((s*LENGTH+l+step) % 2 == 0) {
                    m.set(s, l, (CRGB) {220, 15, 149});
                } else {
                    m.set(s, l, color);
                }
            }
        }
    }
    
    void drawParticles(CLedMatrix &m)
    {
    }
};

#endif
