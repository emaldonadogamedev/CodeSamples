#include"TestApp.h"

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
  srand(static_cast<unsigned int>(time(NULL)));

  TestApp tApp(hInstance);

  if ( !tApp.Init() )
    return -1;

  return tApp.Run();
}