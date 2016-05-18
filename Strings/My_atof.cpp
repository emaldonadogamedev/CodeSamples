#include <stdio.h>

float My_atof(char* aValue)
{
  /*Note: For the purposes of this problem I am assuming the following:
  1) The aValue pointer is properly formatted and does not have any
  non numerical characters other than the '.' and that it may contain
  an f at the end exclusively. Ex: 12.09f, -1.009f, 0.009f, -655f, 0f, 0, etc.

  2) The string is null terminated
  */
  if (!aValue)
    //assumption: if the pointer is a null value then we return 0.0f
    //In other cases we could've used assert, an exception or simply return another value
    return 0;

  float result = 0;
  float multiplier = 1.0f;
  
  //get a pointer to the end, the decimal point or the end of the string (null)
  char* integerPart = aValue, *decimalPart = 0;
  while (*integerPart && !(*integerPart == '.' || *integerPart == 'f'))
    ++integerPart;

  //prepare the decimal part pointer if we need it
  if (*integerPart == '.')
    decimalPart = integerPart + 1;

  //set back the integer part pointer back by 1 to have it look at a number
  --integerPart;

  float temp = 1.0f;

  //add all of the integer part of the number
  while (aValue < integerPart)
  {
    result += temp * (float)(*integerPart - '0');
    temp *= 10.0f;
    --integerPart;
  }
  
  //if the first character is a negative sign, then change the multiplier.
  if (*aValue == '-')
    multiplier = -1.0f;
  
  //else add the final number to the result
  else
    result += temp * (float)(*aValue - '0');

  //Now add all of the decimal parts of the number
  temp = 1.0f / 10.0f;
  while (decimalPart && *decimalPart && *decimalPart != 'f')
  {
    result += temp * (float)(*decimalPart - '0');
    temp /= 10.0f;
    *decimalPart++;
  }
  
  //lastly, apply the multiplier to make sure to have the right sign
  return result * multiplier;
}

int main() 
{
    printf("%f\n", My_atof("-987873.01f"));
    printf("%f\n", My_atof("987873.01f"));
    printf("%f\n", My_atof("-0.065765765"));
    printf("%f\n", My_atof("0"));
    printf("%f\n", My_atof("-7"));
    
    return 0;
}