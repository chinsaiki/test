#include <gtest/gtest.h>

extern "C" {
    #include "my_lib/Switcher.h"
    #include "SwitchableMock.h"
}

class SwitcherTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        SwitchableMock_initialize(&_switchableMock);
        _switcher = Switcher_create(&_switchableMock.switchable);
    }

    SwitchableMock _switchableMock;
    Switcher_Handle _switcher;
};

TEST_F(SwitcherTest, SwitcherCreated)
{
    EXPECT_NE(nullptr, _switcher);
}

TEST_F(SwitcherTest, CallsSwitchableInterfaceDuringSwitching)
{
    Switcher_doSomeSwitching(_switcher, 9);

    EXPECT_EQ(5, _switchableMock.turnOnCallCounter);
    EXPECT_EQ(4, _switchableMock.turnOffCallCounter);
    EXPECT_GE(1, _switchableMock.getStateCallCounter);
}

TEST_F(SwitcherTest, CanUpdateSwitchableInterface)
{
    SwitchableMock anotherSwitchableMock;
    SwitchableMock_initialize(&anotherSwitchableMock);

    // Do some switching with the old interface
    Switcher_doSomeSwitching(_switcher, 1);
    EXPECT_GE(1, _switchableMock.getStateCallCounter);

    // Now switch interfaces and see if new interface is called
    SwitchableMock_initialize(&_switchableMock);
    Switcher_updateSwitchable(_switcher, &anotherSwitchableMock.switchable);

    Switcher_doSomeSwitching(_switcher, 1);
    // The former interface should not have been called...
    EXPECT_EQ(0, _switchableMock.getStateCallCounter);

    // ... but the updated one should
    EXPECT_GE(1, anotherSwitchableMock.getStateCallCounter);
}

TEST_F(SwitcherTest, TurnsOffIfTurnedOn)
{
    // We now change the state the Switchable interface is in
    _switchableMock.switchableState = SWITCHABLE_STATE_ON;

    // If we switch now...
    Switcher_doSomeSwitching(_switcher, 1);

    // ... we expect turnOff to be called, not turnOn
    EXPECT_EQ(0, _switchableMock.turnOnCallCounter);
    EXPECT_EQ(1, _switchableMock.turnOffCallCounter);
    EXPECT_GE(1, _switchableMock.getStateCallCounter);
}
