#ifndef StateLightning_h
#define StateLigthning_h

#define LIGHT_PROBABILITY 20
#define LIGHT_ROTATION 30.0

class CStateLightning : public IState
{
private:
    byte phase;
    byte age;
    CRGB color;

public:
    CStateLightning(CRGB color)
    {
        CStateLightning::color = color;
    }
    byte getType() 
    {
        return STATE_LIGHTNING;
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
            CStateLightning::phase = PHASE_MAIN;
        } else if (PHASE_FADEOUT == phase) {
            CStateLightning::phase = PHASE_DONE;
        } else {
            CStateLightning::phase = phase;
        }
    }

    byte live() 
    {
        if (++age > LIGHT_ROTATION*2) {
            age = 0;
        }
        
        return phase;
    }
        
    void drawLightning(CLedMatrix &m)
    {
        float sat[] = {0.2, 0.5, 0.8, 1.0};
        for(int s = 0; s < STRIPS; s++ ) {
            for (int l = 0; l < LENGTH; l++) {
                if (random(LIGHT_PROBABILITY) == 1) {
                    float value = 0.5 + random(100) / 200.0;
                    float saturation = sat[random(5)];
                    m.set(s, l, CStateRainbow::HSVtoRGB(6 * fmod((random(age-2, age+2)*.5)/LIGHT_ROTATION, 1), saturation, value));
                }
            }
        }
        delay(40);
    }
    
    void drawBackground(CLedMatrix &m, IState* old_state)
    {  
        if (PHASE_MAIN == phase) {
            m.setColor((CRGB) {0,0,0});
        }
    }
    
    void drawParticles(CLedMatrix &m)
    {
        if (PHASE_MAIN == phase) {
            drawLightning(m);
        }
    }
};

#endif
