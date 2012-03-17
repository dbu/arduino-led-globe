#ifndef ParticleFlare_h
#define ParticleFlare_h

#include <limits.h>
#include <LedMatrix.h>
#include "Particle.h"

#define POS(i) (up ? i : LENGTH - 1 - i)

class CParticleFlare : public IParticle
{

private:
    CParticleFlare(CParticleFlare& f); // avoid copy constructor

    CStateFlare* state;
    byte column;
    byte len;
    byte speed;
    boolean up;
    byte offset;

    int age;

public:
    CParticleFlare()
    {
        speed = 1;
        age = 32000;
    }
    /**
     * @param speed how many cycles to use to fade pixels in. the higher the speed number the slower the flare
     * @param up whether to animate the flare from high to low index
     */
    void init(CStateFlare *state, byte offset, byte column, byte flare_len, byte speed, boolean up)
    {
        CParticleFlare::offset = offset;
        CParticleFlare::column = column;
        CParticleFlare::state = state;
        CParticleFlare::speed = speed;
        len = flare_len;
        age = offset*speed;
        CParticleFlare::up = up;
    }
    bool alive()
    {
        return age/speed < LENGTH+len;
    }
    bool live()
    {
        if (! alive()) {
            return false;
        }
        age++;
        return alive();
    }
    void die()
    {
        age = speed * (LENGTH+len) + 1;
    }
    void draw(CLedMatrix &m)
    {
        if (! alive()) return; // this particle is dead

        byte pos = floor(age / speed);
        byte ease = age % speed;
        float fade = (float) ease/speed;

        byte rel = (max(0, len - pos));
        float step = 1.0/len;
        CRGB color;
        CRGB c = state->getColor();
        for (byte i = max(offset, pos - len); i < min(pos, LENGTH); i++) {
            float factor = ++rel*step - fade*step; // this could be a better function than just linear

            if (factor < FLARE_COLOR_FACTOR) {
                factor = FLARE_COLOR_FACTOR;
            }

            color.r = c.r*factor; color.g = c.g*factor; color.b = c.b*factor;
            if (state->isAdditive()) {
                m.brighten(column, POS(i), color);
            } else {
                m.set(column, POS(i), color);
            }
        }
        if (pos < LENGTH) {
            color.r = floor(c.r*fade); color.g = floor(c.g*fade); color.b = floor(c.b*fade);
            m.brighten(column, POS(pos), color);
        }
    }

};

#endif
