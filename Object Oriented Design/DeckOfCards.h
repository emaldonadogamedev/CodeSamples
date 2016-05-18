/*
Using C++, create a class for a standard deck of 52 cards. You should include the code for any 
functions that you have declared. Assume that it will be used to play a card game. You will need this 
deck for the next question. You may want to copy it to a text editor before moving to question 2. 

You should use the C\C++ language drop down option to help with syntax.
*/

/*CardDeck.h*/
#pragma once

#include <vector>
#define DECK_SIZE 52

class CardDeck
{
public:
  /*enum to specify the type of card, I decided to declare specify it as a char
  since we'll actually never go beyond a value of 255 or 127 for signed*/
  enum CardType : char
  {
    Spades,
    Hearts,
    Diamonds,
    Clubs,
    Count
  };

  /*structure that represents a card*/
  struct Card
  {
    //card unique ID
    char cardID;

    //card type..
    CardType cardType;

    //card value (-1 to K, joker to King)
    char value;
    
    //card symbol j(for joker), A, 2, 3, ... J, Q, K
    char symbol;
  };

  CardDeck(bool begin_shuffled = false);
  ~CardDeck();

  //Getters
  bool isDeckEmpty() const;

  const Card& lookAtNextCardTop()const;
  const Card& lookAtNextCardBottom()const;

  const Card& getNextCardTop();
  const Card& getNextCardBottom();

  const Card& getNextCardFromIndex(char index, bool nextUnique = false);

  //Setters
  void sortCards(bool ascending_order = true);
  void shuffleCards();

  void placeBackCardTop(char index);
  void placeBackCardBottom(char index);

private:
  //helper functions
  void updateTopIndex();
  void updateBottomIndex();
  void getCardsBackInDeck();

  char m_topIndex;
  char m_bottomIndex;
  bool m_deckSorted;

  bool m_isSortedAsc;

  typedef std::pair<char, bool> cardInfo;
  std::vector<cardInfo> m_cardIndices;

  static const Card m_cards[DECK_SIZE];
};

/*CardDeck.cpp*/
#include "CardDeck.h"

#include<cstdlib> //srand, rand
#include<ctime> //time
#include<algorithm> //random_shuffle
#include<assert.h> //assert

CardDeck::CardDeck(bool begin_shuffled) : m_topIndex(0),
m_bottomIndex(DECK_SIZE - 1), m_deckSorted(false), m_isSortedAsc(false)
{
  /*Give a new seed to the random number generator, 
  this is a constant time function*/
  srand(time(NULL));

  //resize the indices vector
  m_cardIndices.resize(DECK_SIZE);

  //sort the cards, get the info on the vector...
  sortCards();

  //shuffle them if specified
  if (begin_shuffled)
    shuffleCards();
}


CardDeck::~CardDeck()
{
}

bool CardDeck::isDeckEmpty() const
{
  return m_topIndex > m_bottomIndex;
}

const CardDeck::Card& CardDeck::lookAtNextCardTop() const
{
  return m_cards[ m_cardIndices[m_topIndex].first ];
}

const CardDeck::Card& CardDeck::lookAtNextCardBottom() const
{
  return m_cards[ m_cardIndices[m_bottomIndex].first ];
}

const CardDeck::Card& CardDeck::getNextCardTop()
{
  //fail message is deck is empty
  assert(m_topIndex <= m_bottomIndex);

  //set the card as taken
  m_cardIndices[m_topIndex].second = true;
  return m_cards[ m_cardIndices[m_topIndex++].first ];
}

const CardDeck::Card& CardDeck::getNextCardBottom()
{
  //fail message is deck is empty
  assert(m_topIndex <= m_bottomIndex);

  //set the card as taken
  m_cardIndices[m_bottomIndex].second = true;
  return m_cards[ m_cardIndices[m_bottomIndex--].first ];
}

const CardDeck::Card& CardDeck::getNextCardFromIndex(char index, bool nextUnique)
{
  //fail message if index is out of bounds
  assert(index >= 0 && index < DECK_SIZE);

  for (char i = 0; i < DECK_SIZE; ++i)
  {
    if (m_cardIndices[i].first == index)
    {
      //see if we need to check if can get the next card through an index using
      // like in real life, next random card has to one that's not taken
      //by default the search is not Unique
      if (nextUnique)
        assert(m_cardIndices[i].second);

      //set the card as taken
      m_cardIndices[i].second = true;

      //update the top and bottom indices
      updateTopIndex();
      updateBottomIndex();

      //return a reference to the card
      return m_cards[ m_cardIndices[i].first ];
    }
  }
}

