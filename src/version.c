#include "version.h"

static const int major = 2;
static const int minor = 3;
static const int revision = 0;

int Calculate_Size_Of_String_From_Int(int value)
{
  if(!value)
    value = 1;
  return (int)((ceil(log10(value))+1)*sizeof(char));
}

char* Get_Version_String()
{
  char* version;
  int major_ss = Calculate_Size_Of_String_From_Int(major);
  int minor_ss = Calculate_Size_Of_String_From_Int(minor);
  int revision_ss = Calculate_Size_Of_String_From_Int(revision);
  int full_size = major_ss + minor_ss + revision_ss + 3*sizeof(char);
  char* major_s = malloc(major_ss);
  char* minor_s = malloc(minor_ss);
  char* revision_s = malloc(revision_ss);
  version = malloc(full_size);
  sprintf(major_s,"%d",major);
  sprintf(minor_s,"%d",minor);
  sprintf(revision_s,"%d",revision);
  strcpy(version,major_s);
  strcat(version,".");
  strcat(version,minor_s);
  strcat(version,".");
  strcat(version,revision_s);
  free(major_s);
  free(minor_s);
  free(revision_s);
  return version;
}
