// Card.h - ไฟล์ Header สำหรับคลาส Card
#ifndef CARD_H
#define CARD_H

#include <string>
#include <iostream>

// คลาส Card - เป็นคลาสที่ใช้เก็บข้อมูลการ์ดแต่ละใบในเกม
class Card
{
private:
  // ตัวแปรสมาชิกสำหรับเก็บคุณสมบัติต่างๆ ของการ์ด
  std::string code_name;         // รหัสการ์ด
  std::string name;              // ชื่อการ์ด
  int grade;                     // เกรดของการ์ด
  int power;                     // ค่าพลังโจมตี
  int shield;                    // ค่าการป้องกัน
  std::string skill_description; // คำอธิบายความสามารถพิเศษ
  std::string type_role;         // ประเภทของการ์ด
  int critical;                  // ค่าโจมตีคริติคอล

public:
  // Constructor - สร้างการ์ดใหม่พร้อมกำหนดค่าเริ่มต้น
  Card(const std::string &code_name,
       const std::string &name,
       int grade,
       int power,
       int shield,
       const std::string &skill_description,
       const std::string &type_role,
       int critical);

  // Destructor - ทำลายออบเจ็กต์การ์ดเมื่อไม่ได้ใช้งาน
  ~Card();

  // เมธอดสำหรับดึงข้อมูลต่างๆ ของการ์ด (Getter methods)
  std::string getCodeName() const;         // ดึงรหัสการ์ด
  std::string getName() const;             // ดึงชื่อการ์ด
  int getGrade() const;                    // ดึงเกรดการ์ด
  int getPower() const;                    // ดึงค่าพลังโจมตี
  int getShield() const;                   // ดึงค่าการป้องกัน
  std::string getSkillDescription() const; // ดึงคำอธิบายสกิล
  std::string getTypeRole() const;         // ดึงประเภทการ์ด
  int getCritical() const;                 // ดึงค่าคริติคอล

  // แสดงข้อมูลการ์ดทั้งหมด
  void displayInfo() const;

  // ฟังก์ชันเพื่อน สำหรับแสดงผลการ์ดในรูปแบบที่กำหนดเอง
  friend std::ostream &operator<<(std::ostream &os, const Card &card);
};

#endif // CARD_H
