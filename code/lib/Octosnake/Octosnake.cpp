#include "Octosnake.h"

Oscillator::Oscillator(){
    _period = 2000;
    _amplitude = 50;
    _phase = 0;
    _offset = 0;
    _stop = true;
    _ref_time = micros();
    _delta_time = 0;
    _output = 90;
}

float Oscillator::refresh(){
    if (!_stop){
        float elapsed = (float)(micros() - _ref_time);
        _delta_time = elapsed - _period * floor(elapsed / _period);
        _output =   (float)_amplitude*sin(time_to_radians(_delta_time)
                    + degrees_to_radians(_phase))
                    + _offset;
    }

    return _output;
}

void Oscillator::reset(){
    _ref_time = micros();
}

void Oscillator::start(){
    reset();
    _stop = false;
}

void Oscillator::start(unsigned long ref_time){
    _ref_time = ref_time;
    _stop = false;
}

void Oscillator::stop(){
    _stop = true;
}

void Oscillator::setPeriod(float period){
    _period = period*1000; // Convert seconds to milliseconds
}

void Oscillator::setAmplitude(float amplitude){
    _amplitude = amplitude;
}

void Oscillator::setPhase(float phase){
    _phase = phase;
}

float Oscillator::getPhase(){
    return _phase;
}

void Oscillator::setOffset(float offset){
    _offset = offset;
}

void Oscillator::setTime(unsigned long ref){
    _ref_time = ref;
}

float Oscillator::getOutput(){
    return _output;
}

unsigned long Oscillator::getTime(){
    return _ref_time;
}

float Oscillator::getPhaseProgress(){
    float elapsed = (float)(micros() - _ref_time);
    _delta_time = elapsed - _period * floor(elapsed / _period);
    return ((float)_delta_time/_period) * 360;
}

float Oscillator::time_to_radians(double time){
    return time*2*PI/_period;
}

float Oscillator::degrees_to_radians(float degrees){
    return degrees*2*PI/360;
}

float Oscillator::degrees_to_time(float degrees){
    return degrees*_period/360;
}
