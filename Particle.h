#ifndef Particle_h
#define Particle_h

#include <LedMatrix.h>

/**
 * a real particle will also have an init function with particle specific
 * parameters
 */
class IParticle
{
public:
    /**
     * @return whether this particle is alive
     */
    virtual bool alive();
    /**
     * @return whether this particle is still alive or not
     */
    virtual bool live();
    /**
     * force this particle to die, give it a chance to
     * do last cleanup on the next draw.
     */
    virtual void die();
    virtual void draw(CLedMatrix &m);
};

#endif
