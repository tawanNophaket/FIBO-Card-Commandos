// Deck.cpp - ไฟล์ Source สำหรับการดำเนินการของคลาส Deck
#include "Deck.h"
#include <iostream>
#include <algorithm> // สำหรับฟังก์ชัน std::shuffle และ std::find_if
#include <random>    // สำหรับตัวสร้างเลขสุ่ม (random number generator)
#include <chrono>    // สำหรับจับเวลาระบบใช้เป็นเมล็ดพันธุ์ในการสุ่ม

// Constructor - สร้างสำรับไพ่จากคลังการ์ดและสูตรที่กำหนด
// all_cards_pool: คลังการ์ดทั้งหมดที่มีในเกม
// deck_recipe: แผนผังการ์ดที่ต้องการ (รหัสการ์ด -> จำนวนที่ต้องการ)
Deck::Deck(const std::vector<Card> &all_cards_pool,
           const std::map<std::string, int> &deck_recipe)
{
  this->cards.clear(); // ล้างสำรับให้ว่างก่อนเริ่มต้น

  // วนลูปผ่านทุกคู่ของรหัสการ์ดและจำนวนที่ต้องการจากสูตร
  for (const auto &pair : deck_recipe)
  {
    const std::string &code_name_to_find = pair.first;
    int quantity = pair.second;

    // ค้นหาการ์ดในคลังด้วยรหัส
    auto it_pool = std::find_if(all_cards_pool.begin(), all_cards_pool.end(),
                                [&code_name_to_find](const Card &card_in_pool)
                                {
                                  return card_in_pool.getCodeName() == code_name_to_find;
                                });

    // ถ้าพบการ์ดในคลัง เพิ่มเข้าสำรับตามจำนวนที่ต้องการ
    if (it_pool != all_cards_pool.end())
    {
      for (int i = 0; i < quantity; ++i)
      {
        this->cards.push_back(*it_pool);
      }
    }
  }
}

// สับการ์ดในสำรับให้สุ่ม โดยใช้เวลาปัจจุบันเป็นเมล็ดพันธุ์
void Deck::shuffle()
{
  // สร้างเมล็ดพันธุ์สำหรับการสุ่มจากเวลาระบบ
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  // สับการ์ดแบบสุ่มด้วยเมล็ดพันธุ์ที่สร้าง
  std::shuffle(this->cards.begin(), this->cards.end(), std::default_random_engine(seed));
}

// จั่วการ์ดใบบนสุดของสำรับ
// คืนค่า: การ์ดที่จั่วได้ หรือ std::nullopt ถ้าไม่มีการ์ดเหลือ
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

// ตรวจสอบว่าสำรับว่างหรือไม่
bool Deck::isEmpty() const
{
  return this->cards.empty();
}

// ดูจำนวนการ์ดที่เหลือในสำรับ
size_t Deck::getSize() const
{
  return this->cards.size();
}

// เพิ่มการ์ดหนึ่งใบไปที่ก้นสำรับ
void Deck::addCardToBottom(const Card &card)
{
  this->cards.insert(this->cards.begin(), card);
}

// เพิ่มการ์ดหลายใบไปที่ก้นสำรับพร้อมกัน
void Deck::addCardsToBottom(const std::vector<Card> &cards_to_add)
{
  this->cards.insert(this->cards.begin(), cards_to_add.begin(), cards_to_add.end());
}

// ค้นหาและลบการ์ดที่มีรหัสตรงกับที่ระบุออกจากสำรับ
// คืนค่า: การ์ดที่ลบออก หรือ std::nullopt ถ้าไม่พบการ์ด
std::optional<Card> Deck::removeCardByCodeName(const std::string &code_name_to_remove)
{
  // ค้นหาการ์ดในสำรับที่มีรหัสตรงกับที่ต้องการ
  auto it = std::find_if(cards.begin(), cards.end(),
                         [&code_name_to_remove](const Card &c)
                         {
                           return c.getCodeName() == code_name_to_remove;
                         });

  // ถ้าพบการ์ด
  if (it != cards.end())
  {
    Card found_card = *it;
    cards.erase(it);   // ลบการ์ดออกจากสำรับ
    return found_card; // ส่งการ์ดที่ลบกลับไป
  }

  // ถ้าไม่พบการ์ด
  return std::nullopt;
}

// แสดงรายละเอียดการ์ดทั้งหมดในสำรับ (สำหรับการดีบั๊ก)
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
