#include "gtest/gtest.h"

#include "../src/Manager.hpp"

TEST(ManagerTest, ManagerEmptyInitTest) {
    kp::Manager mgr;
    mgr.~Manager();
}
