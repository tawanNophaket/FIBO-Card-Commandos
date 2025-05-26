// Card.h - ไฟล์ Header สำหรับคลาส Card
#ifndef CARD_H
#define CARD_H

#include <string>
#include <iostream>

class Card
{
private:
  std::string code_name;
  std::string name;
  int grade;
  int power;
  int shield;
  std::string skill_description;
  std::string type_role;
  int critical;

public:
  // Constructor
  Card(const std::string &code_name,
       const std::string &name,
       int grade,
       int power,
       int shield,
       const std::string &skill_description,
       const std::string &type_role,
       int critical);

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
  int getCritical() const;

  void displayInfo() const; // อาจจะไม่จำเป็นถ้ามี operator<< ที่ดีแล้ว

  friend std::ostream &operator<<(std::ostream &os, const Card &card);
};

#endif // CARD_H
