#ifndef EVT_COMMAND_LISTENER_H
#define EVT_COMMAND_LISTENER_H

#include <Eventually.h>
#include <EvtCommandAction.h>
#include <Arduino.h>

#include <stdlib.h>
#include "stdint.h"
#include "string.h"

#ifndef EVENTUALLY_MAX_COMMANDS
#define EVENTUALLY_MAX_COMMANDS 10
#endif

struct commandAction
{
  EvtCommandAction Action;
  const char *Command;
  bool IsLoop;
} typedef CommandAction;

class EvtCommandListener : public EvtListener
{
public:
  EvtCommandListener(Stream *stream, short readDelayMs = 5);
  bool tryReadCommand();
  /// @brief Register a command to be listened for. It will trigger only once.
  /// @param command
  /// @param action
  /// @param isLoop If true, the command will trigger loop. which equals to whenever.
  void when(const char *command, EvtCommandAction action, bool isLoop = false);
  /// @brief Register a command to be listened for. It will trigger loop.
  /// @param command
  /// @param action
  void whenever(const char *command, EvtCommandAction action);

  void reset();
  bool isEventTriggered();
  bool performTriggerAction(IEvtContext *ctx);

private:
  String _commandBuffer;
  String _dataBuffer;
  Stream *_stream;
  short _commandActionIndex = 0;
  short _currentStep = 0;
  unsigned long _readDelayMs;
  CommandAction _commands[EVENTUALLY_MAX_COMMANDS];

  void appendToCommand(char ch);
  void appendToData(char ch);
};

#endif
