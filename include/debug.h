#ifndef CLIENT_DEBUG_H
#define CLIENT_DEBUG_H

extern volatile int indent;

void printSpaces(bool minus_one);

//#define FUNC(space, data) printSpaces(space);Serial.print(data);Serial.println(__FUNCTION__);Serial.flush();
#define FUNC(space, data)
#define FUNC_IN FUNC(false, "+") indent++;
#define FUNC_OUT FUNC(true, "-") indent--;

#endif //CLIENT_DEBUG_H
