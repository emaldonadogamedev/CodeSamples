#include <stdio.h>

int ParseHexadecimalString(const char* hexString)
{
  //General assumption(s):
  //The string is properly formatted like 0x....
  
  if (!hexString) return 0; ///additional nullptr check
  if (hexString[1] != 'x' || hexString[0] != '0') return 0;

  //move a reader const char* to the end of the string
  const char* reader = (hexString += 2);
  while (*(reader + 1))
    ++reader;

  //declare result value
  int value = 0; 
  
  
  int increment = 1;
  for (; reader >= hexString; --reader)
  {
    char hexDigit = *reader;

    //if digit is within 0 & 9, consider it normally
    if (hexDigit >= '0' & hexDigit < '9')
    {
      hexDigit -= '0';
    }
    //it was wrong, it did not use an else if
    else if (hexDigit >= 'A' && hexDigit <= 'F')
    {
      //get the digit in range of 10 to 15
      hexDigit = 10 + (hexDigit - 'A');
    }

    else
    {
      return 0;
    }
    value += increment * hexDigit;//value += hexDigit; wrong…
    increment *= 16;

  }
  return value;
}

int main() {
    printf("%x", ParseHexadecimalString("0x5567"));
}