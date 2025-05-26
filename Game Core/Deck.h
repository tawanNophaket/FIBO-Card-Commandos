// Deck.h - ไฟล์ Header สำหรับคลาส Deck
#ifndef DECK_H
#define DECK_H

#include <vector>
#include <string>
#include <map>
#include <optional> // ใช้สำหรับฟังก์ชันที่อาจจะไม่ส่งค่ากลับมา เช่น เมื่อไพ่หมด
#include "Card.h"   // include คลาส Card ที่ใช้เป็นองค์ประกอบของสำรับ

// คลาส Deck - จัดการสำรับไพ่ทั้งหมดของผู้เล่น
class Deck
{
private:
  std::vector<Card> cards; // เก็บการ์ดทั้งหมดในสำรับในรูปแบบ vector

public:
  // Constructor - สร้างสำรับไพ่จากคลังการ์ดและสูตรการ์ดที่กำหนด
  // all_cards_pool: คลังการ์ดทั้งหมดที่มีในเกม
  // deck_recipe: แผนผังการ์ดที่ต้องการใส่ในสำรับ (รหัสการ์ด -> จำนวนที่ต้องการ)
  Deck(const std::vector<Card> &all_cards_pool,
       const std::map<std::string, int> &deck_recipe);

  void shuffle();                         // สับการ์ดในสำรับให้สุ่ม
  std::optional<Card> draw();             // จั่วการ์ดจากบนสุดของสำรับ (ถ้าไม่มีการ์ดจะคืนค่า empty)
  bool isEmpty() const;                   // ตรวจสอบว่าสำรับว่างหรือไม่
  size_t getSize() const;                 // ดูจำนวนการ์ดที่เหลือในสำรับ
  void addCardToBottom(const Card &card); // เพิ่มการ์ดไปที่ก้นสำรับ

  // ลบการ์ดที่ต้องการออกจากสำรับโดยใช้รหัสการ์ด
  // คืนค่าการ์ดที่ลบถ้าพบ หรือ empty ถ้าไม่พบ
  std::optional<Card> removeCardByCodeName(const std::string &code_name_to_remove);

  // เพิ่มการ์ดหลายใบไปที่ก้นสำรับ
  void addCardsToBottom(const std::vector<Card> &cards_to_add);

  void printDeckContents() const; // แสดงรายละเอียดการ์ดทั้งหมดในสำรับ (ใช้สำหรับดีบั๊ก)
};

#endif // DECK_H
