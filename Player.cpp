// Player.cpp - ไฟล์ Source สำหรับ υλολοποίηση คลาส Player
#include "Player.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <limits>

// Static helper function definition for Player class (or make it a free function if preferred)
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
      // std::cout << "*** ผู้เล่น '" << name << "' ไม่สามารถจั่วการ์ดได้ เด็คหมด! ***" << std::endl;
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

// --- Battle Phase Actions ---
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
  else if (unit_status_idx > 0 && static_cast<size_t>(unit_status_idx) <= NUM_REAR_GUARD_CIRCLES + 1)
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
  { // Booster มีผลเฉพาะตอนโจมตี
    std::optional<Card> booster_opt = getUnitAtStatusIndex(booster_unit_status_idx);
    if (booster_opt.has_value())
    {
      total_power += booster_opt.value().getPower();
    }
  }
  return total_power;
}

// *** Implement เมธอดสำหรับ Guard ***
int Player::performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt)
{
  std::cout << "\n--- " << name << ": ขั้นตอนการ Guard ---" << std::endl;
  std::cout << "พลังโจมตีที่เข้ามา: " << incoming_attack_power << std::endl;
  if (target_unit_opt.has_value())
  {
    std::cout << "เป้าหมายคือ: " << target_unit_opt.value().getName()
              << " (Power ปัจจุบัน: " << target_unit_opt.value().getPower() << ")" << std::endl;
  }

  int total_shield_value = 0;
  char guard_choice = 'y';

  // (ยังไม่ได้ implement Perfect Guard หรือ Intercept ในขั้นนี้)

  while (guard_choice == 'y' || guard_choice == 'Y')
  {
    if (hand.empty())
    {
      std::cout << "ไม่เหลือการ์ดบนมือให้ Guard แล้ว!" << std::endl;
      break;
    }
    std::cout << "\nการ์ดบนมือของคุณ:" << std::endl;
    displayHand(true); // แสดง power/shield
    std::cout << "Guardian Zone ปัจจุบัน: ";
    displayGuardianZone();
    std::cout << "Shield รวมปัจจุบัน: " << total_shield_value << std::endl;

    // (main.cpp ควรจะเรียก getActionInput)
    // นี่เป็นแค่โครงเบื้องต้น การรับ input จริงๆ จะซับซ้อนกว่านี้ใน main.cpp
    int card_idx_to_guard = -1;
    std::cout << "เลือกหมายเลขการ์ดที่จะใช้ Guard (-1 เพื่อหยุด Guard): ";
    std::cin >> card_idx_to_guard;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (card_idx_to_guard == -1)
    {
      break;
    }

    if (card_idx_to_guard >= 0 && static_cast<size_t>(card_idx_to_guard) < hand.size())
    {
      Card card_for_guard = hand[static_cast<size_t>(card_idx_to_guard)];
      // ตรวจสอบว่าเป็น PG หรือไม่ (ยังไม่ได้ทำ)
      // G1-02 เล้ง (Sentinel • Perfect Guard) - สกิล: AUTO วาง GC : ทิ้งการ์ด 1 → ยกเลิกการโจมตี นั้น
      // ในที่นี้จะใช้ Shield value ของการ์ดไปก่อน
      guardian_zone.push_back(card_for_guard);
      total_shield_value += card_for_guard.getShield();
      hand.erase(hand.begin() + static_cast<size_t>(card_idx_to_guard));
      std::cout << "ใช้ '" << card_for_guard.getName() << "' (Shield: " << card_for_guard.getShield()
                << ") ในการ Guard. Shield รวม: " << total_shield_value << std::endl;
    }
    else
    {
      std::cout << "หมายเลขการ์ดไม่ถูกต้อง!" << std::endl;
    }
    // ถามว่าจะ Guard อีกไหม (main.cpp จะจัดการ loop นี้)
  }
  std::cout << "จบขั้นตอนการ Guard. Shield ที่ได้ทั้งหมด: " << total_shield_value << std::endl;
  return total_shield_value;
}

void Player::displayGuardianZone() const
{
  if (guardian_zone.empty())
  {
    std::cout << "(ว่าง)" << std::endl;
  }
  else
  {
    for (const auto &card : guardian_zone)
    {
      std::cout << "[" << card.getName() << " S:" << card.getShield() << "] ";
    }
    std::cout << std::endl;
  }
}

// --- Information Display ---
std::string formatCardForDisplay(const std::optional<Card> &card_opt, int width, bool is_standing = true, bool is_vc = false)
{
  std::ostringstream oss;
  std::string content;

  if (card_opt.has_value())
  {
    const Card &card = card_opt.value();
    std::string grade_str = "G" + std::to_string(card.getGrade());
    std::string name_str = card.getName();
    std::string status_str = is_standing ? "" : " (R)";

    std::string full_text = grade_str + " " + name_str + status_str;
    if (full_text.length() > static_cast<size_t>(width))
    {
      int available_width_for_name = width - grade_str.length() - status_str.length() - 1 - 3;
      if (available_width_for_name < 1)
        available_width_for_name = 1;
      name_str = name_str.substr(0, available_width_for_name) + "...";
      content = grade_str + " " + name_str + status_str;
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
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << "   (RC FL)"
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << "   (VC)"
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << "   (RC FR)"
            << V_BORDER << std::endl;
  std::cout << "  " << vc_row_separator_left << vc_row_separator_center << vc_row_separator_right << std::endl;
  std::cout << std::endl;

  std::cout << "  แถวหลัง:" << std::endl;
  std::cout << "  " << row_separator << std::endl;
  std::cout << "  " << V_BORDER << formatCardForDisplay(rear_guard_circles[RC_BACK_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_LEFT)])
            << V_BORDER << formatCardForDisplay(rear_guard_circles[RC_BACK_CENTER], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_CENTER)])
            << V_BORDER << formatCardForDisplay(rear_guard_circles[RC_BACK_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_RIGHT)])
            << V_BORDER << std::endl;
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << "   (RC BL)"
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << "   (RC BC)"
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << "   (RC BR)"
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
    std::cout << "การ์ดใน Guardian Zone (" << name << "): ";
    for (const auto &card : guardian_zone)
    {
      std::cout << "[" << card.getName() << "] ";
      drop_zone.push_back(card); // ย้ายไป Drop Zone
    }
    std::cout << "ถูกย้ายไป Drop Zone." << std::endl;
    guardian_zone.clear();
  }
}
