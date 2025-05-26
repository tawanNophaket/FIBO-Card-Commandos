// Card.cpp - ไฟล์ Source สำหรับ υλολοποίηση คลาส Card
#include "Card.h"

// Constructor Implementation
Card::Card(const std::string &code_name,
           const std::string &name,
           int grade,
           int power,
           int shield,
           const std::string &skill_description,
           const std::string &type_role,
           int critical) // <<< เพิ่ม critical ใน constructor
    : code_name(code_name), name(name), grade(grade), power(power),
      shield(shield), skill_description(skill_description), type_role(type_role),
      critical(critical)
{ // <<< กำหนดค่า critical
}

// Destructor Implementation
Card::~Card()
{
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
int Card::getCritical() const
{ // <<< Implement getter สำหรับ critical
  return critical;
}

// Member function สำหรับแสดงข้อมูลการ์ด Implementation
void Card::displayInfo() const
{
  std::cout << "------------------------------------" << std::endl;
  std::cout << "รหัสการ์ด: " << code_name << std::endl;
  std::cout << "ชื่อการ์ด: " << name << " (G" << grade << ")" << std::endl;
  std::cout << "ประเภท/บทบาท: " << type_role << std::endl;
  std::cout << "พลัง: " << power << " / โล่ห์: " << shield << " / คริติคอล: " << critical << std::endl; // <<< เพิ่ม critical
  std::cout << "สกิล: " << skill_description << std::endl;
  std::cout << "------------------------------------" << std::endl;
}

// Overload a << operator Implementation
std::ostream &operator<<(std::ostream &os, const Card &card)
{
  os << "[" << card.code_name << "] " << card.name << " (G" << card.grade << " C" << card.critical << ")"; // <<< เพิ่ม critical
  return os;
}
