// Player.cpp - ไฟล์ Source สำหรับ υλολοποίηση คลาส Player
#include "Player.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <limits>
#include <vector> // สำหรับ Mulligan

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
    if (deck.isEmpty())
    { // ตรวจสอบก่อนจั่ว
      std::cout << "*** " << name << " ไม่สามารถจั่วการ์ดได้ เด็คหมด! ***" << std::endl;
      break;
    }
    std::optional<Card> drawn_card = deck.draw();
    if (drawn_card.has_value())
    {
      hand.push_back(drawn_card.value());
    }
    else
    {
      // ควรจะไม่เกิดถ้าเช็ค isEmpty() ก่อน
      break;
    }
  }
}

// --- Setup & Mulligan ---
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

void Player::performMulligan()
{
  // แสดงมือปัจจุบัน
  std::cout << "\n--- " << name << ": MULLIGAN PHASE ---" << std::endl;
  if (hand.empty())
  {
    std::cout << "ไม่มีการ์ดบนมือให้ Mulligan" << std::endl;
    return;
  }
  displayHand(false); // แสดงมือแบบไม่ละเอียด

  std::cout << "เลือกการ์ดที่คุณต้องการเปลี่ยน (ใส่หมายเลข คั่นด้วยเว้นวรรค, หรือพิมพ์ 'n' ถ้าไม่ต้องการเปลี่ยน): ";
  std::string line;
  std::getline(std::cin, line);

  if (line == "n" || line == "N" || line.empty())
  {
    std::cout << name << " ไม่ทำการ Mulligan." << std::endl;
    return;
  }

  std::stringstream ss(line);
  std::vector<int> indices_to_mulligan;
  int index;
  while (ss >> index)
  {
    if (index >= 0 && static_cast<size_t>(index) < hand.size())
    {
      // ตรวจสอบว่า index ไม่ซ้ำ
      if (std::find(indices_to_mulligan.begin(), indices_to_mulligan.end(), index) == indices_to_mulligan.end())
      {
        indices_to_mulligan.push_back(index);
      }
    }
    else
    {
      std::cout << "หมายเลข " << index << " ไม่ถูกต้อง จะถูกข้ามไป." << std::endl;
    }
  }

  if (indices_to_mulligan.empty())
  {
    std::cout << name << " ไม่ได้เลือกการ์ดสำหรับ Mulligan." << std::endl;
    return;
  }

  // เรียง index จากมากไปน้อยเพื่อลบจาก vector ได้ถูกต้อง
  std::sort(indices_to_mulligan.rbegin(), indices_to_mulligan.rend());

  std::vector<Card> cards_returned_to_deck;
  for (int idx : indices_to_mulligan)
  {
    cards_returned_to_deck.push_back(hand[static_cast<size_t>(idx)]);
    hand.erase(hand.begin() + static_cast<size_t>(idx));
  }

  std::cout << name << " นำ " << cards_returned_to_deck.size() << " ใบกลับเข้าเด็ค: ";
  for (const auto &c : cards_returned_to_deck)
  {
    std::cout << "[" << c.getName() << "] ";
  }
  std::cout << std::endl;

  deck.addCardsToBottom(cards_returned_to_deck); // เพิ่มการ์ดกลับไปใต้เด็ค
  deck.shuffle();                                // สับเด็คใหม่

  std::cout << name << " จั่วการ์ดใหม่ " << cards_returned_to_deck.size() << " ใบ." << std::endl;
  drawCards(cards_returned_to_deck.size()); // จั่วการ์ดเท่าจำนวนที่ใส่คืน
  displayHand(false);
}

// --- Turn Phases ---
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

