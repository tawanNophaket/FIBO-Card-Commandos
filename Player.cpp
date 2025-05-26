// Player.cpp - ไฟล์ Source สำหรับ υλολοποίηση คลาส Player
#include "Player.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <limits>

// Static helper function definition for Player class
void Player::printDisplayLine(char c, int length)
{
  std::cout << std::string(length, c) << std::endl;
}

// Constructor
Player::Player(const std::string &player_name, Deck &&player_deck)
    : name(player_name), deck(std::move(player_deck))
{
  unit_is_standing.fill(true);
}

// Helper method to draw cards
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
      break;
    }
  }
}

// Setup Game
bool Player::setupGame(const std::string &starter_code_name, int initial_hand_size)
{
  std::optional<Card> starter_card_opt = deck.removeCardByCodeName(starter_code_name);
  if (!starter_card_opt.has_value())
  {
    return false;
  }
  vanguard_circle = starter_card_opt.value();
  unit_is_standing[UNIT_STATUS_VC_IDX] = true;
  deck.shuffle();
  drawCards(initial_hand_size);
  return true;
}

void Player::performStandPhase()
{
  unit_is_standing.fill(true);
}

bool Player::performDrawPhase()
{
  if (deck.isEmpty())
  {
    return false;
  }
  drawCards(1);
  return true;
}

bool Player::rideFromHand(size_t hand_card_index)
{
  if (hand_card_index >= hand.size())
  {
    return false;
  }
  Card card_to_ride = hand[hand_card_index];
  int current_vg_grade = vanguard_circle.has_value() ? vanguard_circle.value().getGrade() : -1;
  bool can_ride = false;
  if (!vanguard_circle.has_value())
  {
    if (card_to_ride.getGrade() == 0)
      can_ride = true;
  }
  else
  {
    if (card_to_ride.getGrade() == current_vg_grade || card_to_ride.getGrade() == current_vg_grade + 1)
    {
      can_ride = true;
    }
  }
  if (!can_ride)
  {
    return false;
  }
  if (vanguard_circle.has_value())
  {
    soul.push_back(vanguard_circle.value());
  }
  vanguard_circle = card_to_ride;
  unit_is_standing[UNIT_STATUS_VC_IDX] = true;
  hand.erase(hand.begin() + hand_card_index);
  return true;
}

