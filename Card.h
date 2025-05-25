// Card.h - ไฟล์ Header สำหรับคลาส Card
#ifndef CARD_H
#define CARD_H

#include <string>
#include <iostream> // สำหรับ std::cout ใน displayInfo และ operator<<

class Card
{
private:
  std::string code_name;         // รหัสการ์ด เช่น "G0-01"
  std::string name;              // ชื่อการ์ด เช่น "เบเบี้ตี๋"
  int grade;                     // เกรดของการ์ด (0, 1, 2, 3, 4)
  int power;                     // พลังโจมตี
  int shield;                    // ค่าโล่ห์
  std::string skill_description; // คำอธิบายสกิล
  std::string type_role;         // ประเภท/บทบาทของการ์ด เช่น "Starter", "Trigger - Heal"
  int critical;                  // <<< เพิ่มค่า Critical

public:
  // Constructor
  Card(const std::string &code_name,
       const std::string &name,
       int grade,
       int power,
       int shield,
       const std::string &skill_description,
       const std::string &type_role,
       int critical); // <<< เพิ่ม critical ใน constructor

  // Destructor
  ~Card();

  // Getter methods
  std::string getCodeName() const;
  std::string getName() const;
  int getGrade() const;
  int getPower() const;
  int getShield() const;
  std::string getSkillDescription() const;
  std::string getTypeRole() const;
  int getCritical() const; // <<< เพิ่ม getter สำหรับ critical

  // Member function สำหรับแสดงข้อมูลการ์ด
  void displayInfo() const;

  friend std::ostream &operator<<(std::ostream &os, const Card &card);
};

#endif // CARD_H