// --- Ride & Call ---
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
    placeCardIntoSoul(vanguard_circle.value()); // ใช้เมธอดนี้เพื่อความชัดเจน
    if (old_vg_card.has_value() && old_vg_card.value().getCodeName() == "G0-01")
    { // เบเบี้ตี๋
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
  Card called_card_copy = hand[hand_card_index]; // เก็บ copy ก่อนลบ
  hand.erase(hand.begin() + hand_card_index);

  // --- สกิล On-Place: G1-04 เกม ---
  if (called_card_copy.getCodeName() == "G1-04")
  {
    std::cout << "✨ สกิลของ '" << called_card_copy.getName() << "' (วางที่ RC) ทำงาน: จั่ว 1 ใบ แล้วทิ้ง 1 ใบ!" << std::endl;
    drawCards(1);
    if (!hand.empty())
    {
      std::cout << "เลือกการ์ดที่จะทิ้งจากมือสำหรับสกิล '" << called_card_copy.getName() << "':" << std::endl;
      displayHand(true);
      int discard_idx = -1;
      // ควรจะเรียก getIntegerInput จาก main.cpp หรือมี helper ที่ปลอดภัย
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
        std::cout << "เลือกไม่ถูกต้อง หรือไม่ต้องการทิ้ง. (อาจจะทิ้งใบสุดท้ายถ้าจำเป็น)" << std::endl;
        if (!hand.empty())
          discardFromHandToDrop(hand.size() - 1); // Default behavior for simplicity
      }
    }
    else
    {
      std::cout << "ไม่มีการ์ดบนมือให้ทิ้ง" << std::endl;
    }
  }
  return true;
}

// --- Battle Phase Actions ---
std::vector<std::pair<int, std::string>> Player::chooseAttacker() { /* เหมือนเดิม */ return {}; }
int Player::chooseBooster(int attacker_unit_status_idx) { /* เหมือนเดิม */ return -1; }
void Player::restUnit(int unit_status_idx) { /* เหมือนเดิม */ }
bool Player::isUnitStanding(int unit_status_idx) const { /* เหมือนเดิม */ return false; }
std::optional<Card> Player::getUnitAtStatusIndex(int unit_status_idx) const { /* เหมือนเดิม */ return std::nullopt; }

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
      // แสดงผลการ Boost ใน main.cpp ถ้าต้องการ
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

// --- Trigger Handling ---
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

  // การรับ input จาก main.cpp ผ่าน getIntegerInput จะดีกว่า
  // ส่วนนี้เป็นการจำลอง input ชั่วคราว
  int choice_idx = -1;
  std::cout << "เลือกหมายเลขยูนิต (-1 หากไม่ต้องการให้ผล): ";
  std::string input_str;
  std::cin >> input_str; // อาจจะต้องใช้ getIntegerInput จาก main.cpp ที่นี่
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  try
  {
    choice_idx = std::stoi(input_str);
  }
  catch (...)
  {
    choice_idx = -2;
  }

  if (choice_idx >= 0 && static_cast<size_t>(choice_idx) < available_units.size())
  {
    return available_units[static_cast<size_t>(choice_idx)].first;
  }
  std::cout << "การเลือกไม่ถูกต้อง หรือไม่เลือกยูนิตสำหรับ Trigger" << std::endl;
  return -1;
}

TriggerOutput Player::applyTriggerEffect(const Card &trigger_card, bool is_drive_check, Player *opponent_for_heal_check, int unit_for_power_crit_idx)
{
  TriggerOutput output_for_main_calc;
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
  // อัปเดต output_for_main_calc สำหรับ Power/Crit ที่จะให้ยูนิตเป้าหมายหลัก (Attacker/Defending VG)
  if (chosen_unit_status_idx == unit_for_power_crit_idx && chosen_unit_status_idx != -1)
  {
    if (trigger_card.getTypeRole() == "Trigger - Critical")
    {
      output_for_main_calc.power_to_chosen_unit = power_bonus_from_trigger;
      if (is_drive_check)
        output_for_main_calc.crit_to_chosen_unit = 1;
    }
    else if (trigger_card.getTypeRole() == "Trigger - Draw" || trigger_card.getTypeRole() == "Trigger - Heal")
    {
      output_for_main_calc.power_to_chosen_unit = power_bonus_from_trigger;
    }
    output_for_main_calc.chosen_unit_status_idx = chosen_unit_status_idx;
  }

  return output_for_main_calc;
}

