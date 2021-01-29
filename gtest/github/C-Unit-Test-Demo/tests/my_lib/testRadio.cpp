#include <gtest/gtest.h>

extern "C" {
    #include "my_lib/Radio.h"
}

class RadioTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _radio = Radio_create();
    }

    Radio_Handle _radio;
};

TEST_F(RadioTest, RadioCreated)
{
    EXPECT_NE(nullptr, _radio);
}

TEST_F(RadioTest, ImplementsSwitchable)
{
    auto* switchable = Radio_getSwitchableInterface(_radio);

    EXPECT_NE(nullptr, switchable);
}

TEST_F(RadioTest, RadioIsTurnedOffAfterCreation)
{
    auto* switchable = Radio_getSwitchableInterface(_radio);

    EXPECT_NE(nullptr, _radio);
    auto state = switchable->getState(switchable->handle);

    EXPECT_EQ(SWITCHABLE_STATE_OFF, state);
}

TEST_F(RadioTest, TurnsOn)
{
    auto* switchable = Radio_getSwitchableInterface(_radio);

    switchable->turnOn(switchable->handle);

    auto state = switchable->getState(switchable->handle);
    EXPECT_EQ(SWITCHABLE_STATE_ON, state);
}

TEST_F(RadioTest, TurnsOnAgainAfterTurningOff)
{
    auto* switchable = Radio_getSwitchableInterface(_radio);

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