#include <iostream>
#include "Utils.h"
#include "MessageHelper.h"
#include <unistd.h>

using namespace std;

void initLogging();

int main()
{
  /*Comment this line to remove logging function*/
  initLogging();
}

void initLogging()
{
  Logger * ins = Logger::getInstance();
  ins->LoggerInit();
}
