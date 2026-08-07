#
# Copyright (C) 2024 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
TARGET_SUPPORTS_OMX_SERVICE := false
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

TARGET_HAS_UDFPS := true
TARGET_DISABLE_EPPE := true

# Inherit from garnet device
$(call inherit-product, device/xiaomi/garnet/device.mk)

PRODUCT_NAME := lineage_garnet
PRODUCT_DEVICE := garnet
PRODUCT_MANUFACTURER := Xiaomi
PRODUCT_BRAND := Redmi
PRODUCT_MODEL := 2312DRA50G

PRODUCT_SYSTEM_NAME := garnet_global
PRODUCT_SYSTEM_DEVICE := garnet

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildDesc="garnet_global-user 16 BP2A.250605.031.A3 OS3.0.5.0.WNRMIXM release-keys" \
    BuildFingerprint=Redmi/garnet_global/garnet:16/BP2A.250605.031.A3/OS3.0.5.0.WNRMIXM:user/release-keys \
    DeviceName=$(PRODUCT_SYSTEM_DEVICE) \
    DeviceProduct=$(PRODUCT_SYSTEM_NAME)

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi

# Axion Device Configuration
AXION_MAINTAINER := ZetyaWyn
AXION_PROCESSOR := Snapdragon®_7s_Gen_2

# Camera Info
AXION_CAMERA_REAR_INFO := 200/64,8,2
AXION_CAMERA_FRONT_INFO := 16

# Display
TARGET_SUPPORTED_REFRESH_RATES := 60,90,120
TARGET_BOOT_ANIMATION_RES := 1080

# Performance
PERF_GOV_SUPPORTED := true
PERF_DEFAULT_GOV := walt
