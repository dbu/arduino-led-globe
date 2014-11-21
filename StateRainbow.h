#ifndef StateRainbow_h
#define StateRainbow_h

#ifdef MATRIX
#define DIVISOR float(LENGTH)
#else
#define DIVISOR 20.0
#endif

class CStateRainbow : public IState
{
private:
    byte phase;
    byte age;
    byte movein;

public:
    /**
     * @param base the base color of the sparkling dots and the background
     */
    CStateRainbow(boolean dummy)
    {
    }
    byte getType() 
    {
        return STATE_RAINBOW;
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
        CStateRainbow::phase = phase;
    }
    byte live() 
    {   
        if (PHASE_INTRO == phase) {
            #ifdef MATRIX
                movein += 1;
            #else
                movein += 3;
            #endif
            if (movein >= LENGTH) {
                phase = PHASE_MAIN;
            }
        }
        if (++age > DIVISOR*2) {
            age = 0;
        }
        return phase;
    }
    
    void drawRainbow(CLedMatrix &m)
    {
        for (byte s = 0; s<STRIPS; s++) {
            for (byte l = 0; l < min(movein, LENGTH); l++) {
                m.set(s, l, HSVtoRGB(6 * fmod((age*.5+LENGTH-l-1)/DIVISOR, 1), 1, 1));
            }
        }
    }
    
    void drawBackground(CLedMatrix &m, IState* old_state)
    {
        if (PHASE_FADEOUT == phase) {
            drawRainbow(m);
        }
    }
    void drawParticles(CLedMatrix &m)
    {
        if (PHASE_INTRO == phase || PHASE_MAIN == phase) {
            drawRainbow(m);
        }
    }
    
    /**
     * found somewhere on the internets
     *
     * @param h the hue, in pseudo-radians from 0-6
     * @param s saturation
     * @param v value (dark to light)
     */
    static CRGB HSVtoRGB(float h, float s, float v)
    {
        // H is given on [0->6] or -1. S and V are given on [0->1].
        float f;
        int i, m, n;

        CRGB rgb;

        if (h == -1)
        {
            rgb.r = rgb.g = rgb.b = int(h);
            return rgb;
        }
        i = (int) (floor(h));
        f = h - i;
        if (i % 2 == 0)
        {
            f = 1 - f; // if i is even
        }
        m = int(v * (1 - s) * 255);
        n = int(v * (1 - s * f) * 255);
        int v1 = int(v*255);
        switch (i)
        {
            case 6:
            case 0:
                rgb.r = int(v1);
                rgb.g = int(n);
                rgb.b = int(m);
                break;
            case 1:
                rgb.r = int(n);
                rgb.g = int(v1);
                rgb.b = int(m);
                break;
            case 2:
                rgb.r = int(m);
                rgb.g = int(v1);
                rgb.b = int(n);
                break;
            case 3:
                rgb.r = int(m);
                rgb.g = int(n);
                rgb.b = int(v1);
                break;
            case 4:
                rgb.r = int(n);
                rgb.g = int(m);
                rgb.b = int(v1);
                break;
            case 5:
                rgb.r = int(v1);
                rgb.g = int(m);
                rgb.b = int(n);
                break;
        }

        return rgb;
    }
};

#endif
