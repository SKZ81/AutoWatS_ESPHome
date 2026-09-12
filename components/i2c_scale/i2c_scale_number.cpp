#include "i2c_scale_number.h"
#include "i2c_scale.h"

namespace esphome {
namespace i2c_scale {

void I2CScaleCalibrationNumber::setup() {
    if (!restore_value_)
        return;

    uint32_t pref_key =
        fnv1_hash(("i2c_scale_calibration_" +
               str_sprintf("%02x", this->address_)).c_str());

    calibration_pref_ =
        global_preferences->make_preference<float>(pref_key);

    if (calibration_pref_.load(&calibration_))
    {
        ESP_LOGI(TAG, "Loaded calibration value: %f", calibration_);

        parent_->set_calibration(calibration_);

        if (calibration_number_ != nullptr)
            calibration_number_->publish_state(calibration_);
    }
}

void I2CScaleCalibrationNumber::control(float value)
{
    if(parent_->set_calibration(value))
    {
        publish_state(value);

        if (restore_value_)
            calibration_pref_.save(&value);
    }}

}
}
