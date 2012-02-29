#ifndef StateBlink_h
#define StateBlink_h

class CStateBlink : public IState
{
private:
    byte phase;
    byte age;
    byte movein;
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
            age = 0;
            movein = 0;
        }
        CStateBlink::phase = phase;
    }

    byte live() 
    {
        if (PHASE_INTRO == phase) {
            movein += 4;
            if (movein >= LENGTH) {
                phase = PHASE_MAIN;
            }
        }
        if (++age >= 5) {
            age = 0;
            step = ! step;
        }
        return phase;
    }
    
    void drawBlink(CLedMatrix &m)
    {
        byte pixels = STRIPS*LENGTH;
        byte shift = step % 8;
        byte pos = age / 8;
        for(int s = 0; s < STRIPS; s++ ) {
            for (int l = 0; l < min(movein, LENGTH); l++) {
                if ((s*LENGTH+l+step) % 2 == 0) {
                    m.set(s, l, (CRGB) {255, 15, 49});
                } else {
                    m.set(s, l, color);
                }
            }
        }      
    }
    
    void drawBackground(CLedMatrix &m, IState* old_state)
    {
        if (PHASE_FADEOUT == phase) {
            drawBlink(m);
        }
    }
    
    void drawParticles(CLedMatrix &m)
    {
        if (PHASE_MAIN == phase || PHASE_INTRO == phase) {
            drawBlink(m);
        }
    }
};

#endif
