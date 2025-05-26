// Card.cpp - ไฟล์ Source สำหรับการดำเนินการของคลาส Card
#include "Card.h"

// Constructor - สร้างออบเจ็กต์การ์ดใหม่
// รับพารามิเตอร์ทั้งหมดที่จำเป็นสำหรับการสร้างการ์ด
// และกำหนดค่าเริ่มต้นให้กับตัวแปรสมาชิกทั้งหมด
Card::Card(const std::string &code_name,
           const std::string &name,
           int grade,
           int power,
           int shield,
           const std::string &skill_description,
           const std::string &type_role,
           int critical)
    : code_name(code_name), name(name), grade(grade), power(power),
      shield(shield), skill_description(skill_description), type_role(type_role),
      critical(critical)
{
}

// Destructor - ทำความสะอาดหน่วยความจำเมื่อออบเจ็กต์ถูกทำลาย
Card::~Card()
{
}

// เมธอดสำหรับเรียกดูข้อมูลต่างๆ ของการ์ด (Getter methods)

// ดึงรหัสการ์ด
std::string Card::getCodeName() const
{
  return code_name;
}

// ดึงชื่อการ์ด
std::string Card::getName() const
{
  return name;
}

// ดึงเกรดของการ์ด
int Card::getGrade() const
{
  return grade;
}

// ดึงค่าพลังโจมตี
int Card::getPower() const
{
  return power;
}

// ดึงค่าการป้องกัน
int Card::getShield() const
{
  return shield;
}

// ดึงคำอธิบายความสามารถพิเศษ
std::string Card::getSkillDescription() const
{
  return skill_description;
}

// ดึงประเภทของการ์ด
std::string Card::getTypeRole() const
{
  return type_role;
}

// ดึงค่าคริติคอล
int Card::getCritical() const
{
  return critical;
}

// แสดงข้อมูลการ์ดทั้งหมดในรูปแบบที่อ่านง่าย
// แสดงผลทางหน้าจอโดยใช้เส้นขีดเป็นกรอบ
void Card::displayInfo() const
{
  std::cout << "------------------------------------" << std::endl;
  std::cout << "รหัสการ์ด: " << code_name << std::endl;
  std::cout << "ชื่อการ์ด: " << name << " (G" << grade << ")" << std::endl;
  std::cout << "ประเภท/บทบาท: " << type_role << std::endl;
  std::cout << "พลัง: " << power << " / โล่ห์: " << shield << " / คริติคอล: " << critical << std::endl;
  std::cout << "สกิล: " << skill_description << std::endl;
  std::cout << "------------------------------------" << std::endl;
}

// โอเวอร์โหลดตัวดำเนินการ << สำหรับแสดงข้อมูลการ์ดแบบย่อ
// ใช้สำหรับแสดงข้อมูลการ์ดในรูปแบบบรรทัดเดียว
std::ostream &operator<<(std::ostream &os, const Card &card)
{
  os << "[" << card.code_name << "] " << card.name << " (G" << card.grade << " C" << card.critical << ")";
  return os;
}
