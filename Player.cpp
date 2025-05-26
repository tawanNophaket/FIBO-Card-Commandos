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
  current_battle_power_buffs.fill(0);
  current_battle_crit_buffs.fill(0);
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
  if (vanguard_circle.value().getCodeName() == "G0-01")
  { // สกิล Starter เบเบี้ตี๋: เมื่อถูก Ride : จั่ว 1
    // แต่ setupGame คือการ *วาง* Starter ไม่ใช่ *Ride ทับ* Starter
    // สกิลนี้จะทำงานเมื่อมีการ Ride G1 ทับ G0-01 จริงๆ
  }
  deck.shuffle();
  drawCards(initial_hand_size);
  return true;
}

void Player::performStandPhase()
{
  unit_is_standing.fill(true);
  resetBattleBuffs();
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

  std::optional<Card> old_vg_card = vanguard_circle;

  if (vanguard_circle.has_value())
  {
    soul.push_back(vanguard_circle.value());
    if (old_vg_card.has_value() && old_vg_card.value().getCodeName() == "G0-01")
    {
      std::cout << "✨ สกิล Starter (" << old_vg_card.value().getName() << ") ทำงาน: " << name << " จั่ว 1 ใบ!" << std::endl;
      drawCards(1);
    }
  }
  vanguard_circle = card_to_ride;
  unit_is_standing[UNIT_STATUS_VC_IDX] = true;
  current_battle_power_buffs[UNIT_STATUS_VC_IDX] = 0;
  current_battle_crit_buffs[UNIT_STATUS_VC_IDX] = 0;
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
  size_t status_idx = getUnitStatusIndexForRC(rc_slot_index);
  unit_is_standing[status_idx] = true;
  current_battle_power_buffs[status_idx] = 0;
  current_battle_crit_buffs[status_idx] = 0;
  hand.erase(hand.begin() + hand_card_index);

  // --- สกิล On-Place: G1-04 เกม ---
  if (card_to_call.getCodeName() == "G1-04")
 {
    std::cout << "✨ สกิลของ '" << card_to_call.getName() << "' (วางที่ RC) ทำงาน: จั่ว 1 ใบ แล้วทิ้ง 1 ใบ!" << std::endl;
    drawCards(1);
    if (!hand.empty())
    {
      std::cout << "เลือกการ์ดที่จะทิ้งจากมือสำหรับสกิล '" << card_to_call.getName() << "':" << std::endl;
      displayHand(true);
      // การรับ input ควรจะทำใน main.cpp หรือมี helper ที่ปลอดภัยกว่านี้
      int discard_idx = -1;
      std::string s_discard_idx;
      std::cout << "ใส่หมายเลขการ์ดที่จะทิ้ง (-1 ถ้าไม่มี/ไม่ต้องการ): ";
      std::cin >> s_discard_idx;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      try
      {
        discard_idx = std::stoi(s_discard_idx);
      }
      catch (...)
      {
        discard_idx = -2;
      }

      if (discard_idx >= 0 && static_cast<size_t>(discard_idx) < hand.size())
      {
        discardFromHandToDrop(static_cast<size_t>(discard_idx));
      }
      else
      {
        std::cout << "เลือกไม่ถูกต้อง หรือไม่ต้องการทิ้ง." << std::endl;
      }
    }
    else
    {
      std::cout << "ไม่มีการ์ดบนมือให้ทิ้ง" << std::endl;
    }
  }
  return true;
}

