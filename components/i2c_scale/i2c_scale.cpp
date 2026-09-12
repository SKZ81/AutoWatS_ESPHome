#include "i2c_scale.h"
#include "esphome/core/log.h"

namespace esphome {
namespace i2c_scale {

static const char *TAG = "i2c_scale";

static constexpr uint8_t CMD_POWER_DOWN      = 1;
static constexpr uint8_t CMD_POWER_UP        = 2;
static constexpr uint8_t CMD_TARE            = 3;
static constexpr uint8_t CMD_SET_CALIBRATION = 5;
static constexpr uint8_t CMD_GET_VALUE       = 132;

void I2CScale::setup() {
    if (always_on_)
        power_up_();
}

void I2CScale::update() {
    if (!always_on_)
        power_up_();

    float value;

    if (get_value_(value, read_samples_)) {
        publish_state(value);
    }

    if (!always_on_)
        power_down_();
}

void I2CScale::dump_config() {
    ESP_LOGCONFIG(TAG, "Scale I2C");
    LOG_I2C_DEVICE(this);
    LOG_UPDATE_INTERVAL(this);

    ESP_LOGCONFIG(TAG, "Always on: %s", YESNO(always_on_));
}

bool I2CScale::power_up_() {
    uint8_t cmd = CMD_POWER_UP;
    return write(&cmd, 1) == i2c::ERROR_OK;
}

bool I2CScale::power_down_() {
    uint8_t cmd = CMD_POWER_DOWN;
    return write(&cmd, 1) == i2c::ERROR_OK;
}

bool I2CScale::get_value_(float &value, uint8_t samples) {
    uint8_t tx[2] = {
        CMD_GET_VALUE,
        samples
    };

    if (write(tx,2) != i2c::ERROR_OK)
        return false;

    uint8_t rx[4];

    if (read(rx, 4) != i2c::ERROR_OK)
        return false;

    union {
        float f;
        uint8_t b[4];
    } conv;

    conv.b[3] = rx[0];
    conv.b[2] = rx[1];
    conv.b[1] = rx[2];
    conv.b[0] = rx[3];

    value = conv.f;

    return true;
}

bool I2CScale::tare() {
    if (!always_on_)
        power_up_();

    uint8_t tx[2] = {
        CMD_TARE,
        maintenance_samples_
    };

    bool ok = write(tx, 2) == i2c::ERROR_OK;

    if (!always_on_)
        power_down_();

    return ok;
}

bool I2CScale::set_calibration(float calibration) {
    if (!always_on_)
        power_up_();

    union {
        float f;
        uint8_t b[4];
    } conv;

    conv.f = calibration;

    uint8_t tx[5];

    tx[0] = CMD_SET_CALIBRATION;
    tx[1] = conv.b[3];
    tx[2] = conv.b[2];
    tx[3] = conv.b[1];
    tx[4] = conv.b[0];

    bool ok = write(tx, 5) == i2c::ERROR_OK;

    if (!always_on_)
        power_down_();

    if (ok)
    {
        calibration_ = calibration;
        calibration_pref_.save(&calibration_);

        if (calibration_number_ != nullptr)
            calibration_number_->publish_state(calibration_);
    }

    return ok;
}

}
}
