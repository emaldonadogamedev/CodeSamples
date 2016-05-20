//linked list node definition
typedef struct LinkNode
{
  struct LinkNode	*pPrev;
  struct LinkNode	*pNext;
  int			*pData;	//dynamically allocated data.
} LinkNode_t;

//global pointer to head of linked list
LinkNode_t	*g_pLinkedList;

void KillNode(LinkNode_t *pNodeToRemove)
{
  //if the pointer is null, exit the function early
  if (!pNodeToRemove)
    return;

  //edge case - the node is the head
  if (pNodeToRemove == g_pLinkedList)
  {
    //update the head
    g_pLinkedList = pNodeToRemove->pNext;
    g_pLinkedList->pPrev = nullptr;
    
  }

  //else, the node is somewhere in the middle or in the end...
  else
  {
    //if this is the last node...
    if(!pNodeToRemove->pNext)
      pNodeToRemove->pPrev->pNext = nullptr;
      
    else
    {
      //adjust the pointers accordingly
      pNodeToRemove->pPrev->pNext = pNodeToRemove->pNext;
      pNodeToRemove->pNext->pPrev = pNodeToRemove->pPrev;
    }
  }

  //verify if there's data allocated on the node
  if (pNodeToRemove->pData)
  {
    delete pNodeToRemove->pData;
  }
  delete pNodeToRemove;
}