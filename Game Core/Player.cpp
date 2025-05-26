// Player_Improved.cpp - ไฟล์จัดการระบบผู้เล่นที่ปรับปรุงใหม่
// ไฟล์นี้รับผิดชอบการจัดการทุกการกระทำของผู้เล่น เช่น การจั่วไพ่ การวางการ์ด การโจมตี
// รวมถึงการแสดงผลผ่านระบบ UI ที่ปรับปรุงใหม่เพื่อให้ใช้งานง่ายและสวยงามขึ้น
#include "Player.h"
#include <optional>
#include "Card.h"
#include "../UI System/UIHelper.h"
#include "../UI System/MenuSystem.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <limits>

// ฟังก์ชันช่วยสำหรับแสดงเส้นคั่นบนหน้าจอ
void Player::printDisplayLine(char c, int length)
{
  std::cout << std::string(length, c) << std::endl;
}

// Constructor - สร้างผู้เล่นใหม่พร้อมสำรับไพ่
Player::Player(const std::string &player_name, Deck &&player_deck)
    : name(player_name), deck(std::move(player_deck)), turn_count(0)
{
  unit_is_standing.fill(true); // เริ่มต้นให้ทุกยูนิตอยู่ในสถานะยืน (Standing)
}

// ฟังก์ชันช่วยสำหรับจั่วการ์ดจำนวนที่กำหนด
void Player::drawCards(int num_to_draw)
{
  for (int i = 0; i < num_to_draw; ++i)
  {
    std::optional<Card> drawn_card = deck.draw();
    if (drawn_card.has_value())
    {
      hand.push_back(drawn_card.value());
    }
    else
    {
      break; // หยุดจั่วถ้าไม่มีการ์ดเหลือในสำรับ
    }
  }
}

// เตรียมเกมเริ่มต้น โดยวาง starter และจั่วการ์ดเริ่มต้น
bool Player::setupGame(const std::string &starter_code_name, int initial_hand_size)
{
  // ค้นหาและดึง starter การ์ดจากสำรับ
  std::optional<Card> starter_card_opt = deck.removeCardByCodeName(starter_code_name);
  if (!starter_card_opt.has_value())
  {
    return false; // ไม่พบ starter การ์ด
  }

  // วาง starter ลงใน vanguard circle
  vanguard_circle = starter_card_opt.value();
  unit_is_standing[UNIT_STATUS_VC_IDX] = true;

  // สับสำรับและจั่วการ์ดเริ่มต้น
  deck.shuffle();
  drawCards(initial_hand_size);
  return true;
}

// ทำให้ยูนิตทั้งหมดกลับมายืน (Stand Phase)
void Player::performStandPhase()
{
  unit_is_standing.fill(true);
}

// จั่วการ์ด 1 ใบในเฟสจั่ว (Draw Phase)
bool Player::performDrawPhase()
{
  if (deck.isEmpty())
  {
    return false;
  }
  drawCards(1);
  return true;
}

// ไรด์การ์ดจากมือลงบน Vanguard Circle
bool Player::rideFromHand(size_t hand_card_index)
{
  // ตรวจสอบความถูกต้องของ index
  if (hand_card_index >= hand.size())
  {
    return false;
  }

  Card card_to_ride = hand[hand_card_index];
  int current_vg_grade = vanguard_circle.has_value() ? vanguard_circle.value().getGrade() : -1;

  // ตรวจสอบเงื่อนไขการไรด์
  bool can_ride = false;
  if (!vanguard_circle.has_value())
  {
    // ถ้ายังไม่มี vanguard ต้องเป็นการ์ดเกรด 0
    if (card_to_ride.getGrade() == 0)
      can_ride = true;
  }
  else
  {
    // ถ้ามี vanguard แล้ว ต้องเป็นเกรดเท่ากันหรือมากกว่า 1 เกรด
    if (card_to_ride.getGrade() == current_vg_grade || card_to_ride.getGrade() == current_vg_grade + 1)
    {
      can_ride = true;
    }
  }

  if (!can_ride)
  {
    return false;
  }

  // ย้าย vanguard เดิมลง soul (ถ้ามี)
  if (vanguard_circle.has_value())
  {
    soul.push_back(vanguard_circle.value());
  }

  // วางการ์ดใหม่เป็น vanguard
  vanguard_circle = card_to_ride;
  unit_is_standing[UNIT_STATUS_VC_IDX] = true;
  hand.erase(hand.begin() + hand_card_index);
  return true;
}

