#include <ArduinoFake.h>
#include <unity.h>

#include "EvtCommandListener.h"
#include "EvtContext.h"

using namespace fakeit;

Stream *stream;
EvtContext ctx;
EvtCommandListener *target;
bool _called = false;
char *_data;

void setUp(void)
{
  ArduinoFakeReset();
  When(Method(ArduinoFake(), delay)).AlwaysReturn();
  _called = false;
}

void tearDown(void)
{
  delete stream;
  delete target;
}

bool mockMethod(EvtListener *listener, EvtContext *ctx, const String &data)
{
  _called = true;
  // printf("mockMethod called with data: %s\n", data.c_str());
  _data = (char *)malloc(data.length() + 1);
  strcpy(_data, data.c_str());
  return true;
}

void test_not_triggered_by_invalid_command(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('b', 'l', 'a', 'h');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_FALSE(actual);
}

void test_waits_default_time_before_read(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('b', 'l', 'a', 'h');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->isEventTriggered();

  Verify(Method(ArduinoFake(), delay).Using(5)).Exactly(4_Times);
}

void test_waits_specified_time_before_read(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('b', 'l', 'a', 'h');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream, 99);

  target->isEventTriggered();

  Verify(Method(ArduinoFake(), delay).Using(99)).Exactly(4_Times);
}

void test_not_triggered_by_non_terminated_command(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_FALSE(actual);
}

void test_not_triggered_by_non_terminated_command_does_not_call_action(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->isEventTriggered();
  bool actual = target->performTriggerAction(&ctx);
  TEST_ASSERT_FALSE(actual);
  TEST_ASSERT_FALSE(_called);
}

void test_triggered_by_valid_command(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_TRUE(actual);
}

void test_triggered_by_valid_command_calls_action(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->when("set", (EvtCommandAction)mockMethod);

  target->isEventTriggered();
  bool actual = target->performTriggerAction(&ctx);
  TEST_ASSERT_TRUE(actual);
  TEST_ASSERT_TRUE(_called);
}

void test_does_not_trigger_when_disabled(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);
  target->disable();
  TEST_ASSERT_FALSE(target->isEventTriggered());
}

void test_triggered_by_embedded_command(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('r', 'a', 'n', '>', 's', 'e', 't', '!', 'd', 'o', 'm');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_TRUE(actual);
}

void test_triggered_by_command_with_missing_data(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_TRUE(actual);
}

void test_triggered_by_command_with_invalid_data(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', 'x', 'x', 'x', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_TRUE(actual);
}

void test_triggered_by_command_with_invalid_data_calls_action(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->when("set", (EvtCommandAction)mockMethod);

  target->isEventTriggered();
  bool actual = target->performTriggerAction(&ctx);
  TEST_ASSERT_TRUE(actual);
  TEST_ASSERT_TRUE(_called);
  TEST_ASSERT_EQUAL_STRING("", _data);
}

void test_triggered_by_command_with_positive_data(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '3', '5', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_TRUE(actual);
}

void test_triggered_by_command_with_positive_data_calls_action(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '3', '5', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->when("set", (EvtCommandAction)mockMethod);

  target->isEventTriggered();
  bool actual = target->performTriggerAction(&ctx);
  TEST_ASSERT_TRUE(actual);
  TEST_ASSERT_TRUE(_called);
  TEST_ASSERT_EQUAL_STRING("35", _data);
}

void test_triggered_by_command_with_single_digit_data_calls_action(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '3', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->when("set", (EvtCommandAction)mockMethod);

  target->isEventTriggered();
  bool actual = target->performTriggerAction(&ctx);
  TEST_ASSERT_TRUE(actual);
  TEST_ASSERT_TRUE(_called);
  TEST_ASSERT_EQUAL_STRING("3", _data);
}

void test_triggered_by_command_with_negative_data(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '-', '3', '5', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_TRUE(actual);
}

void test_triggered_by_command_with_negative_data_calls_action(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '-', '3', '5', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->when("set", (EvtCommandAction)mockMethod);

  target->isEventTriggered();
  bool actual = target->performTriggerAction(&ctx);
  TEST_ASSERT_TRUE(actual);
  TEST_ASSERT_TRUE(_called);
  TEST_ASSERT_EQUAL_STRING("-35", _data);
}

void test_triggered_by_command_with_max_data(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '1', '6', '4', '1', '0', '9', '2', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_TRUE(actual);
}

void test_triggered_by_command_with_max_data_calls_action(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '1', '6', '4', '1', '0', '9', '2', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->when("set", (EvtCommandAction)mockMethod);

  target->isEventTriggered();
  bool actual = target->performTriggerAction(&ctx);
  TEST_ASSERT_TRUE(actual);
  TEST_ASSERT_TRUE(_called);
  TEST_ASSERT_EQUAL_STRING("1641092", _data);
}

void test_triggered_by_command_with_large_data(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '1', '6', '4', '1', '0', '9', '2', '4', '9', '4', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  bool actual = target->isEventTriggered();
  TEST_ASSERT_TRUE(actual);
}

void test_triggered_by_command_with_large_data_calls_action_with_truncated_data(void)
{
  When(Method(ArduinoFake(Stream), available)).Return(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0);
  When(Method(ArduinoFake(Stream), read)).Return('>', 's', 'e', 't', ':', '1', '6', '4', '1', '0', '9', '2', '4', '9', '4', '!');
  stream = ArduinoFakeMock(Stream);
  target = new EvtCommandListener(stream);

  target->when("set", (EvtCommandAction)mockMethod);

  target->isEventTriggered();
  bool actual = target->performTriggerAction(&ctx);
  TEST_ASSERT_TRUE(actual);
  TEST_ASSERT_TRUE(_called);
  TEST_ASSERT_EQUAL_STRING("1641092494", _data);
}

int main(int argc, char **argv)
{
  UNITY_BEGIN();
  RUN_TEST(test_not_triggered_by_invalid_command);
  RUN_TEST(test_waits_default_time_before_read);
  RUN_TEST(test_waits_specified_time_before_read);
  RUN_TEST(test_not_triggered_by_non_terminated_command);
  RUN_TEST(test_not_triggered_by_non_terminated_command_does_not_call_action);
  RUN_TEST(test_triggered_by_valid_command);
  RUN_TEST(test_triggered_by_valid_command_calls_action);
  RUN_TEST(test_does_not_trigger_when_disabled);
  RUN_TEST(test_triggered_by_embedded_command);
  RUN_TEST(test_triggered_by_command_with_missing_data);
  RUN_TEST(test_triggered_by_command_with_invalid_data);
  RUN_TEST(test_triggered_by_command_with_invalid_data_calls_action);
  RUN_TEST(test_triggered_by_command_with_positive_data);
  RUN_TEST(test_triggered_by_command_with_positive_data_calls_action);
  RUN_TEST(test_triggered_by_command_with_single_digit_data_calls_action);
  RUN_TEST(test_triggered_by_command_with_negative_data);
  RUN_TEST(test_triggered_by_command_with_negative_data_calls_action);
  RUN_TEST(test_triggered_by_command_with_max_data);
  RUN_TEST(test_triggered_by_command_with_max_data_calls_action);
  RUN_TEST(test_triggered_by_command_with_large_data);
  RUN_TEST(test_triggered_by_command_with_large_data_calls_action_with_truncated_data);
  UNITY_END();

  return 0;
}
