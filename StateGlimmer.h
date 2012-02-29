#ifndef StateGlimmer_h
#define StateGlimmer_h

#include <limits.h>
#include <LedMatrix.h>

// how many frames until fully faded in
#define GLIMMER_FULL 100
// fraction of base color used for background
#define GLIMMER_BASE_FACTOR 0.05

// time from one color to the next
#define GLIMMER_SPEED 30.0
// number of times the same pixel takes a new direction
#define GLIMMER_MAX_AGE 5

struct SParticleGlimmer {
    byte pos;
    byte progress;
    byte age;
    // factor of base color to start + end
    float source, target;
};

class CStateGlimmer : public IState
{
private:
    CRGB base;
    CRGB bgcolor;
    float maximum;
    SParticleGlimmer* particles;
    byte particles_count;
    byte phase;
    short fade;

public:
    /**
     * @param base the base color of the sparkling dots and the background
     */
    CStateGlimmer(CRGB base, SParticleGlimmer* particles, byte particles_count)
    {
        CStateGlimmer::base = base;
        bgcolor.r = base.r * GLIMMER_BASE_FACTOR; 
        bgcolor.g = base.g * GLIMMER_BASE_FACTOR; 
        bgcolor.b = base.b * GLIMMER_BASE_FACTOR;
        
        maximum = 255.0 / float(max(base.r, max(base.g, base.b)));
        
        CStateGlimmer::particles = particles;
        CStateGlimmer::particles_count = particles_count;
        for (byte i = 0; i<particles_count; i++) {
          particles[i].age = GLIMMER_MAX_AGE;
        }
    }

    byte getType()
    {
        return STATE_GLIMMER;
    }
    byte getPhase()
    {
        return phase;
    }
    void setPhase(byte phase)
    {
        if (phase == PHASE_INTRO) {
            fade = 0;
        } else if (phase == PHASE_FADEOUT) {
            if (PHASE_EXTRA == CStateGlimmer::phase) {
                fade = 0;
            } else {
                fade = GLIMMER_FULL;
            }
            for (byte i=0; i < particles_count; i++) {
                particleDie(particles[i]);
            }
        }
        CStateGlimmer::phase = phase;
    }

    byte live()
    {
        if (phase == PHASE_INTRO) {
            if (++fade == GLIMMER_FULL) {
                phase = PHASE_MAIN;
            }
        } else if (phase == PHASE_FADEOUT) {
            --fade;
            // state change to happen below, to wait for pixels fading away
        }
        bool alive = false;

        for (byte i = 0; i < particles_count; i++) {
            if (! particleLive(particles[i])) {
                if (phase != PHASE_FADEOUT && phase != PHASE_DONE && phase != PHASE_INTRO) {
                    if (random(10) == 1) {
                        CRGB c;
                        byte pos = random(0, STRIPS*LENGTH);
                        boolean isfree = true;
                        for (byte n = 0; n < particles_count; n++) {
                            if (particleLive(particles[i]) && particles[i].pos == pos) {
                                isfree = false;
                            }
                        }
                        if (isfree) {
                            particleInit(particles[i], pos);
                        }
                    }
                }
            } else {
                alive = true;
            }
        }

        if (PHASE_FADEOUT == phase && fade <= 0 && ! alive) {
            phase = PHASE_DONE;
        }
        return phase;
    }

    /**
     * Do state level drawing. The particles are drawn elsewhere
     */
    void drawBackground(CLedMatrix &m, IState* old_state)
    {
        if (PHASE_DONE == phase) return;
        if ((PHASE_INTRO == phase) && NULL != old_state) {
            old_state->drawBackground(m, NULL);
        } else {
            memset(leds, 0, STRIPS*LENGTH * 3);
        }

        if (PHASE_INTRO == phase || PHASE_FADEOUT == phase) {
            if (fade <= 0) return;

            for(byte i=0; i<STRIPS; i++) {
                for (byte j=0; j<LENGTH; j++) {
                    if (PHASE_INTRO == phase) {
                        m.blend(i, j, bgcolor, fade / float(GLIMMER_FULL));
                    } else {
                        // FADEOUT
                        m.blend(i, j, (CRGB){0,0,0}, (GLIMMER_FULL - fade) / float(GLIMMER_FULL));
                    }
                }
            }
        } else if (PHASE_MAIN == phase) {
            m.setColor(bgcolor);
        }
    }

    /**
     * Draw living particles
     */
    void drawParticles(CLedMatrix &m)
    {
        for (byte i=0; i<particles_count; i++) {
            if (particles[i].age < GLIMMER_MAX_AGE) {
                particleDraw(particles[i]);
            }
        }
    }
    
    ///// methods to operate the particles //////
    
    void particleInit(SParticleGlimmer& p, byte pos)
    {
        p.pos = pos;
        p.target = GLIMMER_BASE_FACTOR;
        p.source = 0;
        p.age = 0;
        p.progress = GLIMMER_SPEED; // trigger new direction in next live()
    }
    
    bool particleLive(SParticleGlimmer& p)
    {
        if (p.age >= GLIMMER_MAX_AGE) {
            return false;
        }
        if (++p.progress > GLIMMER_SPEED) {
            p.progress = 0;
            p.age++;
            p.source = p.target;
            p.target = (p.age+1==GLIMMER_MAX_AGE) ? GLIMMER_BASE_FACTOR : random(0, maximum*100)/100.0;
        }
        return p.age < GLIMMER_MAX_AGE;
    }
    
    void particleDraw(SParticleGlimmer& p)
    {
        if (p.age >= GLIMMER_MAX_AGE) return; // this particle is dead

        float factor = particleGetFactor(p.progress, p.source, p.target);
        CRGB n;
        n.r = int(base.r * factor);
        n.g = int(base.g * factor);
        n.b = int(base.b * factor);
        
        leds[p.pos] = n;
    }
    
    void particleDie(SParticleGlimmer& p) {
        p.age = GLIMMER_MAX_AGE - 1;
        p.source = particleGetFactor(p.progress, p.source, p.target);
        p.progress = 0;
        p.target = 0;
    }
    
    inline float particleGetFactor(float progress, float source, float target)
    {
        float pos = (float(progress) / GLIMMER_SPEED);
        return (1 - pos) * source + pos * target;
    }
};

#endif