bool Player::callToRearGuard(size_t hand_card_index, size_t rc_slot_index)
{
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

std::vector<std::pair<int, std::string>> Player::chooseAttacker()
{
  std::vector<std::pair<int, std::string>> available_attackers;
  if (vanguard_circle.has_value() && unit_is_standing[UNIT_STATUS_VC_IDX])
  {
    available_attackers.push_back({UNIT_STATUS_VC_IDX, "VC: " + vanguard_circle.value().getName()});
  }
  const size_t front_row_rcs[] = {RC_FRONT_LEFT, RC_FRONT_RIGHT};
  for (size_t rc_idx : front_row_rcs)
  {
    if (rear_guard_circles[rc_idx].has_value() && unit_is_standing[getUnitStatusIndexForRC(rc_idx)])
    {
      available_attackers.push_back({(int)getUnitStatusIndexForRC(rc_idx),
                                     std::to_string(rc_idx) + ": RC " + rear_guard_circles[rc_idx].value().getName()});
    }
  }
  if (available_attackers.empty())
  {
    std::cout << "ไม่มียูนิตที่สามารถโจมตีได้ในขณะนี้ (หรือทั้งหมด Rest อยู่)" << std::endl;
  }
  else
  {
    std::cout << "ยูนิตที่สามารถโจมตีได้:" << std::endl;
    for (size_t i = 0; i < available_attackers.size(); ++i)
    {
      std::cout << i << ": " << available_attackers[i].second << std::endl;
    }
  }
  return available_attackers;
}

int Player::chooseBooster(int attacker_unit_status_idx)
{
  if (attacker_unit_status_idx == -1)
    return -1;
  std::optional<Card> attacker_card = getUnitAtStatusIndex(attacker_unit_status_idx);
  if (!attacker_card.has_value())
    return -1;
  int potential_booster_status_idx = -1;
  size_t booster_rc_idx = static_cast<size_t>(-1);
  if (attacker_unit_status_idx == UNIT_STATUS_VC_IDX)
  {
    booster_rc_idx = RC_BACK_CENTER;
  }
  else if (attacker_unit_status_idx == UNIT_STATUS_RC_FL_IDX)
  {
    booster_rc_idx = RC_BACK_LEFT;
  }
  else if (attacker_unit_status_idx == UNIT_STATUS_RC_FR_IDX)
  {
    booster_rc_idx = RC_BACK_RIGHT;
  }
  if (booster_rc_idx != static_cast<size_t>(-1))
  {
    potential_booster_status_idx = getUnitStatusIndexForRC(booster_rc_idx);
    if (rear_guard_circles[booster_rc_idx].has_value() &&
        unit_is_standing[potential_booster_status_idx] &&
        (rear_guard_circles[booster_rc_idx].value().getGrade() == 0 || rear_guard_circles[booster_rc_idx].value().getGrade() == 1))
    {
      std::cout << "คุณสามารถ Boost Attacker (" << attacker_card.value().getName() << ") ด้วย: "
                << rear_guard_circles[booster_rc_idx].value().getName()
                << " (G" << rear_guard_circles[booster_rc_idx].value().getGrade() << ") ได้" << std::endl;
      return potential_booster_status_idx;
    }
  }
  return -1;
}

void Player::restUnit(int unit_status_idx)
{
  if (unit_status_idx >= 0 && static_cast<size_t>(unit_status_idx) < NUM_FIELD_UNITS)
  {
    unit_is_standing[static_cast<size_t>(unit_status_idx)] = false;
  }
}

bool Player::isUnitStanding(int unit_status_idx) const
{
  if (unit_status_idx >= 0 && static_cast<size_t>(unit_status_idx) < NUM_FIELD_UNITS)
  {
    return unit_is_standing[static_cast<size_t>(unit_status_idx)];
  }
  return false;
}

std::optional<Card> Player::getUnitAtStatusIndex(int unit_status_idx) const
{
  if (unit_status_idx == UNIT_STATUS_VC_IDX)
  {
    return vanguard_circle;
  }
  else if (unit_status_idx > 0 && static_cast<size_t>(unit_status_idx - 1) < NUM_REAR_GUARD_CIRCLES)
  {
    return rear_guard_circles[static_cast<size_t>(unit_status_idx - 1)];
  }
  return std::nullopt;
}

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

// Helper function for apply_trigger_logic (can be a free function or static private if preferred)
TriggerOutput apply_trigger_logic_helper(Player *self, const Card &trigger_card, bool is_drive_check, Player *opponent_for_heal_check)
{
  TriggerOutput output;
  std::cout << "เปิดได้ Trigger: " << trigger_card.getName() << " (" << trigger_card.getTypeRole() << ")!" << std::endl;

  if (trigger_card.getTypeRole() == "Trigger - Critical")
  {
    output.extra_power += 10000; // ตามกติกา
    if (is_drive_check)
    {
      output.extra_crit += 1;
      std::cout << "ผล: +10000 Power และ +1 Critical ให้ยูนิตที่เลือก (ใน Battle นี้)!" << std::endl;
    }
    else
    {
      std::cout << "ผล: +10000 Power ให้ยูนิตที่เลือก (ใน Battle นี้)!" << std::endl;
    }
  }
  else if (trigger_card.getTypeRole() == "Trigger - Draw")
  {
    output.extra_power += 10000;
    self->getDeck().draw(); // แก้ไข: ให้ Player เป็นคนจั่วผ่าน getDeck() แล้ว draw()
                            // หรือมี Player::drawOneCard()
    // self->drawCards(1); // ถ้า drawCards เป็น public หรือมี helper ที่เหมาะสม
    std::cout << "✔️ " << self->getName() << " จั่ว 1 ใบจาก Draw Trigger!" << std::endl;
    output.card_drawn = true; // อาจจะไม่จำเป็นต้อง track ถ้าแค่จั่ว
    std::cout << "ผล: +10000 Power ให้ยูนิตที่เลือก (ใน Battle นี้) และ จั่ว 1 ใบ!" << std::endl;
  }
  else if (trigger_card.getTypeRole() == "Trigger - Heal")
  {
    output.extra_power += 10000;
    std::cout << "ผล: +10000 Power ให้ยูนิตที่เลือก (ใน Battle นี้)" << std::endl;
    if (opponent_for_heal_check && self->getDamageCount() > 0 && self->getDamageCount() >= opponent_for_heal_check->getDamageCount())
    {
      if (self->healOneDamage())
      {
        output.damage_healed = true;
        std::cout << "และ Heal 1 ดาเมจ!" << std::endl;
      }
    }
    else if (!opponent_for_heal_check && self->getDamageCount() > 0)
    {
      if (self->healOneDamage())
      {
        output.damage_healed = true;
        std::cout << "และ Heal 1 ดาเมจ!" << std::endl;
      }
    }
  }
  return output;
}

TriggerOutput Player::performDriveCheck(int num_drives, Player *opponent_for_heal_check)
{
  std::cout << "\n--- " << name << ": ทำการ Drive Check " << num_drives << " ครั้ง ---" << std::endl;
  TriggerOutput total_trigger_output;
  for (int i = 0; i < num_drives; ++i)
  {
    if (deck.isEmpty())
    {
      std::cout << "เด็คหมด! ไม่สามารถ Drive Check ได้" << std::endl;
      break;
    }
    std::optional<Card> drive_card_opt = deck.draw();
    if (drive_card_opt.has_value())
    {
      Card drive_card = drive_card_opt.value();
      std::cout << "Drive Check ครั้งที่ " << (i + 1) << ": " << drive_card << std::endl;
      hand.push_back(drive_card);
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
  std::cout << name << " เปิดได้จากการ Damage Check: " << damage_card << std::endl;
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
    std::cout << name << " Heal 1 ดาเมจ! (การ์ด '" << healed_card.getName() << "' ไปยัง Drop Zone)" << std::endl;
    return true;
  }
  return false;
}

int Player::chooseUnitForTriggerEffect(const std::string &trigger_effect_description)
{
  std::cout << name << ": " << trigger_effect_description << std::endl;
  std::cout << "เลือกยูนิตที่จะรับผล:" << std::endl;
  std::vector<std::pair<int, std::string>> available_units;
  if (vanguard_circle.has_value())
  {
    available_units.push_back({UNIT_STATUS_VC_IDX, "VC: " + vanguard_circle.value().getName()});
  }
  for (size_t i = 0; i < NUM_REAR_GUARD_CIRCLES; ++i)
  {
    if (rear_guard_circles[i].has_value())
    {
      available_units.push_back({(int)getUnitStatusIndexForRC(i), std::to_string(i) + ": RC " + rear_guard_circles[i].value().getName()});
    }
  }
  if (available_units.empty())
  {
    std::cout << "ไม่มียูนิตในสนามให้เลือก" << std::endl;
    return -1;
  }
  for (size_t i = 0; i < available_units.size(); ++i)
  {
    std::cout << i << ": " << available_units[i].second << std::endl;
  }
  int choice_idx = -1;
  std::cout << "เลือกหมายเลขยูนิต (-1 หากไม่ต้องการให้ผลกับยูนิตใด): "; // เพิ่ม -1
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
    std::cerr << "*** ERROR: ไม่สามารถ Guard ด้วยการ์ดตำแหน่ง " << hand_card_index << " (ไม่มีการ์ด) ***" << std::endl;
    return -1;
  }
  Card card_to_guard = hand[hand_card_index];
  guardian_zone.push_back(card_to_guard);
  int shield_value = card_to_guard.getShield();
  hand.erase(hand.begin() + static_cast<long>(hand_card_index));
  std::cout << name << " ใช้ '" << card_to_guard.getName() << "' (Shield: " << shield_value << ") ในการ Guard." << std::endl;
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

// performGuardStep จะถูกเรียกจาก main.cpp และ main.cpp จะ loop ถามว่าจะ Guard เพิ่มไหม
// ฟังก์ชันนี้จะแสดงมือและให้เลือก Guard 1 ครั้ง
// คืนค่า Shield รวมที่ได้จากการ Guard ทั้งหมดในขั้นตอนนี้
int Player::performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt)
{
  // การแสดงผลและรับ input จะถูกจัดการใน main.cpp โดยเรียก addCardToGuardianZoneFromHand
  // ฟังก์ชันนี้จะถูกเรียกซ้ำๆ จาก main.cpp ถ้าผู้เล่นต้องการ Guard เพิ่ม
  // ดังนั้นฟังก์ชันนี้ควรจะแค่คำนวณ Shield รวมจาก Guardian Zone ปัจจุบัน
  // หรือ main.cpp ควรจะจัดการ loop และการรวม shield เอง
  // เพื่อความง่าย จะให้ main.cpp จัดการ loop และเรียก addCardToGuardianZoneFromHand
  // ดังนั้น performGuardStep อาจจะไม่จำเป็นต้องทำอะไรมาก หรือเปลี่ยนเป็นแค่การแสดงข้อมูล

  // ** แก้ไข: ให้ performGuardStep loop เองตามที่เคยทำ **
  std::cout << "\n--- " << name << ": ขั้นตอนการ Guard ---" << std::endl;
  std::cout << "⚔️ พลังโจมตีที่เข้ามา: " << incoming_attack_power << std::endl;
  if (target_unit_opt.has_value())
  {
    std::cout << "🎯 เป้าหมายคือ: " << target_unit_opt.value().getName()
              << " (Power ปัจจุบัน: " << target_unit_opt.value().getPower() << ")" << std::endl;
  }

  int current_total_shield = getGuardianZoneShieldTotal(); // Shield จากการ Guard ก่อนหน้านี้ใน Battle เดียวกัน (ถ้ามี)
  char continue_guard_choice = 'y';

  while ((continue_guard_choice == 'y' || continue_guard_choice == 'Y'))
  {
    if (hand.empty())
    {
      std::cout << "ไม่เหลือการ์ดบนมือให้ Guard แล้ว!" << std::endl;
      break;
    }
    std::cout << "\nการ์ดบนมือของคุณ (" << name << "):" << std::endl;
    displayHand(true);
    std::cout << "🛡️ Guardian Zone ปัจจุบัน: ";
    displayGuardianZone();
    std::cout << "🛡️ Shield รวมปัจจุบันใน Guardian Zone: " << getGuardianZoneShieldTotal() << std::endl;

    std::cout << "เลือกการ์ดจากมือเพื่อ Guard (-1 เพื่อหยุด Guard, หรือ 'n' ในคำถามถัดไป): ";
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
      addCardToGuardianZoneFromHand(static_cast<size_t>(card_idx)); // addCardToGuardianZoneFromHand จะแสดงผลเอง
    }
    else
    {
      std::cout << "*** เลือกไม่ถูกต้อง ***" << std::endl;
    }

    if (hand.empty())
    {
      std::cout << "ไม่เหลือการ์ดบนมือให้ Guard แล้ว!" << std::endl;
      break;
    }

    std::cout << "ต้องการ Guard เพิ่มหรือไม่ (y/n): ";
    std::cin >> continue_guard_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  std::cout << "จบขั้นตอนการ Guard. Shield ที่ได้ทั้งหมดในรอบนี้: " << getGuardianZoneShieldTotal() << std::endl;
  return getGuardianZoneShieldTotal();
}

void Player::displayGuardianZone() const
{
  if (guardian_zone.empty())
  {
    std::cout << "(ว่าง)";
  }
  else
  {
    for (const auto &card : guardian_zone)
    {
      std::cout << "[" << card.getName() << " S:" << card.getShield() << "] ";
    }
  }
  // ไม่ต้องมี std::endl ถ้า main.cpp จัดการบรรทัด
}

std::string formatCardForDisplay(const std::optional<Card> &card_opt, int width, bool is_standing, bool is_vc)
{
  std::ostringstream oss;
  std::string content;
  if (card_opt.has_value())
  {
    const Card &card = card_opt.value();
    std::string grade_str = "G" + std::to_string(card.getGrade());
    std::string name_str = card.getName();
    std::string status_str = is_standing ? "" : " (R)";
    std::string crit_str = " C" + std::to_string(card.getCritical()); // เพิ่ม Critical
    std::string full_text = grade_str + crit_str + " " + name_str + status_str;
    if (full_text.length() > static_cast<size_t>(width))
    {
      int available_width_for_name = width - grade_str.length() - crit_str.length() - status_str.length() - 1 - 3;
      if (available_width_for_name < 1)
        available_width_for_name = 1;
      name_str = name_str.substr(0, available_width_for_name) + "...";
      content = grade_str + crit_str + " " + name_str + status_str;
    }
    else
    {
      content = full_text;
    }
  }
  else
  {
    content = "[   ว่าง   ]";
  }
  int text_len = 0;
  for (char c : content)
  {
    text_len++;
  }
  int padding = width - text_len;
  int pad_left = padding / 2;
  int pad_right = padding - pad_left;
  oss << std::string(pad_left, ' ') << content << std::string(pad_right, ' ');
  return oss.str();
}

void Player::displayField(bool show_opponent_field_for_targeting) const
{
  const int card_cell_width = 22;
  const std::string V_BORDER = "|";
  const std::string H_BORDER_THIN_SEGMENT = std::string(card_cell_width, '-');
  const std::string H_BORDER_THICK_SEGMENT = std::string(card_cell_width, '=');
  const std::string CORNER = "+";
  std::string row_separator = CORNER;
  for (int i = 0; i < 3; ++i)
  {
    row_separator += H_BORDER_THIN_SEGMENT + CORNER;
  }
  std::string vc_row_separator_left = CORNER + H_BORDER_THIN_SEGMENT + CORNER;
  std::string vc_row_separator_center = H_BORDER_THICK_SEGMENT;
  std::string vc_row_separator_right = CORNER + H_BORDER_THIN_SEGMENT + CORNER;

  std::cout << "\n<<<<< สนามของ: " << name << " >>>>>" << std::endl;
  std::cout << "มือ: " << std::left << std::setw(2) << hand.size()
            << " | เด็ค: " << std::left << std::setw(2) << deck.getSize()
            << " | ดาเมจ: " << std::left << std::setw(1) << damage_zone.size()
            << " | โซล: " << std::left << std::setw(2) << soul.size()
            << " | ดรอป: " << std::left << std::setw(2) << drop_zone.size() << std::endl;
  Player::printDisplayLine('~', 70);
  std::cout << "  แถวหน้า:" << std::endl;
  std::cout << "  " << vc_row_separator_left << vc_row_separator_center << vc_row_separator_right << std::endl;
  std::cout << "  " << V_BORDER << formatCardForDisplay(rear_guard_circles[RC_FRONT_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_FRONT_LEFT)])
            << V_BORDER << formatCardForDisplay(vanguard_circle, card_cell_width, unit_is_standing[UNIT_STATUS_VC_IDX], true)
            << V_BORDER << formatCardForDisplay(rear_guard_circles[RC_FRONT_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_FRONT_RIGHT)])
            << V_BORDER << std::endl;
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (0: RC FL)" : "   (RC FL)")
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << "   (VC)"
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (1: RC FR)" : "   (RC FR)")
            << V_BORDER << std::endl;
  std::cout << "  " << vc_row_separator_left << vc_row_separator_center << vc_row_separator_right << std::endl;
  std::cout << std::endl;
  std::cout << "  แถวหลัง:" << std::endl;
  std::cout << "  " << row_separator << std::endl;
  std::cout << "  " << V_BORDER << formatCardForDisplay(rear_guard_circles[RC_BACK_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_LEFT)])
            << V_BORDER << formatCardForDisplay(rear_guard_circles[RC_BACK_CENTER], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_CENTER)])
            << V_BORDER << formatCardForDisplay(rear_guard_circles[RC_BACK_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_RIGHT)])
            << V_BORDER << std::endl;
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (2: RC BL)" : "   (RC BL)")
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (3: RC BC)" : "   (RC BC)")
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (4: RC BR)" : "   (RC BR)")
            << V_BORDER << std::endl;
  std::cout << "  " << row_separator << std::endl;
  std::cout << "\nDamage Zone (" << damage_zone.size() << "): ";
  if (damage_zone.empty())
  {
    std::cout << "(ยังไม่ได้รับดาเมจ)";
  }
  else
  {
    for (size_t i = 0; i < damage_zone.size(); ++i)
    {
      std::string name_short = damage_zone[i].getName().substr(0, 8);
      if (damage_zone[i].getName().length() > 8)
        name_short += "..";
      std::cout << "[G" << damage_zone[i].getGrade() << " " << name_short << "]";
      if (i < damage_zone.size() - 1)
        std::cout << " ";
    }
  }
  std::cout << std::endl;
  Player::printDisplayLine('=', 70);
}

