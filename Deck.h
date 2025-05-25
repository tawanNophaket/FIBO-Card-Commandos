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
  std::vector<Card> cards; // เก็บการ์ดทั้งหมดในเด็ค (การ์ดจะถูก copy มา)

public:
  // Constructor: สร้างเด็คจากฐานข้อมูลการ์ดทั้งหมดและสูตรการจัดเด็ค
  // deck_recipe: key คือ code_name, value คือจำนวนการ์ดนั้นๆ ในเด็ค
  Deck(const std::vector<Card> &all_cards_pool,
       const std::map<std::string, int> &deck_recipe);

  // สับการ์ดในเด็ค
  void shuffle();

  // จั่วการ์ดใบบนสุดออกจากเด็ค
  // คืนค่า std::optional<Card> เพื่อจัดการกรณีเด็คหมดได้
  std::optional<Card> draw();

  // ตรวจสอบว่าเด็คหมดหรือยัง
  bool isEmpty() const;

  // คืนจำนวนการ์ดที่เหลือในเด็ค
  size_t getSize() const;

  // (Optional) เพิ่มการ์ดเข้าไปในเด็ค (อาจจะใช้สำหรับบางสกิล)
  void addCardToBottom(const Card &card);

  // ค้นหาและดึงการ์ดที่ระบุด้วย code_name ออกจากเด็ค
  // มีประโยชน์สำหรับการตั้งค่า Starter Vanguard
  std::optional<Card> removeCardByCodeName(const std::string &code_name_to_remove);

  // (For Debugging) แสดงรายชื่อการ์ดในเด็ค
  void printDeckContents() const;
};

#endif // DECK_H
