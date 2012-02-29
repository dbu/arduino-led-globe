#ifndef StateFlareup_h
#define StateFlareup_h

#define FLARE_COLOR_FACTOR 0.05

#include "StateFlare.h"

class CStateFlareup : public CStateFlare
{
    public:
      CStateFlareup(CRGB base, CParticleFlare* flares, byte flares_count) : CStateFlare(base, flares, flares_count)
      {
          // constructors are not inherited...
      }

      byte getType()
      {
          return STATE_FLARE_UP;
      }
      boolean isUp()
      {
        #ifdef MATRIX
        return true;
        #else
        return random(2); //0 or 1
        #endif
      }

};

#endif
