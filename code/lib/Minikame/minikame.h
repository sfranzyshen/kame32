#ifndef minikame_h
#define minikame_h

#include <Octosnake.h>
#include <ArduinoNvs.h>

#define SERVO_0_PIN         26
#define SERVO_1_PIN         25
#define SERVO_2_PIN         18
#define SERVO_3_PIN         19
#define SERVO_4_PIN         32
#define SERVO_5_PIN         33
#define SERVO_6_PIN         5
#define SERVO_7_PIN         21

#define MIN_PWM_DUTY        1638
#define MAX_PWM_DUTY        8191

class MiniKame{
public:
    void init();

    void setCalibration(int calibration[8]);
    int* loadCalibration();
    void saveCalibration(int calibration[8]);
    void reverseServo(int id);

    void setServo(int id, float target);
    float getServo(int id);
    void moveServos(int time, float target[8]);

    void run(float steps, int period);
    void walk(float steps, int period);
    void backward(float steps, int period);
    void omniWalk(float steps, int T);
    void turnL(float steps, int period);
    void turnR(float steps, int period);
    void moonwalkL(float steps, int period);
    void dance(float steps, int period);
    void upDown(float steps, int period);
    void pushUp(float steps, int period);
    void hello();
    void jump();
    void home();
    void zero();
    void frontBack(float steps, int period);

//private:
    Oscillator oscillator[8];
    int board_pins[8];
    int calibration[8];
    bool reverse[8];
    unsigned long _init_time;
    unsigned long _final_time;
    unsigned long _partial_time;
    float _increment[8];
    float _servo_position[8];

    int angToUsec(float value);
    void execute(float steps, int period[8], int amplitude[8], int offset[8], int phase[8]);
};

#endif
