#ifndef EV_CONTROL_H
#define EV_CONTROL_H

#include <stdint.h>

typedef enum
{
    DRIVE_ECO = 0,
    DRIVE_NORMAL = 1,
    DRIVE_SPORT = 2
} DriveMode_t;

typedef struct
{
    float speed_kmh;
    float torque_Nm;
    float soc_pct;
    float range_km;
    float power_kW;
    float motor_temp_C;
    float regen_level;
    DriveMode_t drive_mode;
    uint8_t regen_active;
} EV_State_t;

void EV_Init(EV_State_t *ev);
void EV_SetDriveMode(EV_State_t *ev, DriveMode_t mode);
void EV_Update(EV_State_t *ev, float accel_pct, float brake_pct, float dt_s);

#endif
