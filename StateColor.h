#ifndef StateColor_h
#define StateColor_h

class CStateColor : public IState
{
private:
    CRGB color;
    byte phase;

public:
    /**
     * @param base the base color of the sparkling dots and the background
     */
    CStateColor(CRGB c)
    {
        color = c;
    }
    byte getType() 
    {
        return STATE_COLOR;
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
        }
        CStateColor::phase = phase;
    }

    byte live() 
    {
    }
    
    void drawBackground(CLedMatrix &m, IState* old_state)
    {
        m.setColor(color);
    }
    void drawParticles(CLedMatrix &m)
    {
    }
};

#endif
