#ifndef State_h
#define State_h

class IState
{
private:
      IState(IState& f); //disable copy constructor
protected:
      IState() {} //but allow extending classes constructors
public:
      /**
       * @return constant identifying this state, one of the STATE_ constants
       */
      virtual byte getType() = 0;
      /**
       * @return constant telling which phase this state is in, one of the PHASE_ constants
       */
      virtual byte getPhase() = 0;
      /**
       * Tell the state to start a phase transition.
       * Maybe be used with PHASE_INTRO, PHASE_EXTRA or PHASE_FADEOUT
       * Its up to the state to decide when it switches from PHASE_INTRO to PHASE_MAIN and from PHASE_FADEOUT to PHASE_DONE
       */
      virtual void setPhase(byte phase) = 0;

      virtual byte live() = 0;
      /**
       * Do state level drawing. The particles are drawn elsewhere
       * @param old_state give the state a chance to let the old state draw if it wants to. might be NULL
       */
      virtual void drawBackground(CLedMatrix &m, IState* old_state) = 0;
      virtual void drawParticles(CLedMatrix &m) = 0;
};

#endif

