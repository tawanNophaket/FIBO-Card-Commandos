// Card.cpp - ไฟล์ Source สำหรับ υλολοποίηση คลาส Card
#include "Card.h" // ต้อง include header file ของคลาสตัวเอง

// Constructor Implementation
Card::Card(const std::string &code_name,
           const std::string &name,
           int grade,
           int power,
           int shield,
           const std::string &skill_description,
           const std::string &type_role)
    : code_name(code_name), name(name), grade(grade), power(power),
      shield(shield), skill_description(skill_description), type_role(type_role)
{
  // สามารถเพิ่มโค้ดเริ่มต้นอื่นๆ ใน constructor ได้ถ้าจำเป็น
  // std::cout << "Card '" << this->name << "' created." << std::endl; // สำหรับ debug
}

// Destructor Implementation
Card::~Card()
{
  // โค้ดสำหรับคืน memory หรือทรัพยากรอื่นๆ ถ้ามีการจองไว้แบบ dynamic
  // ในคลาสนี้ยังไม่มีการจอง memory แบบ dynamic จึงอาจจะไม่ต้องทำอะไร
  // std::cout << "Card '" << this->name << "' destroyed." << std::endl; // สำหรับ debug
}

// Getter methods Implementation
std::string Card::getCodeName() const
{
  return code_name;
}

std::string Card::getName() const
{
  return name;
}

int Card::getGrade() const
{
  return grade;
}

int Card::getPower() const
{
  return power;
}

int Card::getShield() const
{
  return shield;
}

std::string Card::getSkillDescription() const
{
  return skill_description;
}

std::string Card::getTypeRole() const
{
  return type_role;
}

// Member function สำหรับแสดงข้อมูลการ์ด Implementation
void Card::displayInfo() const
{
  std::cout << "------------------------------------" << std::endl;
  std::cout << "รหัสการ์ด: " << code_name << std::endl;
  std::cout << "ชื่อการ์ด: " << name << " (G" << grade << ")" << std::endl;
  std::cout << "ประเภท/บทบาท: " << type_role << std::endl;
  std::cout << "พลัง: " << power << " / โล่ห์: " << shield << std::endl;
  std::cout << "สกิล: " << skill_description << std::endl;
  std::cout << "------------------------------------" << std::endl;
}

// Overload a << operator Implementation
std::ostream &operator<<(std::ostream &os, const Card &card)
{
  os << "[" << card.code_name << "] " << card.name << " (G" << card.grade << ")";
  return os;
}