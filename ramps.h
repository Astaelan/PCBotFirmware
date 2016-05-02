/* 
 * File:   ramps.h
 * Author: arsi
 *
 * Created on September 1, 2014, 4:44 PM
 */
#ifndef RAMPS_H
#define	RAMPS_H


#ifdef	__cplusplus
extern "C" {
#endif

#include"cpu_map.h"
#include<stdint.h>
#include"fastio.h"
#include"gcode.h"

#define CHECK(var,pos) ((var) & (1<<(pos)))

    extern void rampsInitCoolant();
    extern void rampsInitSpindle();
    extern void rampsInitSteppers();
    extern void rampsWriteDisable(uint8_t value);
    extern void rampsWriteSteps(uint8_t value);
    extern void rampsWriteDirections(uint8_t value);
    extern void rampsStartSpindle(uint8_t direction, float rpm);
    extern void rampsStopSpindle();
    extern void rampsLeftSpindle();
    extern void rampsRightSpindle();
    extern void rampsCoolantOn();
    extern void rampsCoolantOff();

#ifdef SPINDLE_Y3675
    uint8_t spindleDirection = SPINDLE_ENABLE_CW;
    uint8_t spindleDuty = 0;
#endif

    inline void rampsCoolantOn() {
        WRITE(COOLANT_FLOOD_PIN, 1);
    }

    inline void rampsCoolantOff() {
        WRITE(COOLANT_FLOOD_PIN, 0);
    }

    inline void rampsLeftSpindle() {
#ifndef SPINDLE_Y3675
        WRITE(SPINDLE_DIRECTION_PIN, 1);
#endif
    }

    inline void rampsRightSpindle() {
#ifndef SPINDLE_Y3675
        WRITE(SPINDLE_DIRECTION_PIN, 0);
#endif
    }

    inline void rampsStartSpindle(uint8_t direction, float rpm) {
#ifdef SPINDLE_Y3675
        if (rpm > SPINDLE_MAX_RPM) rpm = SPINDLE_MAX_RPM;
        uint8_t directionChanged = direction != spindleDirection;
        uint8_t duty = (rpm / (SPINDLE_MAX_RPM / 10)) + 0.5;
        int8_t dutyChange = duty - spindleDuty;

        spindleDirection = direction;
        spindleDuty = duty;

        if (directionChanged || spindleDuty == 0)
        {
          WRITE(SPINDLE_STOP_PIN, 0);
          delay_ms(100);
          WRITE(SPINDLE_STOP_PIN, 1);
          delay_ms(100);

          dutyChange = spindleDuty;
        }

        for (int8_t i = 0; i < dutyChange; ++i)
        {
          if (spindleDirection == SPINDLE_ENABLE_CW) WRITE(SPINDLE_UP_PIN, 0);
          else WRITE(SPINDLE_DOWN_PIN, 0);
          delay_ms(100);
          if (spindleDirection == SPINDLE_ENABLE_CW) WRITE(SPINDLE_UP_PIN, 1);
          else WRITE(SPINDLE_DOWN_PIN, 1);
          delay_ms(100);
        }
        for (int8_t i = dutyChange; i < 0; ++i)
        {
          if (spindleDirection == SPINDLE_ENABLE_CW) WRITE(SPINDLE_DOWN_PIN, 0);
          else WRITE(SPINDLE_UP_PIN, 0);
          delay_ms(100);
          if (spindleDirection == SPINDLE_ENABLE_CW) WRITE(SPINDLE_DOWN_PIN, 1);
          else WRITE(SPINDLE_UP_PIN, 1);
          delay_ms(100);
        }

#else
        WRITE(SPINDLE_ENABLE_PIN, 1);
#endif
    }

    inline void rampsStopSpindle() {
#ifdef SPINDLE_Y3675
        WRITE(SPINDLE_STOP_PIN, 0);
        delay_ms(100);
        WRITE(SPINDLE_STOP_PIN, 1);
        delay_ms(100);

        spindleDirection = SPINDLE_ENABLE_CW;
        spindleDuty = 0;
#else
        WRITE(SPINDLE_ENABLE_PIN, 0);
#endif
    }

    inline void rampsInitCoolant() {
        SET_OUTPUT(COOLANT_FLOOD_PIN);
    }

    inline void rampsInitSpindle() {
#ifdef SPINDLE_Y3675
        SET_OUTPUT(SPINDLE_STOP_PIN);
        SET_OUTPUT(SPINDLE_UP_PIN);
        SET_OUTPUT(SPINDLE_DOWN_PIN);

        WRITE(SPINDLE_UP_PIN, 1);
        WRITE(SPINDLE_DOWN_PIN, 1);
#else
        SET_OUTPUT(SPINDLE_ENABLE_PIN);
        SET_OUTPUT(SPINDLE_DIRECTION_PIN);
#endif
    }

/**
     * Perform port direction init for ramps for steppers
     */
    inline void rampsInitSteppers() {
        SET_OUTPUT(X_STEP_PIN);
        SET_OUTPUT(Y_STEP_PIN);
        SET_OUTPUT(Z_STEP_PIN);
        SET_OUTPUT(X_DIR_PIN);
        SET_OUTPUT(Y_DIR_PIN);
        SET_OUTPUT(Z_DIR_PIN);
        SET_OUTPUT(X_ENABLE_PIN);
        SET_OUTPUT(Y_ENABLE_PIN);
        SET_OUTPUT(Z_ENABLE_PIN);
    }

/**
     * Perform set of EN driver signal
     * @param value
     */
    inline void rampsWriteDisable(uint8_t value) {
        if (CHECK(value, STEPPERS_DISABLE_BIT)) {
            WRITE(X_ENABLE_PIN, 1);
            WRITE(Y_ENABLE_PIN, 1);
            WRITE(X_ENABLE_PIN, 1);
        } else {
            WRITE(X_ENABLE_PIN, 0);
            WRITE(Y_ENABLE_PIN, 0);
            WRITE(X_ENABLE_PIN, 0);
        }
    }

/**
     * write stepper pulse
     * @param value
     */
    inline void rampsWriteSteps(uint8_t value) {
        if (CHECK(value, X_STEP_BIT)) {
            WRITE(X_STEP_PIN, 1);
        } else {
            WRITE(X_STEP_PIN, 0);
        }
        if (CHECK(value, Y_STEP_BIT)) {
            WRITE(Y_STEP_PIN, 1);
        } else {
            WRITE(Y_STEP_PIN, 0);
        }
        if (CHECK(value, Z_STEP_BIT)) {
            WRITE(Z_STEP_PIN, 1);
        } else {
            WRITE(Z_STEP_PIN, 0);
        }

    }

/**
     * set stepper direction
     * @param value
     */
    inline void rampsWriteDirections(uint8_t value) {
        if (CHECK(value, X_DIRECTION_BIT)) {
            WRITE(X_DIR_PIN, 1);
        } else {
            WRITE(X_DIR_PIN, 0);
        }
        if (CHECK(value, Y_DIRECTION_BIT)) {
            WRITE(Y_DIR_PIN, 1);
        } else {
            WRITE(Y_DIR_PIN, 0);
        }
        if (CHECK(value, Z_DIRECTION_BIT)) {
            WRITE(Z_DIR_PIN, 0);
        } else {
            WRITE(Z_DIR_PIN, 1);
        }
    }


#ifdef	__cplusplus
}
#endif

#endif	/* RAMPS_H */

