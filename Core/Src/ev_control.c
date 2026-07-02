#include "ev_control.h"

#define EV_MAX_SPEED_KMH        200.0f
#define EV_MAX_TORQUE_NM        200.0f
#define EV_REGEN_MAX_NM         80.0f
#define EV_REGEN_THRESHOLD_PCT  5.0f
#define EV_BATTERY_CAP_KWH      20.0f
#define EV_DRAG_COEFF           5.0f
#define EV_MASS_FACTOR          450.0f
#define EV_SIM_SCALE            32.0f

static float clamp_float(float value, float min, float max)
{
    if (value < min)
    {
        return min;
    }

    if (value > max)
    {
        return max;
    }

    return value;
}

static float get_mode_scale(DriveMode_t mode)
{
    switch (mode)
    {
        case DRIVE_ECO:
            return 0.6f;

        case DRIVE_SPORT:
            return 1.3f;

        case DRIVE_NORMAL:
        default:
            return 1.0f;
    }
}

void EV_Init(EV_State_t *ev)
{
    ev->speed_kmh = 0.0f;
    ev->torque_Nm = 0.0f;
    ev->soc_pct = 100.0f;
    ev->range_km = 0.0f;
    ev->power_kW = 0.0f;
    ev->motor_temp_C = 25.0f;
    ev->regen_level = 0.0f;
    ev->drive_mode = DRIVE_NORMAL;
    ev->regen_active = 0;
}

void EV_SetDriveMode(EV_State_t *ev, DriveMode_t mode)
{
    ev->drive_mode = mode;
}

void EV_Update(EV_State_t *ev, float accel_pct, float brake_pct, float dt_s)
{
    float mode_scale;
    float speed_ms;
    float drag_Nm;
    float accel_ms2;
    float torque_ratio;
    float p_mech_kW;
    float p_loss_kW;
    float delta_soc;
    float remaining_energy_kWh;
    float km_per_kWh;
    float heat_input;
    float cooling;

    accel_pct = clamp_float(accel_pct, 0.0f, 100.0f);
    brake_pct = clamp_float(brake_pct, 0.0f, 100.0f);

    mode_scale = get_mode_scale(ev->drive_mode);

    ev->torque_Nm = (accel_pct / 100.0f) * EV_MAX_TORQUE_NM * mode_scale;
    ev->torque_Nm = clamp_float(ev->torque_Nm, 0.0f, EV_MAX_TORQUE_NM);

    if (brake_pct > EV_REGEN_THRESHOLD_PCT)
    {
        ev->regen_active = 1;
        ev->regen_level = brake_pct * 0.7f;
        ev->torque_Nm = -(ev->regen_level / 100.0f) * EV_REGEN_MAX_NM;
    }
    else
    {
        ev->regen_active = 0;
        ev->regen_level = 0.0f;
    }

    speed_ms = ev->speed_kmh / 3.6f;
    drag_Nm = speed_ms * EV_DRAG_COEFF;
    accel_ms2 = (ev->torque_Nm - drag_Nm) / EV_MASS_FACTOR;

    ev->speed_kmh = ev->speed_kmh + (accel_ms2 * dt_s * 3.6f);
    ev->speed_kmh = clamp_float(ev->speed_kmh, 0.0f, EV_MAX_SPEED_KMH);

    speed_ms = ev->speed_kmh / 3.6f;

    p_mech_kW = (ev->torque_Nm * speed_ms) / 1000.0f;

    torque_ratio = ev->torque_Nm / EV_MAX_TORQUE_NM;
    p_loss_kW = torque_ratio * torque_ratio * 5.0f;

    ev->power_kW = p_mech_kW + p_loss_kW;

    delta_soc = (ev->power_kW * dt_s) / (EV_BATTERY_CAP_KWH * 3600.0f);
    delta_soc = delta_soc * 100.0f * EV_SIM_SCALE;

    ev->soc_pct = ev->soc_pct - delta_soc;
    ev->soc_pct = clamp_float(ev->soc_pct, 0.0f, 100.0f);

    remaining_energy_kWh = (ev->soc_pct / 100.0f) * EV_BATTERY_CAP_KWH;

    if (ev->drive_mode == DRIVE_ECO)
    {
        km_per_kWh = 8.0f;
    }
    else if (ev->drive_mode == DRIVE_SPORT)
    {
        km_per_kWh = 4.2f;
    }
    else
    {
        km_per_kWh = 5.5f;
    }

    ev->range_km = remaining_energy_kWh * km_per_kWh;

    if (ev->power_kW < 0.0f)
    {
        heat_input = -ev->power_kW * 0.05f;
    }
    else
    {
        heat_input = ev->power_kW * 0.05f;
    }

    cooling = (ev->motor_temp_C - 25.0f) * 0.01f;

    ev->motor_temp_C = ev->motor_temp_C + ((heat_input - cooling) * dt_s);
    ev->motor_temp_C = clamp_float(ev->motor_temp_C, 25.0f, 130.0f);
}
