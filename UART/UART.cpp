#include<stdafx.h>

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

HANDLE hSerialIn = INVALID_HANDLE_VALUE;
DCB config = {0};
DWORD WINAPI GetLastError(void);
LPCWSTR lpFileName;
long	lLastError = ERROR_SUCCESS;
char	portIn[16];
char	lastError[1024],buf1[100];
int		ReadUart(int len,HANDLE);


int _tmain(int argc, _TCHAR* argv[])
{
	sprintf(portIn,"COM4:");
	lpFileName = (LPCWSTR)portIn;
	printf("lpFileName: %s\n", lpFileName);

	hSerialIn = CreateFile(_T("COM4"),
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 
		NULL); 

	if(hSerialIn==INVALID_HANDLE_VALUE)
	{
		if(GetLastError()==ERROR_FILE_NOT_FOUND)
		{
			printf("\nError: \nThe system cannot find the file specified (%s)\n",portIn);		//error code 0x02
		}
		else if(GetLastError()==ERROR_INVALID_NAME)
		{
			printf("\nError: \n%s 'filename, directory name, or volume label syntax is incorrect'\n",portIn);		//error code 0x7B
		}
		else
		{
			printf("\nHandle creation error code: %x\n", GetLastError());
		}
		puts("\t...CreateFile returned an invalid handle value");
	}
  config.DCBlength = sizeof(config);


    if((GetCommState(hSerialIn, &config) == 0))
    {
        printf("Get configuration port has a problem.");
        return FALSE;
    }

    config.BaudRate = 115200;
    config.StopBits = ONESTOPBIT;
    config.Parity = PARITY_NONE; 
    config.ByteSize = DATABITS_8;
    config.fDtrControl = 0;
    config.fRtsControl = 0;

    if (!SetCommState(hSerialIn, &config))
    {

        printf( "Failed to Set Comm State Reason: %d\n",GetLastError());
        //return E_FAIL;
    }

    printf("Current Settings\n Baud Rate %d\n Parity %d\n Byte Size %d\n Stop Bits %d", config.BaudRate, 
        config.Parity, config.ByteSize, config.StopBits);



	while(1){
		int len = ReadUart(sizeof(buf1), hSerialIn);
		/*
		for (int i =0; i < len; i++)
		{
			printf("%c", buf1[i]);	
		}
		*/
	}
	CloseHandle(hSerialIn);
	
	puts("\npress any key to exit...");
	getchar();

	return 1;
}
int ReadUart(int len, HANDLE hPort)
{
	BOOL ret;
	DWORD dwRead;
    BOOL fWaitingOnRead = FALSE;
    OVERLAPPED osReader = {0};
    unsigned long retlen=0;

   // Create the overlapped event. Must be closed before exiting to avoid a handle leak.

   osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
   if (osReader.hEvent == NULL)
       MessageBox (NULL, L"Error in creating Overlapped event" ,L"Error", MB_OK);
   if (!fWaitingOnRead)
   {
          if (!ReadFile(hPort, buf2, len, &dwRead,  &osReader)) 

          {
			
          FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPWSTR)lastError,
                        1024,
                        NULL);
		   MessageBox (NULL, (LPWSTR)lastError ,L"MESSAGE", MB_OK);
            
           }
           else
		   {
             
	          //MessageBox (NULL, L"ReadFile Suceess" ,L"Success", MB_OK);
           }
        
    }



 
	
	if(dwRead > 0)	
	{
		//MessageBox (NULL, L"Read DATA Success" ,L"Success", MB_OK);//If we have data
		printf("%d\n", retlen);
		return (int) retlen;
	}
	     //return the length
    
	else return 0;     //else no data has been read
 }
