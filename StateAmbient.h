#ifndef StateAmbient_h
#define StateAmbient_h

#define AMBIENT_MAX_AGE 10000

class CStateAmbient : public IState
{
private:
    byte phase;
    int age;

public:
    CStateAmbient()
    {
        age = 0;
    }
    
    byte getType() 
    {
        return STATE_AMBIENT;
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
        CStateAmbient::phase = phase;
    }

    byte live() 
    {
        if (++age >  AMBIENT_MAX_AGE) age = 0;
    }
    
    void drawBackground(CLedMatrix &m, IState* old_state)
    {  
        CRGB color = CStateRainbow::HSVtoRGB(6 * (age/(float)AMBIENT_MAX_AGE), 10, 1); //analogRead(SPEED_PIN) / 1023.0, 1);
        
        m.setColor(color);
    }
    void drawParticles(CLedMatrix &m)
    {
    }
};

#endif