// เรียกการ์ดจากมือลงบน Rear-guard Circle
bool Player::callToRearGuard(size_t hand_card_index, size_t rc_slot_index)
{
  // ตรวจสอบความถูกต้องของ indexes และเงื่อนไขต่างๆ
  if (hand_card_index >= hand.size())
  {
    return false;
  }
  if (rc_slot_index >= NUM_REAR_GUARD_CIRCLES)
  {
    return false;
  }
  if (rear_guard_circles[rc_slot_index].has_value())
  {
    return false;
  }
  Card card_to_call = hand[hand_card_index];
  if (!vanguard_circle.has_value())
  {
    return false;
  }
  if (card_to_call.getGrade() > vanguard_circle.value().getGrade())
  {
    return false;
  }
  rear_guard_circles[rc_slot_index] = card_to_call;
  unit_is_standing[getUnitStatusIndexForRC(rc_slot_index)] = true;
  hand.erase(hand.begin() + hand_card_index);
  return true;
}

// เลือกยูนิตที่จะใช้โจมตี
std::vector<std::pair<int, std::string>> Player::chooseAttacker()
{
  std::vector<std::pair<int, std::string>> available_attackers;

  // ตรวจสอบ Vanguard
  if (vanguard_circle.has_value() && unit_is_standing[UNIT_STATUS_VC_IDX])
  {
    // เพิ่ม Vanguard เข้าลิสต์ผู้โจมตีที่เป็นไปได้
    std::string vg_info = Icons::CROWN + " VC: " + vanguard_circle.value().getName() +
                          " (G" + std::to_string(vanguard_circle.value().getGrade()) +
                          " P:" + std::to_string(vanguard_circle.value().getPower()) + ")";
    available_attackers.push_back({UNIT_STATUS_VC_IDX, vg_info});
  }

  // ตรวจสอบ Rear-guards แถวหน้า
  const size_t front_row_rcs[] = {RC_FRONT_LEFT, RC_FRONT_RIGHT};
  for (size_t rc_idx : front_row_rcs)
  {
    // เพิ่ม Rear-guards ที่ยังยืนอยู่เข้าลิสต์
    if (rear_guard_circles[rc_idx].has_value() && unit_is_standing[getUnitStatusIndexForRC(rc_idx)])
    {
      const Card &rc_card = rear_guard_circles[rc_idx].value();
      std::string rc_info = Icons::SWORD + " RC" + (rc_idx == RC_FRONT_LEFT ? "L" : "R") +
                            ": " + rc_card.getName() + " (G" + std::to_string(rc_card.getGrade()) +
                            " P:" + std::to_string(rc_card.getPower()) + ")";
      available_attackers.push_back({(int)getUnitStatusIndexForRC(rc_idx), rc_info});
    }
  }

  // แสดงผลยูนิตที่สามารถโจมตีได้
  if (available_attackers.empty())
  {
    UIHelper::PrintWarning("ไม่มียูนิตที่สามารถโจมตีได้ในขณะนี้ (หรือทั้งหมด Rest อยู่)");
  }
  else
  {
    std::cout << Colors::BRIGHT_CYAN << Icons::SWORD << " ยูนิตที่สามารถโจมตีได้:" << Colors::RESET << std::endl;
    for (size_t i = 0; i < available_attackers.size(); ++i)
    {
      std::cout << Colors::YELLOW << "[" << i << "] " << Colors::RESET
                << available_attackers[i].second << std::endl;
    }
  }
  return available_attackers;
}

// เลือก Booster สำหรับยูนิตที่โจมตี
int Player::chooseBooster(int attacker_unit_status_idx)
{
  // ตรวจสอบความถูกต้องของตำแหน่งผู้โจมตี
  if (attacker_unit_status_idx == -1)
    return -1;

  // ดึงข้อมูลการ์ดผู้โจมตี
  std::optional<Card> attacker_card = getUnitAtStatusIndex(attacker_unit_status_idx);
  if (!attacker_card.has_value())
    return -1;

  // หาตำแหน่งที่เป็นไปได้สำหรับ booster
  int potential_booster_status_idx = -1;
  size_t booster_rc_idx = static_cast<size_t>(-1);

  // กำหนดตำแหน่ง booster ตามตำแหน่งผู้โจมตี
  if (attacker_unit_status_idx == UNIT_STATUS_VC_IDX)
  {
    booster_rc_idx = RC_BACK_CENTER; // ถ้าผู้โจมตีเป็น Vanguard, booster จะอยู่ตำแหน่งกลางแถวหลัง
  }
  else if (attacker_unit_status_idx == UNIT_STATUS_RC_FL_IDX)
  {
    booster_rc_idx = RC_BACK_LEFT; // ถ้าผู้โจมตีอยู่ซ้ายแถวหน้า, booster จะอยู่ซ้ายแถวหลัง
  }
  else if (attacker_unit_status_idx == UNIT_STATUS_RC_FR_IDX)
  {
    booster_rc_idx = RC_BACK_RIGHT; // ถ้าผู้โจมตีอยู่ขวาแถวหน้า, booster จะอยู่ขวาแถวหลัง
  }

  // ตรวจสอบว่ามี booster ที่ใช้ได้หรือไม่
  if (booster_rc_idx != static_cast<size_t>(-1))
  {
    potential_booster_status_idx = getUnitStatusIndexForRC(booster_rc_idx);

    // ตรวจสอบเงื่อนไขการ boost:
    // 1. มีการ์ดในตำแหน่ง booster
    // 2. การ์ดนั้นต้องยังยืนอยู่
    // 3. การ์ดนั้นต้องเป็นเกรด 0 หรือ 1
    if (rear_guard_circles[booster_rc_idx].has_value() &&
        unit_is_standing[potential_booster_status_idx] &&
        (rear_guard_circles[booster_rc_idx].value().getGrade() <= 1))
    {
      // แสดงข้อมูล booster ที่สามารถใช้ได้
      const Card &booster_card = rear_guard_circles[booster_rc_idx].value();
      // ...existing UI code...
      return potential_booster_status_idx;
    }
  }
  return -1; // ไม่มี booster ที่ใช้ได้
}

