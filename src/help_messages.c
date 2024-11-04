#include "help_messages.h"

void Show_Version()
{
  char* version = Get_Version_String();
  printf("MLangC Interpreter Version %s\n",version);
  puts("By TheNeverMan in 2024");
  /* fuck doctrine, but i can't live without putting my faith everywhere sorry :<*/
  puts("do not defeat. do not fight back. do not surrender.");
  free(version);
}


void Show_Help_Message()
{
  Show_Version();
  puts("Usage: mlang [OPTIONS] [FILE]");
  puts("Available options:");
  puts("-h Display this help information.");
  puts("-v Display version information.");
  puts("-d Run in debug mode, display variable and command info after every step");
  puts("-s Run in step-by-step mode, waiting for user input after every command");
  puts("-p Treat warnings as errors");
  puts("-e Display full error and warning messages (Warnings are displayed only if -w is used).");
  puts("-w Display optional warning messages.");
  puts("-q Stop displaying filename of the executed program at the beginning of interpretation.");
  puts("-a Show prompt on ask command.");
}
