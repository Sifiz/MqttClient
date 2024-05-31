#include <Ccaptive_portal.h>
#include <Arduino.h>
#include <unity.h>


void test_init_portal(void){
    Portal portal;
    String result = portal.initPortal();
    if (portal.initPortal() == "ERROR"){
        TEST_FAIL_MESSAGE("Error in the portal initialization");
    }
}