// ทำให้ยูนิตในตำแหน่งที่ระบุหมุนตัว (Rest)
void Player::restUnit(int unit_status_idx)
{
  if (unit_status_idx >= 0 && static_cast<size_t>(unit_status_idx) < NUM_FIELD_UNITS)
  {
    unit_is_standing[static_cast<size_t>(unit_status_idx)] = false;
  }
}

// ตรวจสอบว่ายูนิตในตำแหน่งที่ระบุยังยืนอยู่หรือไม่
bool Player::isUnitStanding(int unit_status_idx) const
{
  if (unit_status_idx >= 0 && static_cast<size_t>(unit_status_idx) < NUM_FIELD_UNITS)
  {
    return unit_is_standing[static_cast<size_t>(unit_status_idx)];
  }
  return false;
}

// ดึงข้อมูลการ์ดจากตำแหน่งที่ระบุ
std::optional<Card> Player::getUnitAtStatusIndex(int unit_status_idx) const
{
  // ถ้าเป็นตำแหน่ง Vanguard
  if (unit_status_idx == UNIT_STATUS_VC_IDX)
  {
    return vanguard_circle;
  }
  // ถ้าเป็นตำแหน่ง Rear-guard
  else if (unit_status_idx > 0 && static_cast<size_t>(unit_status_idx - 1) < NUM_REAR_GUARD_CIRCLES)
  {
    return rear_guard_circles[static_cast<size_t>(unit_status_idx - 1)];
  }
  return std::nullopt; // ถ้าตำแหน่งไม่ถูกต้อง
}

// คำนวณพลังโจมตีรวมของยูนิต รวมถึง booster (ถ้ามี)
int Player::getUnitPowerAtStatusIndex(int unit_status_idx, int booster_unit_status_idx, bool for_defense) const
{
  int total_power = 0;
  std::optional<Card> unit_opt = getUnitAtStatusIndex(unit_status_idx);
  if (unit_opt.has_value())
  {
    total_power += unit_opt.value().getPower();
  }
  if (!for_defense && booster_unit_status_idx != -1)
  {
    std::optional<Card> booster_opt = getUnitAtStatusIndex(booster_unit_status_idx);
    if (booster_opt.has_value())
    {
      total_power += booster_opt.value().getPower();
    }
  }
  return total_power;
}