TriggerOutput Player::performDriveCheck(int num_drives, Player *opponent_for_heal_check, int attacking_vg_idx)
{
  TriggerOutput combined_trigger_output_for_vg_attack;

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
        TriggerOutput effect = applyTriggerEffect(drive_card, true, opponent_for_heal_check, attacking_vg_idx);
        // Power/Crit ที่จะให้ attacking_vg_idx โดยตรงจะถูกคืนค่ามาใน effect
        combined_trigger_output_for_vg_attack.power_to_chosen_unit += effect.power_to_chosen_unit;
        combined_trigger_output_for_vg_attack.crit_to_chosen_unit += effect.crit_to_chosen_unit;
        if (effect.chosen_unit_status_idx == attacking_vg_idx)
        { // ตรวจสอบว่าผลนั้นสำหรับ attacking VG จริงๆ
          combined_trigger_output_for_vg_attack.chosen_unit_status_idx = attacking_vg_idx;
        }
      }
    }
  }
  return combined_trigger_output_for_vg_attack;
}

bool Player::healOneDamage() { /* เหมือนเดิม */ return false; }

int Player::addCardToGuardianZoneFromHand(size_t hand_card_index, bool &perfect_guarded_flag)
{
  perfect_guarded_flag = false; // ตั้งค่าเริ่มต้น
  if (hand_card_index >= hand.size())
  {
    return -1;
  }
  Card card_to_guard = hand[hand_card_index];

  if (card_to_guard.getCodeName() == "G1-02")
  { // "เล้ง" - Sentinel • Perfect Guard
    std::cout << "คุณใช้ '" << card_to_guard.getName() << "' (Perfect Guard)!" << std::endl;
    std::cout << "ต้องการใช้สกิล Perfect Guard หรือไม่? (ต้องทิ้งการ์ดอีก 1 ใบจากมือ):" << std::endl;
    char pg_choice = 'n';
    // การรับ input ควรจะมาจาก getActionInput ใน main.cpp
    std::cout << "ใช้สกิล Perfect Guard? (y/n): ";
    std::cin >> pg_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (pg_choice == 'y' || pg_choice == 'Y')
    {
      if (hand.size() > 1)
      { // ต้องมีอย่างน้อย 1 ใบให้ทิ้ง (ไม่นับตัว PG เองที่กำลังจะใช้)
        std::cout << "เลือกการ์ดที่จะทิ้งเพื่อใช้ Perfect Guard (ยกเว้นตัว PG ที่กำลังจะใช้):" << std::endl;

        std::vector<Card> temp_hand_for_discard_choice = hand;
        // เอาตัว PG ออกจาก list ชั่วคราวที่จะให้เลือกทิ้ง (ถ้ามันยังอยู่ใน hand)
        // (การ์ด PG ที่เลือกคือ hand[hand_card_index])
        // แต่ตอนนี้ hand_card_index อาจจะไม่ตรงกับตำแหน่งจริงใน temp_hand_for_discard_choice ถ้ามีการ์ดถูกลบไปแล้ว
        // เพื่อความง่าย จะให้ผู้เล่นเลือกจากมือปัจจุบัน แล้วเช็คว่าไม่ใช่ PG ตัวที่กำลังจะ Guard

        for (size_t i = 0; i < hand.size(); ++i)
        {
          if (i == hand_card_index)
            continue; // ข้ามตัว PG ที่กำลังจะใช้
          std::cout << i << ": " << hand[i] << std::endl;
        }

        int discard_idx = -1;
        std::cout << "ใส่หมายเลขการ์ดที่จะทิ้ง: ";
        std::cin >> discard_idx;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (discard_idx >= 0 && static_cast<size_t>(discard_idx) < hand.size() && static_cast<size_t>(discard_idx) != hand_card_index)
        {
          Card card_to_discard_for_pg = hand[static_cast<size_t>(discard_idx)];

          guardian_zone.push_back(card_to_guard); // PG เข้า Guardian Zone

          // ลบ PG และการ์ดที่ทิ้งออกจากมืออย่างระมัดระวังเรื่อง index
          size_t pg_actual_idx = hand_card_index;
          size_t discard_actual_idx = static_cast<size_t>(discard_idx);

          // ลบใบที่ index มากกว่าก่อน
          if (pg_actual_idx > discard_actual_idx)
          {
            hand.erase(hand.begin() + pg_actual_idx);
            hand.erase(hand.begin() + discard_actual_idx);
          }
          else
          { // discard_actual_idx >= pg_actual_idx (ถ้าเท่ากันคือเลือกตัว PG เอง ซึ่งไม่ควร)
            hand.erase(hand.begin() + discard_actual_idx);
            hand.erase(hand.begin() + pg_actual_idx);
          }
          drop_zone.push_back(card_to_discard_for_pg);

          std::cout << "ใช้ Perfect Guard! ทิ้ง '" << card_to_discard_for_pg.getName() << "' การโจมตีนี้จะถูกยกเลิก!" << std::endl;
          perfect_guarded_flag = true; // ตั้ง flag
          return 0;                    // PG ทำงาน, shield ไม่ต้องบวก แต่การโจมตีไม่เข้า
        }
        else
        {
          std::cout << "เลือกการ์ดทิ้งไม่ถูกต้อง. '" << card_to_guard.getName() << "' จะ Guard ด้วย Shield ปกติ (0)." << std::endl;
          // ไม่ใช้สกิล PG, ใช้ shield ปกติ
          guardian_zone.push_back(card_to_guard);
          hand.erase(hand.begin() + hand_card_index);
          return card_to_guard.getShield(); // ซึ่งคือ 0
        }
      }
      else
      {
        std::cout << "ไม่มีการ์ดพอจะทิ้งเพื่อใช้ Perfect Guard. '" << card_to_guard.getName() << "' จะ Guard ด้วย Shield ปกติ (0)." << std::endl;
        guardian_zone.push_back(card_to_guard);
        hand.erase(hand.begin() + hand_card_index);
        return card_to_guard.getShield(); // ซึ่งคือ 0
      }
    }
    else
    { // ไม่ใช้สกิล PG
      guardian_zone.push_back(card_to_guard);
      hand.erase(hand.begin() + hand_card_index);
      return card_to_guard.getShield(); // ซึ่งคือ 0
    }
  }
  // ถ้าไม่ใช่ PG
  guardian_zone.push_back(card_to_guard);
  int shield_value = card_to_guard.getShield();
  hand.erase(hand.begin() + hand_card_index);
  // std::cout << name << " ใช้ '" << card_to_guard.getName() << "' (Shield: " << shield_value << ") ในการ Guard." << std::endl; // main.cpp จะแสดง
  return shield_value;
}

