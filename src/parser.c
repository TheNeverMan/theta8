#include "parser.h"
#define MATRIX_SIZE_DEF 64
static const int MATRIX_SIZE = MATRIX_SIZE_DEF;

static FILE* Open_File(char* filename)
{
  FILE* out = fopen(filename, "rb");
  if(!out)
    Print_Error(file_not_found, TRUE);
  return out;
}

static struct RGB* Parse_PPM(char* filename)
{
  struct RGB* out = NULL;
  FILE* File = Open_File(filename);
  out = malloc(sizeof(struct RGB) * MATRIX_SIZE); /* we need 64 pixels*/
  /*read ppm header there is only one correct header so we can just compare it to reference i guess*/
  char* reference_header = "P6\n8 8\n255\n";
  char* header_from_file = malloc(sizeof(char)*(strlen(reference_header)+1));
  fread(header_from_file,sizeof(char),strlen(reference_header),File);
  if(feof(File) || ferror(File))
  {
    free(header_from_file);
    free(out);
    Print_Error(error_reading_from_file, TRUE);
  }
  if(strcmp(reference_header,header_from_file) != 0)
  {
    free(header_from_file);
    free(out);
    Print_Error(invalid_ppm_header, TRUE);
  }
  free(header_from_file);
  int index = 0;
  while(index < MATRIX_SIZE)
  {
    byte tmp[3];
    fread(tmp,sizeof(char),3,File);
    if(feof(File) || ferror(File))
    {
      free(out);
      Print_Error(error_reading_from_file, TRUE);
    }
    out[index].R = tmp[0];
    out[index].G = tmp[1];
    out[index].B = tmp[2];
    index++;
  }
  fclose(File);
  return out;
}

byte* Parse_File(char* filename)
{
  byte* out = NULL;
  struct RGB* Pixel_Array = Parse_PPM(filename);
  out = malloc(sizeof(byte) * MATRIX_SIZE);
  int index = 0;
  while(index < MATRIX_SIZE)
  {
    byte tmp = 0;
    unsigned int mod = ((unsigned int)Pixel_Array[index].R + (unsigned int)Pixel_Array[index].B + (unsigned int)Pixel_Array[index].B) % 255;
    if(!mod)
    {
      if(Pixel_Array[index].R == 255)
        tmp += 4;
      if(Pixel_Array[index].G == 255)
        tmp += 2;
      if(Pixel_Array[index].B == 255)
        tmp += 1;
    }
    else
      tmp = Pixel_Array[index].R;
    out[index] = tmp;
    index++;
  }
  free(Pixel_Array);
  return out;
}
