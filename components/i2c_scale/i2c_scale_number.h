#pragma once

#include "esphome/components/number/number.h"

namespace esphome {
namespace i2c_scale {

class I2CScale;

class I2CScaleCalibrationNumber : public number::Number {
 public:
    void set_parent(I2CScale *parent) {
        parent_ = parent;
    }

    void set_restore_value(bool value)
    {
        restore_value_ = value;
    }

 protected:
    void setup(void) override;
    void control(float value) override;

    I2CScale *parent_;

    // calibration persistence
    bool restore_value_{false};
    ESPPreferenceObject calibration_pref_;
    float calibration_{0.0f};
};

}
}
