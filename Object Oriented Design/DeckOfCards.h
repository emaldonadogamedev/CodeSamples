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

  /*structure that represents a card (just 4 bytes long)*/
  struct Card
  {
    //card unique ID
    char cardID;

    //card type..
    CardType cardType;

    //card value (1 to K, joker to King)
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