#include "EvtCommandListener.h"

EvtCommandListener::EvtCommandListener(Stream *stream, short readDelayMs)
{
  _stream = stream;
  _readDelayMs = readDelayMs;
}

void EvtCommandListener::when(const char *command, EvtCommandAction action, bool isLoop)
{
  CommandAction commandAction;
  commandAction.Command = command;
  commandAction.Action = action;
  commandAction.IsLoop = isLoop;
  _commands[_commandActionIndex] = commandAction;
  _commandActionIndex++;
}

void EvtCommandListener::whenever(const char *command, EvtCommandAction action)
{
  when(command, action, true);
}

bool EvtCommandListener::performTriggerAction(IEvtContext *ctx)
{
  for (short i = 0; i < _commandActionIndex; i++)
  {
    CommandAction action = _commands[i];
    if (strcmp(action.Command, _commandBuffer.c_str()) == 0)
    {
      action.Action(this, ctx, _dataBuffer);
      reset();
      if (action.IsLoop)
      {
        return false;
      }
      return true;
    }
  }

  return false;
}

bool EvtCommandListener::isEventTriggered()
{
  if (!EvtListener::isEventTriggered())
  {
    return false;
  }

  return tryReadCommand();
}

bool EvtCommandListener::tryReadCommand()
{
  while (_stream->available())
  {
    delay(_readDelayMs);
    int ch = _stream->read();
    switch (ch)
    {
    case '>':
      _commandBuffer = "";
      _currentStep = 1; // read command
      break;
    case ':':
      _dataBuffer = "";
      _currentStep = 2; // read data
      break;
    case '!':
      _currentStep = 0; // reset
      return true;
    default:
      if (_currentStep == 1)
      {
        appendToCommand(ch);
      }
      else if (_currentStep == 2)
      {
        appendToData(ch);
      }
      break;
    }
  }
  return false;
}

void EvtCommandListener::appendToCommand(char ch)
{
  _commandBuffer += (char)ch;
}

void EvtCommandListener::appendToData(char ch)
{
  _dataBuffer += (char)ch;
}

void EvtCommandListener::reset()
{
  _commandBuffer = "";
  _dataBuffer = "";
  _currentStep = 0;
}