int Player::getGuardianZoneShieldTotal() const { /* เหมือนเดิม */ return 0; }

int Player::performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt, Player *attacker, bool &out_perfect_guarded_flag)
{
  out_perfect_guarded_flag = false; // Reset flag
  // ClearScreen(); // main.cpp จะ Clear ให้
  // displayField(); // main.cpp จะแสดงให้
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
    if (out_perfect_guarded_flag)
      break; // ถ้า PG ทำงานแล้ว ไม่ต้อง Guard ต่อ
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
      int added_shield = addCardToGuardianZoneFromHand(static_cast<size_t>(card_idx), out_perfect_guarded_flag);
      if (out_perfect_guarded_flag)
      {
        final_total_shield = 999999;
        break;
      }
    }
    else
    {
      std::cout << "*** เลือกไม่ถูกต้อง ***" << std::endl;
    }
    final_total_shield = getGuardianZoneShieldTotal();

    if (hand.empty() || out_perfect_guarded_flag)
    {
      break;
    }

    std::cout << "ต้องการ Guard เพิ่มหรือไม่ (y/n): ";
    std::cin >> continue_guard_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  final_total_shield = getGuardianZoneShieldTotal();
  if (out_perfect_guarded_flag)
    final_total_shield = 999999; // Ensure PG overrides
  std::cout << "จบขั้นตอนการ Guard. Shield ที่ได้ทั้งหมดจาก Guardian Zone: " << (out_perfect_guarded_flag ? "PERFECT GUARD" : std::to_string(final_total_shield)) << std::endl;
  return final_total_shield;
}