void CardDeck::sortCards(bool ascending_order /*= true*/)
{
  //if sorted and in the same order, exit the function
  if (m_deckSorted && m_isSortedAsc == ascending_order)
    return;

  m_isSortedAsc = ascending_order;

  //make adjustments in case we want to order in asc. or desc. order
  char start = 0, end = DECK_SIZE, step = 1;
  if (!m_isSortedAsc)
  {
    start = DECK_SIZE-1;
    end = -1;
    step = -1;
  }

  for (char i = start, j = 0; i != end; i += step)
  {
    //set the sorted index
    m_cardIndices[j].first = i;

    //set the card as not taken(like in life when you sort the deck)
    m_cardIndices[j++].second = false;
  }

  //set the deck as sorted
  m_deckSorted = true;
}

void CardDeck::shuffleCards()
{
  //sort cards, get them back on the deck
  getCardsBackInDeck();

  //shuffle the cards, stl method
  std::random_shuffle(m_cardIndices.begin(), m_cardIndices.end());

  //set the deck as not sorted
  m_deckSorted = false;
}

void CardDeck::updateTopIndex()
{
  for (m_topIndex = 0; m_topIndex < DECK_SIZE + 1; ++m_topIndex)
    //stop index if a non taken card is found
    if (!m_cardIndices[m_topIndex].second)
      break;
}

void CardDeck::updateBottomIndex()
{
  for (m_bottomIndex = DECK_SIZE - 1; m_bottomIndex > -1; --m_bottomIndex)
    //stop index if a non taken card is found
    if (!m_cardIndices[m_bottomIndex].second)
      break;
}

void CardDeck::getCardsBackInDeck()
{
  for (char i = 0; i < DECK_SIZE; ++i)
    //set all cards as not taken
    m_cardIndices[i].second = false;
}

//Definition of the static constant card array, the actual
//"cards" we deal with are actual indices
const CardDeck::Card CardDeck::m_cards[DECK_SIZE] =
{
  //Spades
  { 0, Spades, 1, 'A' },
  { 1, Spades, 2, '2' },
  { 2, Spades, 3, '3' },
  { 3, Spades, 4, '4' },
  { 4, Spades, 5, '5' },
  { 5, Spades, 6, '6' },
  { 6, Spades, 7, '7' },
  { 7, Spades, 8, '8' },
  { 8, Spades, 9, '9' },
  { 9, Spades, 10, 'T' },
  { 10, Spades, 11, 'J' },
  { 11, Spades, 12, 'Q' },
  { 12, Spades, 13, 'K' },
  { 13, Hearts, 1, 'A' },
  { 14, Hearts, 2, '2' },
  { 15, Hearts, 3, '3' },
  { 16, Hearts, 4, '4' },
  { 17, Hearts, 5, '5' },
  { 18, Hearts, 6, '6' },
  { 19, Hearts, 7, '7' },
  { 20, Hearts, 8, '8' },
  { 21, Hearts, 9, '9' },
  { 22, Hearts, 10, 'T' },
  { 23, Hearts, 11, 'J' },
  { 24, Hearts, 12, 'Q' },
  { 25, Hearts, 13, 'K' },
  { 26, Diamonds, 1, 'A' },
  { 27, Diamonds, 2, '2' },
  { 28, Diamonds, 3, '3' },
  { 29, Diamonds, 4, '4' },
  { 30, Diamonds, 5, '5' },
  { 31, Diamonds, 6, '6' },
  { 32, Diamonds, 7, '7' },
  { 33, Diamonds, 8, '8' },
  { 34, Diamonds, 9, '9' },
  { 35, Diamonds, 10, 'T' },
  { 36, Diamonds, 11, 'J' },
  { 37, Diamonds, 12, 'Q' },
  { 38, Diamonds, 13, 'K' },
  { 39, Clubs, 1, 'A' },
  { 40, Clubs, 2, '2' },
  { 41, Clubs, 3, '3' },
  { 42, Clubs, 4, '4' },
  { 43, Clubs, 5, '5' },
  { 44, Clubs, 6, '6' },
  { 45, Clubs, 7, '7' },
  { 46, Clubs, 8, '8' },
  { 47, Clubs, 9, '9' },
  { 48, Clubs, 10, 'T' },
  { 49, Clubs, 11, 'J' },
  { 50, Clubs, 12, 'Q' },
  { 51, Clubs, 13, 'K' }
};