// Deck.cpp - ไฟล์ Source สำหรับ υλολοποίηση คลาส Deck
#include "Deck.h"
#include <iostream>
#include <algorithm> // สำหรับ std::shuffle, std::find_if
#include <random>    // สำหรับ std::default_random_engine
#include <chrono>    // สำหรับ std::chrono::system_clock (ใช้เป็น seed)

// Constructor Implementation
Deck::Deck(const std::vector<Card> &all_cards_pool,
           const std::map<std::string, int> &deck_recipe)
{
  this->cards.clear(); // เริ่มจากเด็คว่างๆ

  for (const auto &pair : deck_recipe)
  {
    const std::string &code_name_to_find = pair.first;
    int quantity = pair.second;

    auto it_pool = std::find_if(all_cards_pool.begin(), all_cards_pool.end(),
                                [&code_name_to_find](const Card &card_in_pool)
                                {
                                  return card_in_pool.getCodeName() == code_name_to_find;
                                });

    if (it_pool != all_cards_pool.end())
    {
      for (int i = 0; i < quantity; ++i)
      {
        this->cards.push_back(*it_pool);
      }
    }
    else
    {
      // Warning นี้อาจจะยังคงไว้ ถ้าการ์ดใน recipe หาไม่เจอใน pool เป็นเรื่องสำคัญ
      // std::cerr << "Warning: ไม่พบการ์ดรหัส '" << code_name_to_find
      //           << "' ในฐานข้อมูลการ์ดทั้งหมด (all_cards_pool) การ์ดนี้จึงไม่ถูกเพิ่มในเด็ค" << std::endl;
    }
  }
  // ลบ output ที่ไม่จำเป็นสำหรับผู้เล่นทั่วไป
  // std::cout << "สร้างเด็คเรียบร้อย มีการ์ดทั้งหมด: " << this->cards.size() << " ใบ" << std::endl;
}

// สับการ์ดในเด็ค
void Deck::shuffle()
{
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(this->cards.begin(), this->cards.end(), std::default_random_engine(seed));
  // ลบ output ที่ไม่จำเป็นสำหรับผู้เล่นทั่วไป
  // std::cout << "เด็คถูกสับแล้ว" << std::endl;
}

// จั่วการ์ดใบบนสุดออกจากเด็ค
std::optional<Card> Deck::draw()
{
  if (this->cards.empty())
  {
    return std::nullopt;
  }
  Card drawn_card = this->cards.back();
  this->cards.pop_back();
  return drawn_card;
}

// ตรวจสอบว่าเด็คหมดหรือยัง
bool Deck::isEmpty() const
{
  return this->cards.empty();
}

// คืนจำนวนการ์ดที่เหลือในเด็ค
size_t Deck::getSize() const
{
  return this->cards.size();
}

// เพิ่มการ์ดเข้าไปใต้เด็ค
void Deck::addCardToBottom(const Card &card)
{
  this->cards.insert(this->cards.begin(), card);
  // ลบ output ที่ไม่จำเป็นสำหรับผู้เล่นทั่วไป
  // std::cout << "เพิ่มการ์ด '" << card.getName() << "' ลงใต้เด็ค" << std::endl;
}

// ค้นหาและดึงการ์ดที่ระบุด้วย code_name ออกจากเด็ค
std::optional<Card> Deck::removeCardByCodeName(const std::string &code_name_to_remove)
{
  auto it = std::find_if(cards.begin(), cards.end(),
                         [&code_name_to_remove](const Card &c)
                         {
                           return c.getCodeName() == code_name_to_remove;
                         });

  if (it != cards.end())
  {
    Card found_card = *it;
    cards.erase(it);
    // ลบ output ที่ไม่จำเป็นสำหรับผู้เล่นทั่วไป
    // std::cout << "การ์ด '" << found_card.getName() << "' (" << code_name_to_remove << ") ถูกดึงออกจากเด็ค" << std::endl;
    return found_card;
  }
  // std::cout << "Warning: ไม่พบการ์ด '" << code_name_to_remove << "' ในเด็คเพื่อดึงออก" << std::endl;
  return std::nullopt;
}

// (For Debugging) แสดงรายชื่อการ์ดในเด็ค
void Deck::printDeckContents() const
{
  std::cout << "--- รายการการ์ดในเด็ค (" << this->cards.size() << " ใบ) ---" << std::endl;
  if (this->cards.empty())
  {
    std::cout << "(เด็คว่างเปล่า)" << std::endl;
  }
  else
  {
    for (const Card &card : this->cards)
    {
      std::cout << card << std::endl;
    }
  }
  std::cout << "------------------------------------" << std::endl;
}
