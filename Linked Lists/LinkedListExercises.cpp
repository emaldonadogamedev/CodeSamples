#include<iostream>
#include<algorithm>
#include<unordered_map>
#include<map>
#include<chrono>


struct TreeNode
{
  TreeNode(int v = 0) :left(nullptr), right(nullptr), data(v){}

  TreeNode *left, *right;
  int data;
};

struct ListNode
{
  ListNode(int v = 0) :next(nullptr), data(v){}

  ListNode *next;
  int data;
};

void insertToFrontOfList(ListNode** head, int data);
void insertToBackOfList(ListNode** head, int data);
void insertInTree(TreeNode*& root, int data);
void removeDuplicates(ListNode* head);
void removeDuplicatesNoMemory(ListNode* head);
void clearList(ListNode* head);
void printList(ListNode* head);
int getTreeHeight(TreeNode* root);

/*
Given a binary tree, design an algorithm which creates a linked list of all the
nodes at each depth (e.g., if you have a tree with depth D,you'll have D linked
lists).
*/
void TreeDepthLists(TreeNode* root, ListNode* lists[], int level);

/*Given a sorted (increasingorder) array with unique integer elements, write an
algorithm to createa binary search tree with minimal height.*/
void CreateTreeFromSortedArray(TreeNode*& root, int* arr, const int size, const int begin_index, const int end_index);

int main(int argc, char** argv)
{

  ListNode* temp = nullptr;
  for (int i = 0; i < 1000000; ++i)
  {
    insertToFrontOfList(&temp, 1 + (rand() % 8));
  }

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  removeDuplicates(temp);
  end = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout <<  "With memory elapsed time: " << elapsed_seconds.count() << "s\n";

  clearList(temp);
  temp = nullptr;

  for (int i = 0; i < 1000000; ++i)
  {
    insertToFrontOfList(&temp, 1 + (rand() % 8));
  }

  start = std::chrono::system_clock::now();
  removeDuplicatesNoMemory(temp);
  end = std::chrono::system_clock::now();
  elapsed_seconds = end - start;
  std::cout << "No memory elapsed time: " << elapsed_seconds.count() << "s\n";
  //std::cout << "No memory elapsed time: " << elapsed_seconds.count() << "s\n";

  clearList(temp);
  temp = nullptr;

  return 0;
}

int getTreeHeight(TreeNode* root)
{
  if (!root)
    return -1;

  return 1 + std::max(getTreeHeight(root->left), getTreeHeight(root->right));
}

void insertToFrontOfList(ListNode** head, int data)
{
  if (!*head)
  {
    *head = new ListNode(data);
    return;
  }

  ListNode* temp = new ListNode(data);
  temp->next = *head;

  *head = temp;
}

/*NOTE: this is assumming there's no tail pointer*/
void insertToBackOfList(ListNode** head, int data)
{
  if (!*head)
  {
    *head = new ListNode(data);
    return;
  }

  ListNode* temp = *head;
  while (temp->next)
  {
    temp = temp->next;
  }
  temp->next = new ListNode(data);
}

void insertInTree(TreeNode*& root, int data)
{
  if (root == nullptr)
    root = new TreeNode(data);

  else if (data < root->data)
    insertInTree(root->left, data);

  else if (data > root->data)
    insertInTree(root->right, data);

  //if we reached this point then the data is already on the tree
  //else
  //  return;
}

void removeDuplicates(ListNode* head)
{
  if (!head && head->next)
    return;

  std::unordered_map<int, int> duplicates;

  ListNode* temp = head;
  head = head->next;
  ++duplicates[temp->data];

  while (head)
  {
    if ( duplicates[head->data]++ == 0)
    {
      temp = head;
      head = head->next;
      continue;
    }

    //we found a duplicate...
    temp->next = head->next;
    delete head;
    head = temp->next;
  }
}

void removeDuplicatesNoMemory(ListNode* head)
{
  if (!head && head->next)
    return;

  while (head)
  {
    ListNode* temp_head = head;
    ListNode* iterator = head->next;
    while (iterator)
    {
      if (iterator->data == head->data)
      {
        temp_head->next = iterator->next;
        delete iterator;
        iterator = temp_head->next;
        continue;
      }

      temp_head = iterator;
      iterator = iterator->next;
    }
    head = head->next;
  }
}

void clearList(ListNode* head)
{
  ListNode* temp = head;
  while (head)
  {
    temp = head->next;
    delete head;
    head = temp;
  }
}

void printList(ListNode* head)
{
  while (head)
  {
    std::cout << head->data << ", ";
    head = head->next;
  }
  std::cout << "\n";
}

void TreeDepthLists(TreeNode* root, ListNode* lists[], int level)
{
  if (!root)
    return;

  insertToBackOfList(&lists[level], root->data);

  TreeDepthLists(root->left, lists, level + 1);
  TreeDepthLists(root->right, lists, level + 1);
}

void CreateTreeFromSortedArray(TreeNode*& root, int* arr, const int size, const int begin_index, const int end_index)
{
  /*Given a sorted (increasingorder) array with unique integer elements, write an
  algorithm to createa binary search tree with minimal height.*/

  if (arr == nullptr || size == 0 || begin_index > end_index)
    return;

  const int middle_index = begin_index + ((end_index - begin_index) / 2);

  insertInTree(root, arr[middle_index]);

  //insert the new root on the left
  CreateTreeFromSortedArray(root, arr, size, begin_index, middle_index - 1);

  //insert the new root on the right
  CreateTreeFromSortedArray(root, arr, size, middle_index + 1, end_index);
}