// ฟังก์ชันช่วยสำหรับการประมวลผล Trigger
// - self: ผู้เล่นที่เปิด trigger
// - trigger_card: การ์ดที่เปิดได้
// - is_drive_check: เป็นการเช็คจาก drive check หรือไม่
// - opponent_for_heal_check: ผู้เล่นฝ่ายตรงข้าม (ใช้สำหรับเช็คเงื่อนไขการฮีล)
TriggerOutput apply_trigger_logic_helper(Player *self, const Card &trigger_card, bool is_drive_check, Player *opponent_for_heal_check)
{
  TriggerOutput output;
  std::cout << Colors::BRIGHT_MAGENTA << Icons::MAGIC << " เปิดได้ Trigger: " << Colors::BOLD
            << trigger_card.getName() << Colors::RESET << Colors::BRIGHT_BLACK
            << " (" << trigger_card.getTypeRole() << ")" << Colors::RESET << "!" << std::endl;

  if (trigger_card.getTypeRole() == "Trigger - Critical")
  {
    output.extra_power += 10000; // เพิ่มพลังโจมตี 10000
    if (is_drive_check)
    {
      output.extra_crit += 1; // เพิ่มคริติคอล 1 (เฉพาะ Drive Check)
      std::cout << Colors::BRIGHT_RED << Icons::CRITICAL << " ผล: +10000 Power และ +1 Critical!" << Colors::RESET << std::endl;
    }
    else
    {
      std::cout << Colors::BRIGHT_RED << Icons::CRITICAL << " ผล: +10000 Power!" << Colors::RESET << std::endl;
    }
  }
  else if (trigger_card.getTypeRole() == "Trigger - Draw")
  {
    output.extra_power += 10000; // เพิ่มพลังโจมตี 10000
    // จั่วการ์ดเพิ่ม 1 ใบ
    std::optional<Card> drawn = self->getDeck().draw();
    if (drawn.has_value())
    {
      self->addCardToHand(drawn.value());
      std::cout << Colors::BRIGHT_CYAN << Icons::DRAW << " ผล: +10000 Power และ จั่ว 1 ใบ!" << Colors::RESET << std::endl;
      output.card_drawn = true;
    }
  }
  else if (trigger_card.getTypeRole() == "Trigger - Heal")
  {
    output.extra_power += 10000; // เพิ่มพลังโจมตี 10000
    std::cout << Colors::BRIGHT_GREEN << Icons::HEAL << " ผล: +10000 Power" << Colors::RESET;
    // ตรวจสอบเงื่อนไขการฮีล:
    // 1. ต้องมีดาเมจมากกว่าหรือเท่ากับฝ่ายตรงข้าม (ถ้ามีฝ่ายตรงข้าม)
    // 2. ต้องมีดาเมจอย่างน้อย 1
    if (opponent_for_heal_check && self->getDamageCount() > 0 && self->getDamageCount() >= opponent_for_heal_check->getDamageCount())
    {
      if (self->healOneDamage())
      {
        output.damage_healed = true;
        std::cout << Colors::BRIGHT_GREEN << " และ Heal 1 ดาเมจ!" << Colors::RESET << std::endl;
      }
    }
    else if (!opponent_for_heal_check && self->getDamageCount() > 0)
    {
      if (self->healOneDamage())
      {
        output.damage_healed = true;
        std::cout << Colors::BRIGHT_GREEN << " และ Heal 1 ดาเมจ!" << Colors::RESET << std::endl;
      }
    }
    else
    {
      std::cout << Colors::BRIGHT_BLACK << " (ไม่สามารถ Heal ได้)" << Colors::RESET << std::endl;
    }
  }
  return output;
}

// ทำการ Drive Check ตามจำนวนที่กำหนด
TriggerOutput Player::performDriveCheck(int num_drives, Player *opponent_for_heal_check)
{
  // แสดงหัวข้อการ Drive Check
  UIHelper::PrintSectionHeader(name + ": DRIVE CHECK x" + std::to_string(num_drives), Icons::DIAMOND);

  TriggerOutput total_trigger_output; // เก็บผลรวมของ trigger ทั้งหมด

  // ทำการ Drive Check ตามจำนวนที่กำหนด
  for (int i = 0; i < num_drives; ++i)
  {
    // ตรวจสอบว่ายังมีการ์ดเหลือในสำรับหรือไม่
    if (deck.isEmpty())
    {
      UIHelper::PrintError("เด็คหมด! ไม่สามารถ Drive Check ได้");
      break;
    }

    // แสดงแอนิเมชันการ Drive Check
    UIHelper::ShowDriveCheckAnimation();

    // จั่วการ์ดและแสดงผล
    std::optional<Card> drive_card_opt = deck.draw();
    if (drive_card_opt.has_value())
    {
      Card drive_card = drive_card_opt.value();
      std::cout << Colors::BRIGHT_CYAN << "Drive Check ครั้งที่ " << (i + 1) << ": "
                << Colors::RESET << UIHelper::FormatCard(drive_card.getName(), drive_card.getGrade()) << std::endl;
      hand.push_back(drive_card); // เพิ่มการ์ดเข้ามือ

      // ตรวจสอบว่าเป็นการ์ด Trigger หรือไม่
      if (drive_card.getTypeRole().find("Trigger") != std::string::npos)
      {
        TriggerOutput current_drive_trigger_output = apply_trigger_logic_helper(this, drive_card, true, opponent_for_heal_check);
        total_trigger_output.extra_power += current_drive_trigger_output.extra_power;
        total_trigger_output.extra_crit += current_drive_trigger_output.extra_crit;
      }
    }
  }
  return total_trigger_output;
}

TriggerOutput Player::handleDamageCheckTrigger(const Card &damage_card, Player *opponent_for_heal_check)
{
  std::cout << Colors::BRIGHT_RED << name << " เปิดได้จากการ Damage Check: " << Colors::RESET
            << UIHelper::FormatCard(damage_card.getName(), damage_card.getGrade()) << std::endl;
  if (damage_card.getTypeRole().find("Trigger") != std::string::npos)
  {
    return apply_trigger_logic_helper(this, damage_card, false, opponent_for_heal_check);
  }
  return TriggerOutput();
}

bool Player::healOneDamage()
{
  if (!damage_zone.empty())
  {
    Card healed_card = damage_zone.back(); // Heal การ์ดใบล่าสุด
    damage_zone.pop_back();
    drop_zone.push_back(healed_card);
    std::cout << Colors::BRIGHT_GREEN << name << " Heal 1 ดาเมจ! " << Colors::RESET
              << Colors::BRIGHT_BLACK << "(การ์ด '" << healed_card.getName()
              << "' ไปยัง Drop Zone)" << Colors::RESET << std::endl;
    return true;
  }
  return false;
}

