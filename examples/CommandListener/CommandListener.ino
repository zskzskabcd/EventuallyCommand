// define the data buffer length
// This depends on the length of data you need. The larger
// the value, the more memory it occupies, and the longer
// the data can be received. You can adjust it according to
// your needs and the memory size of the development board.
#define EVENTUALLY_DATA_BUFFER_LENGTH 32

#include <Eventually.h>
#include <EventuallyCommand.h>

EvtManager mgr;
// monitor Serial stream with an optional read
// delay (defaults to 5ms). You might need to
// set this higher depending on your baudrate
EvtCommandListener commandListener(&Serial, 5);

void setup()
{
  Serial.begin(115200);

  commandListener.when("set", (EvtCommandAction)set);
  commandListener.whenever("update", (EvtCommandAction)update);

  mgr.addListener(&commandListener);

  Serial.println("Setup complete. Continuing...");
}

// serial will be checked on each loop
void loop()
{
  mgr.loopIteration();
}

// no need to specify EvtCommandAction arguments
bool set()
{
  Serial.println("Setting...");

  // continue with other listeners in chain
  return false;
}

// unless you are interested in the data element
bool update(EvtListener *, EvtContext *, const String &data)
{
  Serial.print("Updating with ");
  Serial.print(data);
  Serial.println("...");

  // continue with other listeners in chain
  return false;
}
