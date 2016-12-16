#include <iostream>
#include<thread>

using std::string;
using std::cout;
using std::cin;
using std::endl;

void function1()
{
  cout << "OYE PAPAAAAAAAA!!\n";
}

//thread objects can be used for functiors and lambas
class Fctor 
{
public:
  
  //RULE
  //Even if parameters are passed by ref. a thread always passes by value...dangerous
  void operator()(string& msg) 
  {
    for (int i = 0; i > -100; --i)
    {
      cout << "t1 says: " << msg.c_str() << endl;
      msg = "Cambiame esto pai!";
    }
  }
};

int main(int argc, char** argv)
{
  string s = "Un mensajito pa ti mami";
  Fctor fc1;
  //std::thread t1(fc1); //t1 starts running
  
  //Another way of passing the functor as the parameter
  //WRONG WAY -> std::thread t1( Fctor() ); //this will not compile, needs a func ptr.

  //CORRECT, the extra parenthesis tell the compiler that it's explicitally a functor
  //std::thread t1( (Fctor()), s );

  //RULE
  //CORRECT, but we have to use std::ref(..) in order to pass a parameter by reference
  std::thread t1((Fctor()), std::ref(s) );

#define FIRST_PART 0
#define SECOND_PART 1
#if FIRST_PART
  try
  {
    for (int i = 0; i < 100; ++i)
    {
      cout << "from main: " << s.c_str() << endl;
    }
  }
  catch (...)
  {
    t1.join();
    throw;
  }


  t1.join();
#endif

#if SECOND_PART
  t1.join();
  cout << "from main: " << s.c_str() << endl;
#endif

  return 0;

}