int Player::chooseUnitForTriggerEffect(const std::string &trigger_effect_description)
{
  std::cout << Colors::BRIGHT_YELLOW << name << ": " << trigger_effect_description << Colors::RESET << std::endl;
  std::cout << "เลือกยูนิตที่จะรับผล:" << std::endl;
  std::vector<std::pair<int, std::string>> available_units;
  if (vanguard_circle.has_value())
  {
    available_units.push_back({UNIT_STATUS_VC_IDX, Icons::CROWN + " VC: " + vanguard_circle.value().getName()});
  }
  for (size_t i = 0; i < NUM_REAR_GUARD_CIRCLES; ++i)
  {
    if (rear_guard_circles[i].has_value())
    {
      std::string rc_name = "RC" + std::to_string(i) + ": " + rear_guard_circles[i].value().getName();
      available_units.push_back({(int)getUnitStatusIndexForRC(i), Icons::SWORD + " " + rc_name});
    }
  }
  if (available_units.empty())
  {
    UIHelper::PrintWarning("ไม่มียูนิตในสนามให้เลือก");
    return -1;
  }
  for (size_t i = 0; i < available_units.size(); ++i)
  {
    std::cout << Colors::YELLOW << "[" << i << "] " << Colors::RESET
              << available_units[i].second << std::endl;
  }
  int choice_idx = -1;
  std::cout << Colors::BRIGHT_CYAN << "เลือกหมายเลขยูนิต (-1 หากไม่ต้องการให้ผลกับยูนิตใด): " << Colors::RESET;
  std::cin >> choice_idx;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  if (choice_idx >= 0 && static_cast<size_t>(choice_idx) < available_units.size())
  {
    return available_units[static_cast<size_t>(choice_idx)].first;
  }
  return -1;
}

// --- Guarding Methods ---
int Player::addCardToGuardianZoneFromHand(size_t hand_card_index)
{
  if (hand_card_index >= hand.size())
  {
    UIHelper::PrintError("ไม่สามารถ Guard ด้วยการ์ดตำแหน่ง " + std::to_string(hand_card_index) + " (ไม่มีการ์ด)");
    return -1;
  }
  Card card_to_guard = hand[hand_card_index];
  guardian_zone.push_back(card_to_guard);
  int shield_value = card_to_guard.getShield();
  hand.erase(hand.begin() + static_cast<long>(hand_card_index));

  std::cout << Colors::BRIGHT_BLUE << Icons::SHIELD << " " << name << " ใช้ '"
            << card_to_guard.getName() << "' (Shield: " << shield_value
            << ") ในการ Guard." << Colors::RESET << std::endl;
  return shield_value;
}

int Player::getGuardianZoneShieldTotal() const
{
  int total_shield = 0;
  for (const auto &card : guardian_zone)
  {
    total_shield += card.getShield();
  }
  return total_shield;
}

