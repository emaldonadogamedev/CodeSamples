/*
Using C++ and the card deck class declared in DeckOfCards.h, write a function that deals
5 cards from your deck to a variable number of players. Treat face cards as having a value of 10. 
Return which player has the highest sum for his cards and that sum. 

Please comment any assumptions that you are making and any design concerns that you may have.
*/

int fiveCardMatch(int numOfPlayers, CardDeck& deck)
{
  //General Assumptions:
  //name of function is fiveCardMatch
  //syntax for variables and functions is camel case
  //no {} on single line if statements
  //The function takes in a reference to a deck and
  // the number of players as an int
  //the return value of the function is an int

  //Assumption:
  //If we have an unreasonable amount of player, then exit with -1
  //if we have 11 players then we'd need 55 cards, which we don't have
  if (numOfPlayers < 0 || numOfPlayers > 10)
    return -1;

  //Assumption: the index of player is one based.
  int winner = -1, winSum = 0, currentSum = 0;

  //shuffle the cards
  deck.shuffleCards();

  for (int player = 1; player <= numOfPlayers; ++player)
  {
    //add up all of the card values
    for (int j = 0; j < 5; ++j)
    {
      //Assumption: get all cards from the top
      const CardDeck::Card& currentCard = deck.getNextCardTop();

      //add 10 if the value is J, Q, or K... simply if it's greater than 9
      currentSum += currentCard.value > 9 ? 10 : currentCard.value;
    }

    //if the currentSum is greater than the winner's sum then update
    // the player number
    if (currentSum > winSum)
    {
      winner = player;
      winSum = currentSum;
    }
    
    //reset current sum
    currentSum = 0;
  }

  //return the player number
  return winner;
}