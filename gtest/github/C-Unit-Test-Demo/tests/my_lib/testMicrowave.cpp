#include <gtest/gtest.h>

extern "C" {
    #include "my_lib/Microwave.h"
}

class MicrowaveTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _microwave = Microwave_create();
    }

    Microwave_Handle _microwave;
};

TEST_F(MicrowaveTest, MicrowaveCreated)
{
    EXPECT_NE(nullptr, _microwave);
}

TEST_F(MicrowaveTest, ImplementsSwitchable)
{
    auto* switchable = Microwave_getSwitchableInterface(_microwave);

    EXPECT_NE(nullptr, switchable);
}

TEST_F(MicrowaveTest, MicrowaveIsTurnedOffAfterCreation)
{
    auto* switchable = Microwave_getSwitchableInterface(_microwave);

    EXPECT_NE(nullptr, _microwave);
    auto state = switchable->getState(switchable->handle);

    EXPECT_EQ(SWITCHABLE_STATE_OFF, state);
}

TEST_F(MicrowaveTest, TurnsOn)
{
    auto* switchable = Microwave_getSwitchableInterface(_microwave);

    switchable->turnOn(switchable->handle);

    auto state = switchable->getState(switchable->handle);
    EXPECT_EQ(SWITCHABLE_STATE_ON, state);
}

TEST_F(MicrowaveTest, TurnsOnAgainAfterTurningOff)
{
    auto* switchable = Microwave_getSwitchableInterface(_microwave);

    switchable->turnOn(switchable->handle);
    auto state = switchable->getState(switchable->handle);
    EXPECT_EQ(SWITCHABLE_STATE_ON, state);

    switchable->turnOff(switchable->handle);
    state = switchable->getState(switchable->handle);
    EXPECT_EQ(SWITCHABLE_STATE_OFF, state);

    switchable->turnOn(switchable->handle);
    state = switchable->getState(switchable->handle);
    EXPECT_EQ(SWITCHABLE_STATE_ON, state);
}