int Player::performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt)
{
  UIHelper::PrintSectionHeader(name + ": GUARD PHASE", Icons::SHIELD, Colors::BRIGHT_BLUE);

  std::cout << Colors::BRIGHT_RED << Icons::SWORD << " พลังโจมตีที่เข้ามา: "
            << incoming_attack_power << Colors::RESET << std::endl;
  if (target_unit_opt.has_value())
  {
    std::cout << Colors::BRIGHT_YELLOW << Icons::TARGET << " เป้าหมายคือ: "
              << target_unit_opt.value().getName()
              << " (Power ปัจจุบัน: " << target_unit_opt.value().getPower() << ")"
              << Colors::RESET << std::endl;
  }

  char continue_guard_choice = 'y';

  while ((continue_guard_choice == 'y' || continue_guard_choice == 'Y'))
  {
    if (hand.empty())
    {
      UIHelper::PrintWarning("ไม่เหลือการ์ดบนมือให้ Guard แล้ว!");
      break;
    }

    std::cout << "\n"
              << Colors::BRIGHT_CYAN << Icons::HAND << " การ์ดบนมือของคุณ ("
              << name << "):" << Colors::RESET << std::endl;
    displayHand(true);

    std::cout << Colors::BRIGHT_BLUE << Icons::GUARD << " Guardian Zone ปัจจุบัน: " << Colors::RESET;
    displayGuardianZone();
    std::cout << std::endl;

    std::cout << Colors::BRIGHT_BLUE << Icons::SHIELD << " Shield รวมปัจจุบัน: "
              << getGuardianZoneShieldTotal() << Colors::RESET << std::endl;

    std::cout << Colors::BRIGHT_CYAN << "เลือกการ์ดจากมือเพื่อ Guard (-1 เพื่อหยุด Guard): " << Colors::RESET;
    std::string s_idx;
    std::cin >> s_idx;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    int card_idx = -1;
    try
    {
      card_idx = std::stoi(s_idx);
    }
    catch (...)
    {
      card_idx = -2; /* invalid input */
    }

    if (card_idx == -1)
      break;

    if (card_idx >= 0 && static_cast<size_t>(card_idx) < hand.size())
    {
      addCardToGuardianZoneFromHand(static_cast<size_t>(card_idx));
    }
    else
    {
      UIHelper::PrintError("เลือกไม่ถูกต้อง");
    }

    if (hand.empty())
    {
      UIHelper::PrintWarning("ไม่เหลือการ์ดบนมือให้ Guard แล้ว!");
      break;
    }

    std::cout << Colors::BRIGHT_CYAN << "ต้องการ Guard เพิ่มหรือไม่ (y/n): " << Colors::RESET;
    std::cin >> continue_guard_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  int final_shield = getGuardianZoneShieldTotal();
  std::cout << Colors::BRIGHT_GREEN << Icons::CONFIRM << " จบขั้นตอนการ Guard. Shield ที่ได้ทั้งหมด: "
            << final_shield << Colors::RESET << std::endl;
  return final_shield;
}

void Player::displayGuardianZone() const
{
  if (guardian_zone.empty())
  {
    std::cout << Colors::BRIGHT_BLACK << "(ว่าง)" << Colors::RESET;
  }
  else
  {
    for (const auto &card : guardian_zone)
    {
      std::cout << Colors::BLUE << "[" << card.getName() << " S:" << card.getShield() << "] " << Colors::RESET;
    }
  }
}

// Enhanced field display
std::string formatCardForDisplayImproved(const std::optional<Card> &card_opt, int width, bool is_standing)
{
  std::ostringstream oss;
  std::string content;
  if (card_opt.has_value())
  {
    const Card &card = card_opt.value();
    std::string grade_icon = UIHelper::GetGradeIcon(card.getGrade());
    std::string status_icon = is_standing ? Icons::RUNNER : Icons::SLEEP;
    std::string name_str = card.getName();
    std::string power_str = "P:" + std::to_string(card.getPower());

    // Safe truncation for Unicode/Thai/emoji
    std::string name_trunc;
    int max_display_width = 12; // Maximum display width we want
    int display_width = 0;      // Current display width
    size_t byte_pos = 0;        // Current byte position in name_str

    while (byte_pos < name_str.length())
    {
      // Get next character width
      unsigned char c = name_str[byte_pos];
      int char_width;
      size_t char_bytes;

      if ((c & 0x80) == 0)
      {
        // ASCII
        char_width = 1;
        char_bytes = 1;
      }
      else if ((c & 0xE0) == 0xC0)
      {
        // 2-byte UTF-8
        char_width = 2;
        char_bytes = 2;
      }
      else if ((c & 0xF0) == 0xE0)
      {
        // 3-byte UTF-8 (Thai, etc)
        char_width = 2;
        char_bytes = 3;
      }
      else if ((c & 0xF8) == 0xF0)
      {
        // 4-byte UTF-8 (emoji)
        char_width = 2;
        char_bytes = 4;
      }
      else
      {
        // Invalid UTF-8, skip
        byte_pos++;
        continue;
      }

      // Check if adding this character would exceed max width
      if (display_width + char_width > max_display_width)
        break;

      // Safe to append this character
      if (byte_pos + char_bytes <= name_str.length())
      {
        name_trunc.append(name_str.substr(byte_pos, char_bytes));
        display_width += char_width;
      }

      byte_pos += char_bytes;
    }

    // Add truncation indicator if necessary
    if (byte_pos < name_str.length() && !name_trunc.empty())
    {
      name_trunc += "..";
    }
    else if (name_trunc.empty())
    {
      name_trunc = "??";
    }

    content = grade_icon + " " + name_trunc + " " + power_str + " " + status_icon;
  }
  else
  {
    content = Colors::BRIGHT_BLACK + "[     ว่าง     ]" + Colors::RESET;
  }

  // Calculate actual display width including color codes
  std::string stripped_content = content;
  size_t pos = 0;
  while ((pos = stripped_content.find("\033[", pos)) != std::string::npos)
  {
    size_t end_pos = stripped_content.find('m', pos);
    if (end_pos != std::string::npos)
    {
      stripped_content.erase(pos, end_pos - pos + 1);
    }
  }

  int text_len = UIHelper::GetDisplayWidth(stripped_content);
  int padding = width - text_len;
  if (padding < 0)
    padding = 0;
  int pad_left = padding / 2;
  int pad_right = padding - pad_left;

  oss << std::string(pad_left, ' ') << content << std::string(pad_right, ' ');
  return oss.str();
}

void Player::displayField(bool show_opponent_field_for_targeting) const
{
  const int card_cell_width = 15; // Width for each card cell
  const std::string V_BORDER = Colors::BRIGHT_BLACK + "│" + Colors::RESET;
  const std::string H_BORDER_THICK_SEGMENT = Colors::BRIGHT_BLACK + std::string(card_cell_width, '=') + Colors::RESET;
  const std::string H_BORDER_THIN_SEGMENT = Colors::BRIGHT_BLACK + std::string(card_cell_width, '-') + Colors::RESET;

  // Player Name and Deck/Soul/Drop
  std::cout << Colors::BOLD << Colors::YELLOW << name << Colors::RESET << " - Turn "
            << Colors::BOLD << turn_count << Colors::RESET << std::endl;

  std::cout << "  " << Colors::CYAN << "┌" << H_BORDER_THICK_SEGMENT << "┬" << H_BORDER_THIN_SEGMENT
            << "┬" << H_BORDER_THIN_SEGMENT << "┐" << Colors::RESET << std::endl;

  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (Colors::GREEN + Icons::DECK + " Deck: " + Colors::BOLD + std::to_string(deck.getSize()) + Colors::RESET)
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (Colors::MAGENTA + Icons::SOUL + " Soul: " + Colors::BOLD + std::to_string(soul.size()) + Colors::RESET)
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (Colors::BRIGHT_BLACK + Icons::DROP + " Drop: " + Colors::BOLD + std::to_string(drop_zone.size()) + Colors::RESET)
            << V_BORDER << std::endl;

  // Field Separator (Top)
  std::cout << "  " << Colors::CYAN << "├" << H_BORDER_THIN_SEGMENT << "┼" << H_BORDER_THICK_SEGMENT
            << "┼" << H_BORDER_THIN_SEGMENT << "┤" << Colors::RESET << std::endl;

  // Front Row Cards
  std::cout << "  " << V_BORDER << formatCardForDisplayImproved(rear_guard_circles[RC_FRONT_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_FRONT_LEFT)])
            << V_BORDER << formatCardForDisplayImproved(vanguard_circle, card_cell_width, unit_is_standing[UNIT_STATUS_VC_IDX])
            << V_BORDER << formatCardForDisplayImproved(rear_guard_circles[RC_FRONT_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_FRONT_RIGHT)])
            << V_BORDER << " " << Colors::RED << Icons::DAMAGE << " Damage: " << Colors::BOLD << damage_zone.size() << "/" << MAX_DAMAGE << Colors::RESET << std::endl;

  // Front Row Labels
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (show_opponent_field_for_targeting ? Colors::YELLOW + "   (0: RC FL)" + Colors::RESET : Colors::BRIGHT_BLACK + "   (RC FL)" + Colors::RESET)
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (Colors::BRIGHT_YELLOW + "   (VC)" + Colors::RESET)
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (show_opponent_field_for_targeting ? Colors::YELLOW + "   (1: RC FR)" + Colors::RESET : Colors::BRIGHT_BLACK + "   (RC FR)" + Colors::RESET)
            << V_BORDER << std::endl;

  // Field Separator (Middle)
  std::cout << "  " << Colors::CYAN << "├" << H_BORDER_THIN_SEGMENT << "┼" << H_BORDER_THIN_SEGMENT
            << "┼" << H_BORDER_THIN_SEGMENT << "┤" << Colors::RESET << std::endl;

  // Back Row Cards
  std::cout << "  " << V_BORDER << formatCardForDisplayImproved(rear_guard_circles[RC_BACK_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_LEFT)])
            << V_BORDER << formatCardForDisplayImproved(rear_guard_circles[RC_BACK_CENTER], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_CENTER)])
            << V_BORDER << formatCardForDisplayImproved(rear_guard_circles[RC_BACK_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_RIGHT)])
            << V_BORDER << std::endl;

  // Back Row Labels
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (show_opponent_field_for_targeting ? Colors::YELLOW + "   (2: RC BL)" + Colors::RESET : Colors::BRIGHT_BLACK + "   (RC BL)" + Colors::RESET)
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (show_opponent_field_for_targeting ? Colors::YELLOW + "   (3: RC BC)" + Colors::RESET : Colors::BRIGHT_BLACK + "   (RC BC)" + Colors::RESET)
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ')
            << (show_opponent_field_for_targeting ? Colors::YELLOW + "   (4: RC BR)" + Colors::RESET : Colors::BRIGHT_BLACK + "   (RC BR)" + Colors::RESET)
            << V_BORDER << std::endl;

  // Field Separator (Bottom)
  std::cout << "  " << Colors::CYAN << "└" << H_BORDER_THIN_SEGMENT << "┴" << H_BORDER_THIN_SEGMENT
            << "┴" << H_BORDER_THIN_SEGMENT << "┘" << Colors::RESET << std::endl;

  // Damage Zone
  std::cout << "\n"
            << Colors::RED << Icons::DAMAGE << " Damage Zone (" << damage_zone.size() << "): " << Colors::RESET;
  if (damage_zone.empty())
  {
    std::cout << Colors::BRIGHT_BLACK << "(ยังไม่ได้รับดาเมจ)" << Colors::RESET;
  }
  else
  {
    for (size_t i = 0; i < damage_zone.size(); ++i)
    {
      std::string name_short = damage_zone[i].getName().substr(0, 6);
      if (damage_zone[i].getName().length() > 6)
      {
        name_short += "..";
      }
      std::cout << Colors::RED << "[" << UIHelper::GetGradeIcon(damage_zone[i].getGrade())
                << " " << name_short << "]" << Colors::RESET;
      if (i < damage_zone.size() - 1)
        std::cout << " ";
    }
  }
  std::cout << std::endl;
  UIHelper::PrintHorizontalLine('=', 70, Colors::CYAN);
}

