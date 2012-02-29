#ifndef StateFlare_h
#define StateFlare_h

#define FLARE_COLOR_FACTOR 0.05

class CParticleFlare;

class CStateFlare : public IState 
{
private:
    CStateFlare(CStateFlare& f);
    CRGB base, bgcolor;
    CParticleFlare* flares;
    byte flares_count;
    byte phase;
    byte columns[STRIPS]; //used during init phase so every column is once visited
    byte columns_pos;
    CParticleFlare* toWatch;

public:
    CStateFlare(CRGB base, CParticleFlare* flares, byte flares_count);
    void setPhase(byte phase);
    byte getType();
    byte getPhase();
    byte live();
    /**
     * Do state level drawing.
     */
    void drawBackground(CLedMatrix &m, IState* old_state);
    void drawParticles(CLedMatrix &m);
    CRGB getColor();
    boolean isAdditive();
    virtual boolean isUp();
protected:
    void restart();
    void reset_flares();
};

#include "ParticleFlare.h"

    CStateFlare::CStateFlare(CRGB base, CParticleFlare* flares, byte flares_count)
    {
        CStateFlare::base = base;
        CStateFlare::flares = flares;
        CStateFlare::flares_count = flares_count;
        bgcolor.r = base.r*FLARE_COLOR_FACTOR; bgcolor.g = base.g*FLARE_COLOR_FACTOR; bgcolor.b = base.b*FLARE_COLOR_FACTOR;
    }
    void CStateFlare::setPhase(byte phase)
    {
        switch(phase) {
            case PHASE_INTRO:
                reset_flares();
                restart();
                break;
            case PHASE_EXTRA:
                reset_flares();
                break;
        }
        CStateFlare::phase = phase;
    }
    byte CStateFlare::getType()
    {
        return STATE_FLARE;
    }
    byte CStateFlare::getPhase()
    {
        return phase;
    }
    byte CStateFlare::live()
    {
        if (toWatch != NULL) {
            for (byte i = 0; i < flares_count; i++) {
                flares[i].live();
            }
            if (toWatch->alive()) {
                return phase;
            } else {
                phase = PHASE_MAIN;
                toWatch = NULL;
            }
        }


        bool alive = false; // to track when to switch from fadeout to done

        for (byte i = 0; i < flares_count; i++) {
            if (! flares[i].live()) {
                if (phase != PHASE_FADEOUT && random(50) == 1) {
                    byte col = 0;
                    switch(phase) {
                        case PHASE_INTRO:
                            col = columns[columns_pos++];
                            if (columns_pos == STRIPS) {
                                toWatch = &flares[i];
                                #ifdef MATRIX
                                toWatch->init(this, col, random(5,8), random(6,8), this->isUp());
                                #else
                                toWatch->init(this, col, 12, 1, this->isUp());
                                #endif
                                return phase;
                            }
                            break;
                        case PHASE_MAIN:
                        case PHASE_EXTRA:
                            col = random(0,STRIPS);
                            break;
                        case PHASE_FADEOUT:
                            continue; // go to next flare, never restart it
                        default:
                            return phase; // abort
                    }
                    flares[i].init(this, col, random(3,10), random(5,9), this->isUp()); // age ist maximal (16+9)*10 + 1 => 251 < 255 (/)
                    alive = true;
                }
            } else {
                //found a living particle
                alive = true;
            }
        }
        if (phase == PHASE_FADEOUT && !alive) {
            phase = PHASE_DONE;
        }

        return phase;
    }
    /**
     * Do state level drawing.
     */
    void CStateFlare::drawBackground(CLedMatrix &m, IState* old_state)
    {
        if (PHASE_INTRO == phase) return;
        if (NULL != old_state) old_state->drawBackground(m, NULL);

        m.setColor(bgcolor);
    }

    void CStateFlare::drawParticles(CLedMatrix &m)
    {
        for (byte i = 0; i < flares_count; i++) {
            if (flares[i].alive()) {
                flares[i].draw(m);
            }
        }
    }
    CRGB CStateFlare::getColor()
    {
        return base;
    }
    boolean CStateFlare::isAdditive()
    {
        return PHASE_MAIN == phase || PHASE_EXTRA == phase;
    }
    boolean CStateFlare::isUp()
    {
        #ifdef MATRIX
        return false;
        #else
        return random(2); //0 or 1
        #endif
    }
    
    void CStateFlare::restart()
    {
        phase = PHASE_INTRO;
        for (byte i = 0; i<STRIPS; i++) {
            columns[i] = i;
        }
        shuffle(columns, STRIPS);
        columns_pos = 0;
        toWatch = NULL;
    }
    void CStateFlare::reset_flares()
    {
        for (byte i = 0; i < flares_count; i++) {
            flares[i].die();
        }
    }

#endif