void Player::displayGuardianZone() const { /* เหมือนเดิม */ }
// std::string formatCardForDisplay(const std::optional<Card>& card_opt, int width, bool is_standing, bool is_vc) { /* เหมือนเดิม */ } // ย้ายไป global หรือ main
// void Player::displayField(bool show_opponent_field_for_targeting) const { /* เหมือนเดิม */ } // ย้าย formatCardForDisplay
// void Player::displayHand(bool show_details) const { /* เหมือนเดิม */ } // ย้าย printDisplayLine
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

// แก้ไขการ implement ของ displayField และ displayHand ให้นิยาม formatCardForPlayerDisplay ภายใน Player.cpp
// หรือทำให้ formatCardForPlayerDisplay เป็น free function ที่ main.cpp ก็เห็น
// เพื่อความง่าย จะ copy มาใส่ใน Player.cpp ก่อน
std::string formatCardForPlayerDisplay_PlayerInternal(const std::optional<Card> &card_opt, int width, bool is_standing = true, bool is_vc = false)
{
  std::ostringstream oss;
  std::string content;

  if (card_opt.has_value())
  {
    const Card &card = card_opt.value();
    std::string grade_str = "G" + std::to_string(card.getGrade());
    std::string name_str = card.getName();
    std::string status_str = is_standing ? "" : " (R)";
    std::string crit_str = " C" + std::to_string(card.getCritical());
    std::string full_text = grade_str + crit_str + " " + name_str + status_str;
    if (full_text.length() > static_cast<size_t>(width))
    {
      int available_width_for_name = width - grade_str.length() - crit_str.length() - status_str.length() - 1 - 3;
      if (available_width_for_name < 1)
        available_width_for_name = 1;
      name_str = name_str.substr(0, static_cast<size_t>(available_width_for_name)) + "...";
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
  oss << std::string(static_cast<size_t>(pad_left), ' ') << content << std::string(static_cast<size_t>(pad_right), ' ');
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
  std::cout << "  " << V_BORDER << formatCardForPlayerDisplay_PlayerInternal(rear_guard_circles[RC_FRONT_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_FRONT_LEFT)])
            << V_BORDER << formatCardForPlayerDisplay_PlayerInternal(vanguard_circle, card_cell_width, unit_is_standing[UNIT_STATUS_VC_IDX], true)
            << V_BORDER << formatCardForPlayerDisplay_PlayerInternal(rear_guard_circles[RC_FRONT_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_FRONT_RIGHT)])
            << V_BORDER << std::endl;
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (0: RC FL)" : "   (RC FL)")
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << "   (VC)"
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (1: RC FR)" : "   (RC FR)")
            << V_BORDER << std::endl;
  std::cout << "  " << vc_row_separator_left << vc_row_separator_center << vc_row_separator_right << std::endl;
  std::cout << std::endl;
  std::cout << "  แถวหลัง:" << std::endl;
  std::cout << "  " << row_separator << std::endl;
  std::cout << "  " << V_BORDER << formatCardForPlayerDisplay_PlayerInternal(rear_guard_circles[RC_BACK_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_LEFT)])
            << V_BORDER << formatCardForPlayerDisplay_PlayerInternal(rear_guard_circles[RC_BACK_CENTER], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_CENTER)])
            << V_BORDER << formatCardForPlayerDisplay_PlayerInternal(rear_guard_circles[RC_BACK_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_RIGHT)])
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
