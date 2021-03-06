/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "ConfigstoreHidlHalTest"

#include <android-base/logging.h>
#include <android/hardware/configstore/1.0/ISurfaceFlingerConfigs.h>
#include <android/hardware/configstore/1.0/types.h>
#include <gtest/gtest.h>
#include <hidl/GtestPrinter.h>
#include <hidl/ServiceManagement.h>
#include <unistd.h>

using ::android::hardware::configstore::V1_0::ISurfaceFlingerConfigs;
using ::android::hardware::configstore::V1_0::OptionalBool;
using ::android::hardware::configstore::V1_0::OptionalInt64;
using ::android::hardware::configstore::V1_0::OptionalUInt64;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

#define ASSERT_OK(ret) ASSERT_TRUE(ret.isOk())
#define EXPECT_OK(ret) EXPECT_TRUE(ret.isOk())

class ConfigstoreHidlTest : public ::testing::TestWithParam<std::string> {
   public:
    sp<ISurfaceFlingerConfigs> sfConfigs;

    virtual void SetUp() override {
        sfConfigs = ISurfaceFlingerConfigs::getService(GetParam());
        ASSERT_NE(sfConfigs, nullptr);
    }

    virtual void TearDown() override {}
};

/**
 * Ensure all ISurfaceFlingerConfigs.hal function calls are successful.
 */
TEST_P(ConfigstoreHidlTest, TestFunctionCalls) {
    bool tmp;

    Return<void> status = sfConfigs->vsyncEventPhaseOffsetNs(
        [&tmp](OptionalInt64 arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->vsyncSfEventPhaseOffsetNs(
        [&tmp](OptionalInt64 arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->useContextPriority(
        [&tmp](OptionalBool arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->hasWideColorDisplay(
        [&tmp](OptionalBool arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->hasHDRDisplay(
        [&tmp](OptionalBool arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->presentTimeOffsetFromVSyncNs(
        [&tmp](OptionalInt64 arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->useHwcForRGBtoYUV(
        [&tmp](OptionalBool arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->maxVirtualDisplaySize(
        [&tmp](OptionalUInt64 arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->hasSyncFramework(
        [&tmp](OptionalBool arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->useVrFlinger(
        [&tmp](OptionalBool arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->maxFrameBufferAcquiredBuffers(
        [&tmp](OptionalInt64 arg) { tmp = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->startGraphicsAllocatorService(
        [&tmp](OptionalBool arg) { tmp = arg.specified; });
    EXPECT_OK(status);
}

/**
 * Ensure repeated call to the same function returns the same result.
 */
TEST_P(ConfigstoreHidlTest, TestSameReturnValue) {
    int64_t original_ret;
    Return<void> status = sfConfigs->vsyncEventPhaseOffsetNs(
        [&original_ret](OptionalInt64 arg) { original_ret = arg.value; });

    int64_t next_ret;
    for (int cnt = 0; cnt < 10; cnt++) {
        status = sfConfigs->vsyncEventPhaseOffsetNs(
            [&next_ret](OptionalInt64 arg) { next_ret = arg.value; });
        EXPECT_EQ(original_ret, next_ret);
    }
}

/**
 * Make sure the constrains of hasWideColorDisplay, hasHDRDisplay
 * are enforced.
 */
TEST_P(ConfigstoreHidlTest, TestColorConstrainsBasic) {
    bool hasWideColorDisplay;
    bool hasHDRDisplay;

    Return<void> status = sfConfigs->hasWideColorDisplay(
        [&](OptionalBool arg) { hasWideColorDisplay = arg.specified; });
    EXPECT_OK(status);

    status = sfConfigs->hasHDRDisplay([&](OptionalBool arg) { hasHDRDisplay = arg.specified; });
    EXPECT_OK(status);

    // When hasHDRDisplay returns true, hasWideColorDisplay must also return true.
    if (hasHDRDisplay) {
        ASSERT_TRUE(hasWideColorDisplay);
    }
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ConfigstoreHidlTest);
INSTANTIATE_TEST_SUITE_P(
        PerInstance, ConfigstoreHidlTest,
        testing::ValuesIn(android::hardware::getAllHalInstanceNames(ISurfaceFlingerConfigs::descriptor)),
        android::hardware::PrintInstanceNameToString);
