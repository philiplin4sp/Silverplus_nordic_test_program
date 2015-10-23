#include "stdafx.h"

#include <stdio.h>
#include <conio.h>
#include <string.h>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
using namespace std;

typedef struct{
   char* keyword;
   int   kw_size;
   int   index;
   int   match;
}keyword_info; 

int SquenceKeywordStrCmp(keyword_info* kw_info, int kw_amount, char char_in);
int AddNewKeyword(keyword_info* kw_info, int index, char* keyword, int kw_size);


void system_error(char *name) {
  // Retrieve, format, and print out a message from the last error.  The 
  // `name' that's passed should be in the form of a present tense noun 
  // (phrase) such as "opening file".
  //
  //char *ptr = NULL;
  WCHAR ptr[1024];
  FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        0,
        GetLastError(),
        0,
        //(char *)&ptr,
        ptr,
        1024,
        NULL);

  //fprintf(stderr, "\nError %s: %s\n", name, ptr);
  fprintf(stderr, "\nError %s: %s\n", name, &ptr);
  LocalFree(ptr);
}


int _tmain(int argc, _TCHAR* argv[])
{
   
  int ch;
  char buffer[1];
  HANDLE file;
  COMMTIMEOUTS timeouts;
  DWORD read, written;
  DCB port;
  HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode;
  //char port_name[128] = "\\\\.\\COM3";
  LPCWSTR port_name = L"\\\\.\\COM4";
  char init[] = ""; // e.g., "ATZ" to completely reset a modem.

  keyword_info kw[10];
  AddNewKeyword(kw, 0, "Linux", 5);
  AddNewKeyword(kw, 1, "Arch", 3);

  if ( argc > 2 )
    swprintf_s((wchar_t *)&port_name, 128,L"\\\\.\\COM%c", argv[1][0]);
  //sprintf(port_name, "\\\\.\\COM%c", argv[1][0]);

  // open the comm port.
  file = CreateFile(port_name,
            GENERIC_READ | GENERIC_WRITE,
            0, 
            NULL, 
            OPEN_EXISTING,
            0,
            NULL);

  if ( INVALID_HANDLE_VALUE == file) {
    system_error("opening file");
    return 1;
  }

  // get the current DCB, and adjust a few bits to our liking.
  memset(&port, 0, sizeof(port));
  port.DCBlength = sizeof(port);
  if ( !GetCommState(file, &port))
    system_error("getting comm state");

  if (!BuildCommDCB(L"baud=115200 parity=n data=8 stop=1", &port))
    system_error("building comm DCB");
  if (!SetCommState(file, &port))
    system_error("adjusting port settings");

  // set short timeouts on the comm port.
  timeouts.ReadIntervalTimeout = 1;
  timeouts.ReadTotalTimeoutMultiplier = 1;
  timeouts.ReadTotalTimeoutConstant = 1;
  timeouts.WriteTotalTimeoutMultiplier = 1;
  timeouts.WriteTotalTimeoutConstant = 1;
  if (!SetCommTimeouts(file, &timeouts))
    system_error("setting port time-outs.");

  // set keyboard to raw reading.
  if (!GetConsoleMode(keyboard, &mode))
    system_error("getting keyboard mode");
  mode &= ~ ENABLE_PROCESSED_INPUT;
  if (!SetConsoleMode(keyboard, mode))
    system_error("setting keyboard mode");

  if (!EscapeCommFunction(file, CLRDTR))
    system_error("clearing DTR");
  Sleep(200);
  if (!EscapeCommFunction(file, SETDTR))
    system_error("setting DTR");

  if ( !WriteFile(file, init, sizeof(init), &written, NULL))
    system_error("writing data to port");

  if (written != sizeof(init))
    system_error("not all data written to port");

  // basic terminal loop:
  do {
    // check for data on port and display it on screen.
    ReadFile(file, buffer, sizeof(buffer), &read, NULL);
    if ( read ){
       
       //printf("%d", read);
       for (DWORD i = 0; i < read; i++){
         printf("%c", buffer[i]);
         SquenceKeywordStrCmp(kw, 2, buffer[i]);
       }
       
       //WriteFile(screen, buffer, read, &written, NULL);
    }
    // check for keypress, and write any out the port.
    if ( _kbhit() ) {
      ch = _getch();
      WriteFile(file, &ch, 1, &written, NULL);
    }
    // until user hits ctrl-backspace.
  } while ( ch != 127);

  // close up and go home.
  CloseHandle(keyboard);
  CloseHandle(file);

  return 0;
}

int AddNewKeyword(keyword_info* kw_info, int index, char* keyword, int kw_size)
{
   kw_info[index].keyword = (char*) malloc(kw_size);
   if (NULL == kw_info[index].keyword){
      printf("Fail to add new keyword\n");
   } else {
      //printf("%p\n", kw_info[index].keyword);
      strcpy(kw_info[index].keyword, keyword);
      //printf("%p\n", kw_info[index].keyword);
      //printf("%c\n", kw_info[index].keyword[1]);
   }

   kw_info[index].keyword = keyword;
   kw_info[index].kw_size = kw_size;
   kw_info[index].match = 0;
   kw_info[index].index = 0;

   return 0;
}
int SquenceKeywordStrCmp(keyword_info* kw_info, int kw_amount, char char_in)
{
   static int index = 0;
   int i = 0;
   for (;i < kw_amount; i++){
      // compare string 
      if (kw_info[i].keyword[kw_info[i].index] == char_in){
         ++kw_info[i].index;
         // match keyword
         if (kw_info[i].index >= kw_info[i].kw_size){
            kw_info[i].match = 1;
            kw_info[i].index = 0;
            printf("[Match]");
         }
      } else {
         kw_info[i].index = 0;
      }
   }
   
   return 0;
}