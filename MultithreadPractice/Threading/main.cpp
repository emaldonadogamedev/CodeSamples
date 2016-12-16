#include <iostream>
#include<thread>

using std::thread;
using std::cout;
using std::cin;

void function1()
{
  cout << "OYE PAPAAAAAAAA!!\n";
}

int main(int argc, char** argv)
{
  thread t1(function1);
  //t1.join();//main thread waits for t1 to finish
  t1.detach();//removes the connection between main and t1 -- daemon process

  //--RULE--
  //you can only join or detach only once
  //t1.join() // crash

  if (t1.joinable())
    t1.join(); //crash.

  return 0;
}