// Deck.h - ไฟล์ Header สำหรับคลาส Deck
#ifndef DECK_H
#define DECK_H

#include <vector>
#include <string>
#include <map>
#include <optional> // สำหรับ std::optional ในฟังก์ชัน draw()
#include "Card.h"   // คลาส Card ที่เราสร้างไว้

class Deck
{
private:
  std::vector<Card> cards;

public:
  Deck(const std::vector<Card> &all_cards_pool,
       const std::map<std::string, int> &deck_recipe);

  void shuffle();
  std::optional<Card> draw();
  bool isEmpty() const;
  size_t getSize() const;
  void addCardToBottom(const Card &card); // สำหรับ Mulligan หรือสกิล
  std::optional<Card> removeCardByCodeName(const std::string &code_name_to_remove);
  void addCardsToBottom(const std::vector<Card> &cards_to_add); // สำหรับ Mulligan

  void printDeckContents() const; // For Debugging
};

#endif // DECK_H
