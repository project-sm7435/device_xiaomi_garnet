/*
 * Copyright (C) 2024 LibreMobileOS Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CameraProviderExtension.h"

#include <fstream>
#include <iterator>
#include <string>

#define TORCH_BRIGHTNESS "brightness"
#define TORCH_MAX_BRIGHTNESS "max_brightness"
#define TORCH_TRIGGER "trigger"
#define TOGGLE_SWITCH "/sys/devices/platform/soc/c42d000.qcom,spmi/spmi-0/0-05/c42d000.qcom,spmi:qcom,pm6150l@5:qcom,leds@d300/leds/led:switch_2/brightness"
#define TOGGLE_SWITCH_TRIGGER "/sys/devices/platform/soc/c42d000.qcom,spmi/spmi-0/0-05/c42d000.qcom,spmi:qcom,pm6150l@5:qcom,leds@d300/leds/led:switch_2/trigger"

static std::string kTorchLed0Path = "/sys/devices/platform/soc/c42d000.qcom,spmi/spmi-0/0-05/c42d000.qcom,spmi:qcom,pm6150l@5:qcom,leds@d300/leds/led:torch_0";
static std::string kTorchLed1Path = "/sys/devices/platform/soc/c42d000.qcom,spmi/spmi-0/0-05/c42d000.qcom,spmi:qcom,pm6150l@5:qcom,leds@d300/leds/led:torch_1";

/**
 * Write value to path and close file.
 */
template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

/**
 * Read value from the path and close file.
 */
template <typename T>
static T get(const std::string& path, const T& def) {
    std::ifstream file(path);
    T result;

    file >> result;
    return file.fail() ? def : result;
}

static bool isRedmiTorch1Active() {
    static int cached = -1;
    if (cached != -1) {
        return cached == 1;
    }

    std::ifstream file(kTorchLed1Path + "/" + TORCH_TRIGGER);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    cached = (content.find("[torch1_trigger]") != std::string::npos) ? 1 : 0;
    return cached == 1;
}

bool supportsTorchStrengthControlExt() {
    return true;
}

bool supportsSetTorchModeExt() {
    return false;
}

int32_t getTorchDefaultStrengthLevelExt() {
    return 500;
}

int32_t getTorchMaxStrengthLevelExt() {
    return 500;
}

int32_t getTorchStrengthLevelExt() {
    const auto& path = isRedmiTorch1Active() ? kTorchLed1Path : kTorchLed0Path;
    return get(path + "/" + TORCH_BRIGHTNESS, 0);
}

void setTorchStrengthLevelExt(int32_t torchStrength, bool enabled) {
    if (isRedmiTorch1Active()) {
        set(kTorchLed1Path + "/" + TORCH_BRIGHTNESS, enabled ? torchStrength : 0);
        return;
    }

    if (enabled) {
        set(TOGGLE_SWITCH, 0);
        set(kTorchLed0Path + "/" + TORCH_BRIGHTNESS, torchStrength);
        set(TOGGLE_SWITCH, 255);
    } else {
        set(TOGGLE_SWITCH, 0);
        set(kTorchLed0Path + "/" + TORCH_BRIGHTNESS, 0);
        set(TOGGLE_SWITCH_TRIGGER, "switch2_trigger");
        set(kTorchLed0Path + "/" + TORCH_TRIGGER, "torch0_trigger");
    }
}

void setTorchModeExt(bool enabled) {
    int32_t strength = getTorchDefaultStrengthLevelExt();
    setTorchStrengthLevelExt(enabled ? strength : 0, enabled);
}