#include <Arduino.h>
#include <SpaIot.h>
#include <unity.h>


using namespace SpaIot;
ControlPanel spa ("SPAIOT32SSP");
void test_setPower_should_setPowerStateON(void){
    spa.setPower(true);
    TEST_ASSERT_TRUE(spa.isPowerOn());
}
void test_setPower_should_setPowerStateOFF(void){
    spa.setPower(false);
    TEST_ASSERT_FALSE(spa.isPowerOn());
}

void test_setFilter_should_setFilterState(void) {
    spa.setFilter(true);
    TEST_ASSERT_TRUE(spa.isFilterOn());
    delay(2000);
    spa.setFilter(false);
    TEST_ASSERT_FALSE(spa.isFilterOn());
}

void test_setBubble_should_setBubbleState(void) {
    spa.setBubble(true);
    TEST_ASSERT_TRUE(spa.isBubbleOn());
    delay(2000);
    spa.setBubble(false);
    TEST_ASSERT_FALSE(spa.isBubbleOn());
}

void test_setHeater_should_setHeaterState(void) {
    spa.setHeater(true);
    TEST_ASSERT_TRUE(spa.isHeaterOn());
    delay(2000);
    spa.setHeater(false);
    TEST_ASSERT_FALSE(spa.isHeaterOn());
}

void test_setPower_should_setPowerState(void) {
    spa.setPower(true);
    TEST_ASSERT_TRUE(spa.isPowerOn());
    delay(2000);
    spa.setPower(false);
    TEST_ASSERT_FALSE(spa.isPowerOn());
}

void setup() {
    // Cette fonction est appelée avant chaque test
    // Vous pouvez y initialiser votre spa si nécessaire
      spa.begin();
        if (spa.isOpen() == false) { // check if the connection to the spa has been open
        for (;;); // loop always, to stop
  }
    UNITY_BEGIN();
    RUN_TEST(test_setPower_should_setPowerStateON);
    delay(10);
    RUN_TEST(test_setFilter_should_setFilterState);
    delay(10);
    RUN_TEST(test_setBubble_should_setBubbleState);
    delay(10);
    RUN_TEST(test_setHeater_should_setHeaterState);
    delay(10);
    RUN_TEST(test_setPower_should_setPowerState);
    delay(10);
    RUN_TEST(test_setPower_should_setPowerStateOFF);
    delay(10);
    UNITY_END();
}
void loop() {
    // put your main code here, to run repeatedly:

}
void tearDown(void) {
    // Cette fonction est appelée après chaque test
    // Vous pouvez y libérer les ressources si nécessaire
}