void Player::displayHand(bool show_details) const
{
  UIHelper::PrintHorizontalLine('-', 40, Colors::YELLOW);
  std::cout << Colors::YELLOW << Icons::HAND << " มือของ " << Colors::BOLD << name
            << Colors::RESET << Colors::YELLOW << " (" << hand.size() << " ใบ)" << Colors::RESET << std::endl;

  if (hand.empty())
  {
    std::cout << Colors::BRIGHT_BLACK << "(มือว่าง)" << Colors::RESET << std::endl;
  }
  else
  {
    for (size_t i = 0; i < hand.size(); ++i)
    {
      std::cout << Colors::CYAN << "[" << i << "] " << Colors::RESET
                << UIHelper::FormatCard(hand[i].getName(), hand[i].getGrade());
      if (show_details)
      {
        std::cout << " " << UIHelper::FormatPowerShield(hand[i].getPower(), hand[i].getShield());
      }
      std::cout << std::endl;
    }
  }
  UIHelper::PrintHorizontalLine('-', 40, Colors::YELLOW);
}

// Getters
std::string Player::getName() const { return name; }
size_t Player::getHandSize() const { return hand.size(); }
const std::vector<Card> &Player::getHand() const { return hand; }
size_t Player::getDamageCount() const { return damage_zone.size(); }
const std::optional<Card> &Player::getVanguard() const { return vanguard_circle; }
const std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> &Player::getRearGuards() const { return rear_guard_circles; }
Deck &Player::getDeck() { return deck; }
const Deck &Player::getDeck() const { return deck; }

