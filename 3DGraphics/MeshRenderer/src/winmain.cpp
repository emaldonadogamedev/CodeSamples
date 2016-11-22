#include"TestApp.h"

void possibleWays(int x, int y, int& width, int& height, int& count)
{
  if (x + 1 == width && y + 1 == height)
  {
    ++count;
    return;
  }

  if (x + 1 < width)
    possibleWays(x + 1, y, width, height, count);

  if (y + 1 < height)
    possibleWays(x, y + 1, width, height, count);
}

int isAnagram(char* s1, char* s2)
{
  if (!s1 && !s2)
    return 1;

  if (!s1 || !s2)
    return 0;

  int arr[26] = { 0 };
  char temp;
  
  while (*s1)
  {
    temp = *s1;
    if (temp >= 'a' && temp <= 'z')
      arr[temp - ('A' + 32)]++;
    
    else if (temp >= 'A' && temp <= 'Z')
      arr[temp - 'A']++;

    ++s1;
  }

  while (*s2)
  {
    temp = *s2;
    if (temp >= 'a' && temp <= 'z')
      arr[temp - ('A' + 32)]--;
    
    else if (temp >= 'A' && temp <= 'Z')
      arr[temp - 'A']--;
    
    ++s2;
  }

  for (int i = 0; i < 26; ++i)
  {
    if (arr[i])
      return 0;
  }

  return 1;
}

struct TreeNode
{
  int data;
  TreeNode* left, *right;

  TreeNode(int data):left(0),right(0){}
};

/*isBinaryTreeSearch - determines whether the tree pointer points
// to a valid Binary Search Tree*/
int isBinarySearchTree(TreeNode* tree)
{
  // the result variable
  bool result = true;

  //check if the pointer is not null
  if (tree)
  {
    //if we have a left node
    if (tree->left)
    {
      //check if the data of the current node is greater than the
      //data on the left node
      if (tree->left->data < tree->data)
        //we use && to accumulate the results of all of the recursive function calls
        result = result && isBinarySearchTree(tree->left);

      //if not, then return false. The test failed
      else
        return false;
    }

    //if we have a right node
    if (tree->right)
    {
      //check is the data of the current node is less than the
      //data on the right node
      if (tree->right->data > tree->data)
        //we use && to accumulate the results of all of the recursive function calls
        result = result && isBinarySearchTree(tree->right);

      //if not, then return false. The test failed
      else
        return false;
    }
  }

  //Base case: return the accumulated result
  return result;
}

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{

#define RUN_RENDERER 1

#if !RUN_RENDERER
  int x = 0, y = 0, w = 3, h = 3;
  int c = 0;
  possibleWays(x, y, w, h, c);

  h = isAnagram("ahha", "hhaa");
#endif


#if RUN_RENDERER
  srand(static_cast<unsigned int>(time(NULL)));

  TestApp tApp(hInstance);

  if ( !tApp.Init() )
    return -1;

  return tApp.Run();
#endif
}