#ifndef octosnake_h
#define octosnake_h

#include <Arduino.h>

#ifndef PI
  #define PI 3.14159
#endif


class Oscillator{

    public:
        Oscillator();
        float refresh();
        void reset();
        void start();
        void start(unsigned long ref_time);
        void stop();
        float time_to_radians(double time);
        float degrees_to_radians(float degrees);
        float degrees_to_time(float degrees);
        void setPeriod(float period);
        void setAmplitude(float amplitude);
        void setPhase(float phase);
        float getPhase();
        void setOffset(float offset);
        void setTime(unsigned long ref);
        float getOutput();
        float getPhaseProgress();
        unsigned long getTime();

    private:
        float _period;
        float _amplitude;
        float _phase;
        float _offset;
        float _output;
        bool _stop;
        unsigned long _ref_time = 0;
        float _delta_time = 0;
};

#endif