std::vector<std::pair<int, std::string>> Player::chooseAttacker()
{
  std::vector<std::pair<int, std::string>> available_attackers;
  if (vanguard_circle.has_value() && unit_is_standing[UNIT_STATUS_VC_IDX])
  {
    available_attackers.push_back({UNIT_STATUS_VC_IDX, "VC: " + vanguard_circle.value().getName() + " (P:" + std::to_string(getUnitPowerAtStatusIndex(UNIT_STATUS_VC_IDX)) + " C:" + std::to_string(getUnitCriticalAtStatusIndex(UNIT_STATUS_VC_IDX)) + ")"});
  }
  const size_t front_row_rcs[] = {RC_FRONT_LEFT, RC_FRONT_RIGHT};
  for (size_t rc_idx : front_row_rcs)
  {
    size_t status_idx = getUnitStatusIndexForRC(rc_idx);
    if (rear_guard_circles[rc_idx].has_value() && unit_is_standing[status_idx])
    {
      available_attackers.push_back({(int)status_idx,
                                     std::to_string(rc_idx) + ": RC " + rear_guard_circles[rc_idx].value().getName() + " (P:" + std::to_string(getUnitPowerAtStatusIndex(status_idx)) + " C:" + std::to_string(getUnitCriticalAtStatusIndex(status_idx)) + ")"});
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
                << " (G" << rear_guard_circles[booster_rc_idx].value().getGrade() << " P+" << rear_guard_circles[booster_rc_idx].value().getPower() << ") ได้" << std::endl;
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
  if (unit_status_idx >= 0 && static_cast<size_t>(unit_status_idx) < NUM_FIELD_UNITS)
  {
    total_power += current_battle_power_buffs[static_cast<size_t>(unit_status_idx)];
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

int Player::getUnitCriticalAtStatusIndex(int unit_status_idx) const
{
  int total_crit = 0;
  std::optional<Card> unit_opt = getUnitAtStatusIndex(unit_status_idx);
  if (unit_opt.has_value())
  {
    total_crit += unit_opt.value().getCritical();
  }
  if (unit_status_idx >= 0 && static_cast<size_t>(unit_status_idx) < NUM_FIELD_UNITS)
  {
    total_crit += current_battle_crit_buffs[static_cast<size_t>(unit_status_idx)];
  }
  return total_crit > 0 ? total_crit : 1;
}

void Player::resetBattleBuffs()
{
  current_battle_power_buffs.fill(0);
  current_battle_crit_buffs.fill(0);
}

int Player::chooseUnitForTriggerEffect(const std::string &trigger_effect_description, bool can_choose_any_unit)
{
  std::cout << "\n✨ " << name << " - ผลจาก Trigger ✨" << std::endl;
  std::cout << trigger_effect_description << std::endl;
  std::cout << "เลือกยูนิตที่จะรับผล:" << std::endl;
  std::vector<std::pair<int, std::string>> available_units;
  if (vanguard_circle.has_value())
  {
    available_units.push_back({UNIT_STATUS_VC_IDX, "VC: " + vanguard_circle.value().getName()});
  }
  if (can_choose_any_unit)
  {
    for (size_t i = 0; i < NUM_REAR_GUARD_CIRCLES; ++i)
    {
      if (rear_guard_circles[i].has_value())
      {
        available_units.push_back({(int)getUnitStatusIndexForRC(i), std::to_string(i) + ": RC " + rear_guard_circles[i].value().getName()});
      }
    }
  }
  if (available_units.empty())
  {
    std::cout << "ไม่มียูนิตในสนามให้เลือกรับผล Trigger" << std::endl;
    return -1;
  }
  for (size_t i = 0; i < available_units.size(); ++i)
  {
    std::cout << i << ": " << available_units[i].second << std::endl;
  }
  int choice_idx = -1;
  // การรับ input ควรจะใช้ getIntegerInput จาก main.cpp
  // แต่ใน Player class อาจจะต้องมี getIntegerInput ของตัวเอง หรือรับค่าจาก main
  std::cout << "เลือกหมายเลขยูนิต (-1 หากไม่ต้องการให้ผล): ";
  std::string input_str;
  std::cin >> input_str;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  try
  {
    choice_idx = std::stoi(input_str);
  }
  catch (...)
  {
    choice_idx = -2; // Invalid input
  }

  if (choice_idx >= 0 && static_cast<size_t>(choice_idx) < available_units.size())
  {
    return available_units[static_cast<size_t>(choice_idx)].first;
  }
  std::cout << "การเลือกไม่ถูกต้อง หรือไม่เลือกยูนิตสำหรับ Trigger" << std::endl;
  return -1;
}

TriggerOutput Player::applyTriggerEffect(const Card &trigger_card, bool is_drive_check, Player *opponent_for_heal_check, int &unit_idx_target_for_power_crit)
{
  TriggerOutput output_for_main_calc; // จะคืนค่า Power/Crit ที่จะบวกเพิ่มให้ Attacker/Defending VG โดยตรง
  std::cout << "เปิดได้ Trigger: " << trigger_card.getName() << " (" << trigger_card.getTypeRole() << ")!" << std::endl;

  int power_bonus_from_trigger = 10000;
  int chosen_unit_status_idx = -1;

  if (trigger_card.getTypeRole() == "Trigger - Critical")
  {
    chosen_unit_status_idx = chooseUnitForTriggerEffect("เลือกยูนิตรับ +10000 Power และ +1 Critical");
    if (chosen_unit_status_idx != -1)
    {
      current_battle_power_buffs[chosen_unit_status_idx] += power_bonus_from_trigger;
      std::cout << getUnitAtStatusIndex(chosen_unit_status_idx).value().getName() << " ได้รับ +" << power_bonus_from_trigger << " Power!" << std::endl;
      if (is_drive_check)
      {
        current_battle_crit_buffs[chosen_unit_status_idx] += 1;
        std::cout << getUnitAtStatusIndex(chosen_unit_status_idx).value().getName() << " ได้รับ +1 Critical!" << std::endl;
        if (chosen_unit_status_idx == unit_idx_target_for_power_crit)
        { // ถ้าผู้เล่นเลือก Attacking VG
          output_for_main_calc.extra_power += power_bonus_from_trigger;
          output_for_main_calc.extra_crit += 1;
        }
      }
      else
      { // Damage Check Crit Trigger
        if (chosen_unit_status_idx == UNIT_STATUS_VC_IDX)
        { // Power ให้ VG ตัวเอง
          output_for_main_calc.extra_power += power_bonus_from_trigger;
        }
      }
    }
  }
  else if (trigger_card.getTypeRole() == "Trigger - Draw")
  {
    chosen_unit_status_idx = chooseUnitForTriggerEffect("เลือกยูนิตรับ +10000 Power");
    if (chosen_unit_status_idx != -1)
    {
      current_battle_power_buffs[chosen_unit_status_idx] += power_bonus_from_trigger;
      std::cout << getUnitAtStatusIndex(chosen_unit_status_idx).value().getName() << " ได้รับ +" << power_bonus_from_trigger << " Power!" << std::endl;
      if (is_drive_check && chosen_unit_status_idx == unit_idx_target_for_power_crit)
      {
        output_for_main_calc.extra_power += power_bonus_from_trigger;
      }
      else if (!is_drive_check && chosen_unit_status_idx == UNIT_STATUS_VC_IDX)
      {
        output_for_main_calc.extra_power += power_bonus_from_trigger;
      }
    }
    drawCards(1);
    std::cout << name << " จั่ว 1 ใบจาก Draw Trigger. (มือ: " << hand.size() << ")" << std::endl;
  }
  else if (trigger_card.getTypeRole() == "Trigger - Heal")
  {
    chosen_unit_status_idx = chooseUnitForTriggerEffect("เลือกยูนิตรับ +10000 Power");
    if (chosen_unit_status_idx != -1)
    {
      current_battle_power_buffs[chosen_unit_status_idx] += power_bonus_from_trigger;
      std::cout << getUnitAtStatusIndex(chosen_unit_status_idx).value().getName() << " ได้รับ +" << power_bonus_from_trigger << " Power!" << std::endl;
      if (is_drive_check && chosen_unit_status_idx == unit_idx_target_for_power_crit)
      {
        output_for_main_calc.extra_power += power_bonus_from_trigger;
      }
      else if (!is_drive_check && chosen_unit_status_idx == UNIT_STATUS_VC_IDX)
      {
        output_for_main_calc.extra_power += power_bonus_from_trigger;
      }
    }
    bool condition_met = (opponent_for_heal_check && getDamageCount() > 0 && getDamageCount() >= opponent_for_heal_check->getDamageCount()) ||
                         (!opponent_for_heal_check && getDamageCount() > 0);
    if (condition_met)
    {
      if (healOneDamage())
      {
        std::cout << "และ Heal 1 ดาเมจ!" << std::endl;
      }
    }
    else
    {
      std::cout << "(แต่เงื่อนไข Heal ไม่สำเร็จ)" << std::endl;
    }
  }
  return output_for_main_calc;
}

TriggerOutput Player::performDriveCheck(int num_drives, Player *opponent_for_heal_check, int attacking_vg_idx)
{
  TriggerOutput combined_output_for_vg_attack;

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
      std::cout << "💎 Drive Check ครั้งที่ " << (i + 1) << ": " << drive_card << std::endl;
      hand.push_back(drive_card);
      if (drive_card.getTypeRole().find("Trigger") != std::string::npos)
      {
        // attacking_vg_idx คือ unit status index ของ VG ที่กำลังโจมตี
        TriggerOutput effect = applyTriggerEffect(drive_card, true, opponent_for_heal_check, attacking_vg_idx);
        // ผล Power/Crit ที่จะให้ VG ที่โจมตีโดยตรง จะถูกคืนค่ามาใน effect แล้ว main.cpp จะบวกเพิ่ม
        combined_output_for_vg_attack.extra_power += effect.extra_power;
        combined_output_for_vg_attack.extra_crit += effect.extra_crit;
      }
    }
  }
  return combined_output_for_vg_attack;
}

bool Player::healOneDamage() { /* เหมือนเดิม */ return false; }
int Player::addCardToGuardianZoneFromHand(size_t hand_card_index)
{ /* เหมือนเดิม แต่เพิ่ม logic PG */
  if (hand_card_index >= hand.size())
  {
    return -1;
  }
  Card card_to_guard = hand[hand_card_index];

  if (card_to_guard.getCodeName() == "G1-02")
  { // "เล้ง" - Sentinel • Perfect Guard
    std::cout << "คุณต้องการใช้สกิล Perfect Guard ของ '" << card_to_guard.getName() << "' หรือไม่?" << std::endl;
    std::cout << "(ต้องทิ้งการ์ด 1 ใบจากมือเพื่อยกเลิกการโจมตี): " << std::endl;
    char pg_choice = 'n';
    // การรับ input ควรจะมาจาก getActionInput ใน main.cpp
    std::cout << "ใช้สกิล Perfect Guard? (y/n): ";
    std::cin >> pg_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (pg_choice == 'y' || pg_choice == 'Y')
    {
      if (hand.size() > 1)
      {
        std::cout << "เลือกการ์ดที่จะทิ้งเพื่อใช้ Perfect Guard (ยกเว้นตัว PG เอง):" << std::endl;
        displayHand(false);
        int discard_idx = -1;
        std::cout << "ใส่หมายเลขการ์ดที่จะทิ้ง: ";
        std::cin >> discard_idx;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // ตรวจสอบว่า index ที่เลือกไม่ใช่ตัว PG ที่กำลังจะใช้
        if (discard_idx >= 0 && static_cast<size_t>(discard_idx) < hand.size() && static_cast<size_t>(discard_idx) != hand_card_index)
        {
          Card card_to_discard_for_pg = hand[static_cast<size_t>(discard_idx)];
          // ย้าย PG ไป Guardian Zone ก่อน แล้วค่อยลบออกจากมือ
          guardian_zone.push_back(card_to_guard);

          // ลบ PG และการ์ดที่ทิ้งออกจากมือ
          // ต้องระวังเรื่อง index เปลี่ยนถ้าการ์ดที่ทิ้งอยู่ก่อน PG ใน vector
          size_t pg_original_idx = hand_card_index;
          size_t discard_original_idx = static_cast<size_t>(discard_idx);

          if (pg_original_idx < discard_original_idx)
          {
            hand.erase(hand.begin() + discard_original_idx);
            hand.erase(hand.begin() + pg_original_idx);
          }
          else
          { // discard_original_idx < pg_original_idx
            hand.erase(hand.begin() + pg_original_idx);
            hand.erase(hand.begin() + discard_original_idx);
          }
          drop_zone.push_back(card_to_discard_for_pg); // การ์ดที่ทิ้งไป Drop

          std::cout << "ใช้ Perfect Guard! ทิ้ง '" << card_to_discard_for_pg.getName() << "' การโจมตีนี้จะถูกยกเลิก!" << std::endl;
          return 999999;
        }
        else
        {
          std::cout << "เลือกการ์ดทิ้งไม่ถูกต้อง หรือไม่มีการ์ดพอจะทิ้ง. ใช้ '" << card_to_guard.getName() << "' Guard ตามปกติ." << std::endl;
        }
      }
      else
      {
        std::cout << "ไม่มีการ์ดพอจะทิ้งเพื่อใช้ Perfect Guard. ใช้ '" << card_to_guard.getName() << "' Guard ตามปกติ." << std::endl;
      }
    }
  }
  // ถ้าไม่ใช่ PG หรือไม่ใช้สกิล PG
  guardian_zone.push_back(card_to_guard);
  int shield_value = card_to_guard.getShield();
  hand.erase(hand.begin() + hand_card_index);
  std::cout << name << " ใช้ '" << card_to_guard.getName() << "' (Shield: " << shield_value << ") ในการ Guard." << std::endl;
  return shield_value;
}
int Player::getGuardianZoneShieldTotal() const { /* เหมือนเดิม */ return 0; }
int Player::performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt, Player *attacker /* เพิ่ม Attacker */)
{
  std::cout << "\n🛡️ --- " << name << ": ขั้นตอนการ Guard --- 🛡️" << std::endl;
  std::cout << "⚔️ พลังโจมตีที่เข้ามา: " << incoming_attack_power << std::endl;
  if (target_unit_opt.has_value())
  { /* ... แสดงเป้าหมาย ... */
}

  int final_total_shield = 0;
  char continue_guard_choice = 'y';

  // --- Intercept Step ---
  std::vector<std::pair<int, size_t>> available_interceptors;
  const size_t front_row_rcs_for_intercept[] = {RC_FRONT_LEFT, RC_FRONT_RIGHT};
  std::cout << "\n--- Intercept Step ---" << std::endl;
  for (size_t rc_idx : front_row_rcs_for_intercept)
  {
    size_t status_idx = getUnitStatusIndexForRC(rc_idx);
    if (rear_guard_circles[rc_idx].has_value() && rear_guard_circles[rc_idx].value().getGrade() == 2 && unit_is_standing[status_idx])
    {
      available_interceptors.push_back({(int)status_idx, rc_idx});
    }
  }

  if (!available_interceptors.empty())
  {
    std::cout << "ยูนิตที่สามารถ Intercept ได้:" << std::endl;
    for (size_t i = 0; i < available_interceptors.size(); ++i)
    {
      std::cout << i << ": " << rear_guard_circles[available_interceptors[i].second].value().getName() << " (G2 แถวหน้า, Slot " << available_interceptors[i].second << ")" << std::endl;
    }
    // การรับ input ควรใช้ getIntegerInput จาก main.cpp หรือ helper ที่ปลอดภัย
    std::string s_intercept_idx;
    std::cout << "เลือกหมายเลขยูนิตที่จะ Intercept (-1 หากไม่ต้องการ): ";
    std::cin >> s_intercept_idx;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    int intercept_choice_idx = -1;
    try
    {
      intercept_choice_idx = std::stoi(s_intercept_idx);
    }
    catch (...)
    {
      intercept_choice_idx = -2;
    }

    if (intercept_choice_idx >= 0 && static_cast<size_t>(intercept_choice_idx) < available_interceptors.size())
    {
      int interceptor_status_idx = available_interceptors[static_cast<size_t>(intercept_choice_idx)].first;
      size_t interceptor_rc_idx = available_interceptors[static_cast<size_t>(intercept_choice_idx)].second;

      Card interceptor_card = rear_guard_circles[interceptor_rc_idx].value();
      guardian_zone.push_back(interceptor_card);
      rear_guard_circles[interceptor_rc_idx] = std::nullopt;
      restUnit(interceptor_status_idx);

      int intercept_shield = 5000;
      // G1-03 คิน ไม่ใช่ G2 จึงไม่ควรเข้าเงื่อนไข Intercept ปกติ
      // แต่ถ้าคุณต้องการให้ G1-03 คิน มีสกิลคล้าย Intercept ต้องเขียน logic แยก
      // if (interceptor_card.getCodeName() == "G1-03"){
      //     intercept_shield = interceptor_card.getShield() + 5000;
      // }

      std::cout << "ใช้ '" << interceptor_card.getName() << "' Intercept! + " << intercept_shield << " Shield." << std::endl;
    }
  }
  else
  {
    std::cout << "ไม่มียูนิตที่สามารถ Intercept ได้" << std::endl;
  }

  final_total_shield = getGuardianZoneShieldTotal();

  // --- Guard from Hand Step ---
  std::cout << "\n--- Guard from Hand Step ---" << std::endl;
  while ((continue_guard_choice == 'y' || continue_guard_choice == 'Y'))
  {
    if (final_total_shield == 999999)
      break; // ถ้า PG ทำงานแล้ว
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

    std::string s_idx;
    std::cout << "เลือกการ์ดจากมือเพื่อ Guard (-1 เพื่อหยุด Guard, หรือพิมพ์ 'n' ในคำถามถัดไป): ";
    std::cin >> s_idx;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    int card_idx = -1;
    try
    {
      card_idx = std::stoi(s_idx);
    }
    catch (...)
    {
      card_idx = -2;
    }

    if (card_idx == -1)
      break;

    if (card_idx >= 0 && static_cast<size_t>(card_idx) < hand.size())
    {
      int added_shield_or_pg = addCardToGuardianZoneFromHand(static_cast<size_t>(card_idx));
      if (added_shield_or_pg == 999999)
      {
        final_total_shield = 999999;
        break;
      }
      // ไม่ต้องบวกเพิ่มถ้า PG ไม่ทำงาน เพราะ getGuardianZoneShieldTotal() จะคำนวณให้ใหม่
    }
    else
    {
      std::cout << "*** เลือกไม่ถูกต้อง ***" << std::endl;
    }
    final_total_shield = getGuardianZoneShieldTotal();

    if (hand.empty() || final_total_shield == 999999)
    {
      break;
}

    std::cout << "ต้องการ Guard เพิ่มหรือไม่ (y/n): ";
    std::cin >> continue_guard_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  final_total_shield = getGuardianZoneShieldTotal();
  std::cout << "จบขั้นตอนการ Guard. Shield ที่ได้ทั้งหมดจาก Guardian Zone: " << final_total_shield << std::endl;
  return final_total_shield;
}

void Player::displayGuardianZone() const { /* เหมือนเดิม */ }
std::string formatCardForDisplay(const std::optional<Card> &card_opt, int width, bool is_standing, bool is_vc) { /* เหมือนเดิม */ }
void Player::displayField(bool show_opponent_field_for_targeting) const { /* เหมือนเดิม */ }
void Player::displayHand(bool show_details) const { /* เหมือนเดิม */ }
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
void Player::takeDamage(const Card &damage_card) { /* เหมือนเดิม */ }
void Player::placeCardIntoSoul(const Card &card) { /* เหมือนเดิม */ }
void Player::discardFromHandToDrop(size_t hand_card_index) { /* เหมือนเดิม */ }
void Player::clearGuardianZoneAndMoveToDrop() { /* เหมือนเดิม */ }