void Player::displayFullStatus() const
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("FULL STATUS: " + name, Icons::PLAYER);
  displayField();
  displayHand(true);
  MenuSystem::WaitForKeyPress();
}

void Player::takeDamage(const Card &damage_card)
{
  UIHelper::ShowDamageAnimation();
  std::cout << Colors::BRIGHT_RED << Icons::DAMAGE << " " << name << " ได้รับ 1 ดาเมจ! " << Colors::RESET
            << "การ์ดที่ตก Damage Zone: " << UIHelper::FormatCard(damage_card.getName(), damage_card.getGrade()) << std::endl;
  damage_zone.push_back(damage_card);
  if (getDamageCount() >= MAX_DAMAGE)
  {
    std::cout << Colors::BRIGHT_RED << Colors::BOLD << Icons::SKULL << " " << name
              << " ได้รับ " << MAX_DAMAGE << " ดาเมจแล้ว! " << name << " แพ้แล้ว! " << Icons::SKULL
              << Colors::RESET << std::endl;
    // Potentially add game over logic here or set a flag
  }
}

void Player::addCardToHand(const Card &card)
{
  hand.push_back(card);
}

void Player::clearGuardianZoneAndMoveToDrop()
{
  if (!guardian_zone.empty())
  {
    std::cout << Colors::BRIGHT_BLACK << Icons::DROP << " " << name << " ย้ายการ์ดจาก Guardian Zone ไป Drop Zone:" << Colors::RESET << std::endl;
    for (const auto &card : guardian_zone)
    {
      std::cout << "  - " << UIHelper::FormatCard(card.getName(), card.getGrade()) << std::endl;
      drop_zone.push_back(card);
    }
    guardian_zone.clear();
  }
}

void Player::discardFromHandToDrop(size_t hand_card_index)
{
  if (hand_card_index < hand.size())
  {
    Card discarded_card = hand[hand_card_index];
    std::cout << Colors::BRIGHT_BLACK << Icons::DROP << " " << name << " ทิ้งการ์ด '"
              << discarded_card.getName() << "' จากมือลง Drop Zone." << Colors::RESET << std::endl;
    hand.erase(hand.begin() + hand_card_index);
    drop_zone.push_back(discarded_card);
  }
}

void Player::placeCardIntoSoul(const Card &card)
{
  soul.push_back(card);
  std::cout << Colors::MAGENTA << Icons::SOUL << " " << name << " วางการ์ด '"
            << card.getName() << "' ลง Soul." << Colors::RESET << std::endl;
}