void Player::displayHand(bool show_details) const
{
  Player::printDisplayLine('-', 36);
  std::cout << "--- มือของ " << name << " (" << hand.size() << " ใบ) ---" << std::endl;
  if (hand.empty())
  {
    std::cout << "(มือว่าง)" << std::endl;
  }
  else
  {
    for (size_t i = 0; i < hand.size(); ++i)
    {
      std::cout << i << ": " << hand[i];
      if (show_details)
      {
        std::cout << " (P:" << hand[i].getPower() << " S:" << hand[i].getShield() << ")";
      }
      std::cout << std::endl;
    }
  }
  Player::printDisplayLine('-', 36);
}

void Player::displayFullStatus() const
{
  displayField();
  displayHand(true);
}
std::string Player::getName() const { return name; }
size_t Player::getHandSize() const { return hand.size(); }
const std::vector<Card> &Player::getHand() const { return hand; }
size_t Player::getDamageCount() const { return damage_zone.size(); }
const std::optional<Card> &Player::getVanguard() const { return vanguard_circle; }
const std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> &Player::getRearGuards() const { return rear_guard_circles; }
Deck &Player::getDeck() { return deck; }
const Deck &Player::getDeck() const { return deck; }

void Player::takeDamage(const Card &damage_card)
{
  std::cout << "🩸 " << name << " ได้รับ 1 ดาเมจ! การ์ดที่ตก Damage Zone: " << damage_card.getName() << std::endl;
  damage_zone.push_back(damage_card);
  if (getDamageCount() >= 6)
  {
    std::cout << "*** 💀 " << name << " ได้รับ 6 ดาเมจแล้ว! " << name << " แพ้แล้ว! 💀 ***" << std::endl;
  }
}
void Player::placeCardIntoSoul(const Card &card)
{
  soul.push_back(card);
}
void Player::discardFromHandToDrop(size_t hand_card_index)
{
  if (hand_card_index < hand.size())
  {
    Card discarded_card = hand[hand_card_index];
    drop_zone.push_back(discarded_card);
    hand.erase(hand.begin() + hand_card_index);
    std::cout << "🗑️ " << name << " ทิ้งการ์ด '" << discarded_card.getName() << "' จากมือลง Drop Zone." << std::endl;
  }
}
void Player::clearGuardianZoneAndMoveToDrop()
{
  if (!guardian_zone.empty())
  {
    for (const auto &card : guardian_zone)
    {
      drop_zone.push_back(card);
    }
    guardian_zone.clear();
  }
}
