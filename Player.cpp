// Player.cpp - ไฟล์ Source สำหรับ υλολοποίηση คลาส Player
#include "Player.h"
#include <iostream>
#include <algorithm> // For std::remove, std::sort (if needed later)
#include <iomanip>   // For std::setw, std::left
#include <sstream>   // For std::ostringstream
#include <limits>    // For std::numeric_limits

// Static helper function definition for Player class (or make it a free function if preferred)
void Player::printDisplayLine(char c, int length)
{
  std::cout << std::string(length, c) << std::endl;
}

// Constructor
Player::Player(const std::string &player_name, Deck &&player_deck)
    : name(player_name), deck(std::move(player_deck)) // Use member initializer list
{
  unit_is_standing.fill(true); // Initialize all units to standing
}

// Helper method to draw cards
void Player::drawCards(int num_to_draw)
{
  for (int i = 0; i < num_to_draw; ++i)
  {
    std::optional<Card> drawn_card = deck.draw(); // Call deck.draw() directly
    if (drawn_card.has_value())
    {
      hand.push_back(drawn_card.value());
    }
    else
    {
      // std::cout << name << " ไม่สามารถจั่วการ์ดได้อีกต่อไป (เด็คหมด)" << std::endl;
      break; // Stop drawing if deck is empty
    }
  }
}

// Setup Game
bool Player::setupGame(const std::string &starter_code_name, int initial_hand_size)
{
  std::optional<Card> starter_card_opt = deck.removeCardByCodeName(starter_code_name);
  if (!starter_card_opt.has_value())
  {
    // std::cerr << "Error: ไม่พบ Starter Card '" << starter_code_name << "' ในเด็คของ " << name << std::endl;
    return false;
  }
  vanguard_circle = starter_card_opt.value();
  unit_is_standing[UNIT_STATUS_VC_IDX] = true; // Starter is standing

  deck.shuffle();
  drawCards(initial_hand_size);
  return true;
}

// --- Turn Phases ---
void Player::performStandPhase()
{
  // Stand all units
  unit_is_standing.fill(true);
}

bool Player::performDrawPhase()
{
  if (deck.isEmpty())
  {               // Call deck.isEmpty() directly
    return false; // Cannot draw if deck is empty
  }
  drawCards(1);
  return true;
}

// --- Battle Phase Actions ---
std::vector<std::pair<int, std::string>> Player::chooseAttacker()
{
  std::vector<std::pair<int, std::string>> available_attackers;
  // Check Vanguard
  if (vanguard_circle.has_value() && unit_is_standing[UNIT_STATUS_VC_IDX])
  {
    available_attackers.push_back({UNIT_STATUS_VC_IDX, "VC: " + vanguard_circle.value().getName()});
  }
  // Check Front Row Rear Guards
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
    return -1; // Should not happen if attacker_unit_status_idx is valid

  int potential_booster_status_idx = -1;
  size_t booster_rc_idx = static_cast<size_t>(-1); // Initialize to an invalid index

  // Determine the RC slot behind the attacker
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
  { // If a valid booster position exists
    potential_booster_status_idx = getUnitStatusIndexForRC(booster_rc_idx);
    if (rear_guard_circles[booster_rc_idx].has_value() &&
        unit_is_standing[potential_booster_status_idx] &&
        (rear_guard_circles[booster_rc_idx].value().getGrade() == 0 || rear_guard_circles[booster_rc_idx].value().getGrade() == 1))
    { // Check if booster exists, is standing, and is G0 or G1
      // std::cout << "คุณสามารถ Boost Attacker (" << attacker_card.value().getName() << ") ด้วย: "
      //           << rear_guard_circles[booster_rc_idx].value().getName()
      //           << " (G" << rear_guard_circles[booster_rc_idx].value().getGrade() << ") ได้" << std::endl;
      return potential_booster_status_idx;
    }
  }
  return -1; // No valid booster found
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
  else if (unit_status_idx > 0 && static_cast<size_t>(unit_status_idx - 1) < NUM_REAR_GUARD_CIRCLES) // unit_status_idx is 1-based for RCs
  {
    return rear_guard_circles[static_cast<size_t>(unit_status_idx - 1)];
  }
  return std::nullopt; // Invalid index
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
      total_power += booster_opt.value().getPower(); // Boosters add their power
    }
  }
  // Add other power modifications here (e.g., skills)
  return total_power;
}

// --- Basic Actions ---
bool Player::rideFromHand(size_t hand_card_index)
{
  if (hand_card_index >= hand.size())
  {
    // std::cout << "Error: Invalid card index for ride." << std::endl;
    return false;
  }

  Card card_to_ride = hand[hand_card_index];
  int current_vg_grade = vanguard_circle.has_value() ? vanguard_circle.value().getGrade() : -1; // Handle no VG case

  // Ride condition: Grade must be equal to or one greater than current VG's grade
  // Or if no VG, must be Grade 0
  bool can_ride = false;
  if (!vanguard_circle.has_value())
  { // Initial ride (no current VG)
    if (card_to_ride.getGrade() == 0)
      can_ride = true;
  }
  else
  { // Normal ride
    if (card_to_ride.getGrade() == current_vg_grade || card_to_ride.getGrade() == current_vg_grade + 1)
    {
      can_ride = true;
    }
  }

  if (!can_ride)
  {
    // std::cout << "Error: Cannot ride " << card_to_ride.getName() << " (G" << card_to_ride.getGrade()
    //           << ") on current Vanguard (G" << current_vg_grade << ")." << std::endl;
    return false;
  }

  // Move current VG to soul (if one exists)
  if (vanguard_circle.has_value())
  {
    soul.push_back(vanguard_circle.value());
  }

  vanguard_circle = card_to_ride;              // Place new VG
  unit_is_standing[UNIT_STATUS_VC_IDX] = true; // New VG is standing
  hand.erase(hand.begin() + hand_card_index);  // Remove card from hand

  // std::cout << name << " rode " << card_to_ride.getName() << "!" << std::endl;
  return true;
}

bool Player::callToRearGuard(size_t hand_card_index, size_t rc_slot_index)
{
  if (hand_card_index >= hand.size())
  {
    // std::cout << "Error: Invalid card index for call." << std::endl;
    return false;
  }
  if (rc_slot_index >= NUM_REAR_GUARD_CIRCLES)
  {
    // std::cout << "Error: Invalid RC slot index." << std::endl;
    return false;
  }
  if (rear_guard_circles[rc_slot_index].has_value())
  {
    // std::cout << "Error: RC slot " << rc_slot_index << " is already occupied." << std::endl;
    return false; // Slot is occupied
  }

  Card card_to_call = hand[hand_card_index];

  // Call condition: Grade must be less than or equal to current VG's grade
  if (!vanguard_circle.has_value())
  { // Should not happen if game setup correctly
    // std::cout << "Error: No Vanguard to determine valid call grade." << std::endl;
    return false;
  }
  if (card_to_call.getGrade() > vanguard_circle.value().getGrade())
  {
    // std::cout << "Error: Cannot call " << card_to_call.getName() << " (G" << card_to_call.getGrade()
    //           << ") - grade is higher than Vanguard (G" << vanguard_circle.value().getGrade() << ")." << std::endl;
    return false;
  }

  rear_guard_circles[rc_slot_index] = card_to_call;
  unit_is_standing[getUnitStatusIndexForRC(rc_slot_index)] = true; // Called unit is standing
  hand.erase(hand.begin() + hand_card_index);

  // std::cout << name << " called " << card_to_call.getName() << " to RC slot " << rc_slot_index << "." << std::endl;
  return true;
}

// --- Information Display ---

// Helper to format card display string (moved from main.cpp for Player class to use)
std::string formatCardForPlayerDisplay(const std::optional<Card> &card_opt, int width, bool is_standing = true, bool is_vc = false)
{
  std::ostringstream oss;
  std::string content;

  if (card_opt.has_value())
  {
    const Card &card = card_opt.value();
    std::string grade_str = "G" + std::to_string(card.getGrade());
    std::string name_str = card.getName();
    std::string status_str = is_standing ? "" : " (R)"; // (R) for Rest
    std::string full_text = grade_str + " " + name_str + status_str;

    // Truncate if too long
    if (full_text.length() > static_cast<size_t>(width))
    {
      int available_width_for_name = width - grade_str.length() - status_str.length() - 1 - 3; // -1 for space, -3 for "..."
      if (available_width_for_name < 1)
        available_width_for_name = 1;
      name_str = name_str.substr(0, static_cast<size_t>(available_width_for_name)) + "...";
      content = grade_str + " " + name_str + status_str;
    }
    else
    {
      content = full_text;
    }
  }
  else
  {
    content = "[   ว่าง   ]"; // Empty slot
  }

  // Centering
  int text_len = 0;
  for (char c : content)
  { // Basic length, may not be accurate for all UTF-8
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
  const int card_cell_width = 22; // Width for each card cell in display
  const std::string V_BORDER = "|";
  const std::string H_BORDER_THIN_SEGMENT = std::string(static_cast<size_t>(card_cell_width), '-');
  const std::string H_BORDER_THICK_SEGMENT = std::string(static_cast<size_t>(card_cell_width), '=');
  const std::string CORNER = "+";

  // Construct row separators
  std::string row_separator = CORNER;
  for (int i = 0; i < 3; ++i)
  { // 3 cells per row
    row_separator += H_BORDER_THIN_SEGMENT + CORNER;
  }
  std::string vc_row_separator_left = CORNER + H_BORDER_THIN_SEGMENT + CORNER;
  std::string vc_row_separator_center = H_BORDER_THICK_SEGMENT; // Thicker for VC
  std::string vc_row_separator_right = CORNER + H_BORDER_THIN_SEGMENT + CORNER;

  std::cout << "\n<<<<< สนามของ: " << name << " >>>>>" << std::endl;
  std::cout << "มือ: " << std::left << std::setw(2) << hand.size()
            << " | เด็ค: " << std::left << std::setw(2) << deck.getSize()
            << " | ดาเมจ: " << std::left << std::setw(1) << damage_zone.size()
            << " | โซล: " << std::left << std::setw(2) << soul.size()
            << " | ดรอป: " << std::left << std::setw(2) << drop_zone.size() << std::endl;
  Player::printDisplayLine('~', 70);

  // Front Row
  std::cout << "  แถวหน้า:" << std::endl;
  std::cout << "  " << vc_row_separator_left << vc_row_separator_center << vc_row_separator_right << std::endl;
  std::cout << "  " << V_BORDER << formatCardForPlayerDisplay(rear_guard_circles[RC_FRONT_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_FRONT_LEFT)])
            << V_BORDER << formatCardForPlayerDisplay(vanguard_circle, card_cell_width, unit_is_standing[UNIT_STATUS_VC_IDX], true) // is_vc = true
            << V_BORDER << formatCardForPlayerDisplay(rear_guard_circles[RC_FRONT_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_FRONT_RIGHT)])
            << V_BORDER << std::endl;
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (0: RC FL)" : "   (RC FL)")
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (VC)" : "   (VC)") // Opponent VC is not targetable with number for now
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (1: RC FR)" : "   (RC FR)")
            << V_BORDER << std::endl;
  std::cout << "  " << vc_row_separator_left << vc_row_separator_center << vc_row_separator_right << std::endl;
  std::cout << std::endl;

  // Back Row
  std::cout << "  แถวหลัง:" << std::endl;
  std::cout << "  " << row_separator << std::endl;
  std::cout << "  " << V_BORDER << formatCardForPlayerDisplay(rear_guard_circles[RC_BACK_LEFT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_LEFT)])
            << V_BORDER << formatCardForPlayerDisplay(rear_guard_circles[RC_BACK_CENTER], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_CENTER)])
            << V_BORDER << formatCardForPlayerDisplay(rear_guard_circles[RC_BACK_RIGHT], card_cell_width, unit_is_standing[getUnitStatusIndexForRC(RC_BACK_RIGHT)])
            << V_BORDER << std::endl;
  std::cout << "  " << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (2: RC BL)" : "   (RC BL)")
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (3: RC BC)" : "   (RC BC)")
            << V_BORDER << std::left << std::setw(card_cell_width) << std::setfill(' ') << (show_opponent_field_for_targeting ? "   (4: RC BR)" : "   (RC BR)")
            << V_BORDER << std::endl;
  std::cout << "  " << row_separator << std::endl;

  // Damage Zone
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
      std::cout << i << ": " << hand[i]; // Uses Card's operator<<
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
  displayHand(true); // Show hand details
                     // Potentially display soul, drop zone too if needed for debugging/full status
}

// --- Getters ---
std::string Player::getName() const { return name; }
size_t Player::getHandSize() const { return hand.size(); }
const std::vector<Card> &Player::getHand() const { return hand; }
size_t Player::getDamageCount() const { return damage_zone.size(); }
const std::optional<Card> &Player::getVanguard() const { return vanguard_circle; }
const std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> &Player::getRearGuards() const { return rear_guard_circles; }
Deck &Player::getDeck() { return deck; }             // Non-const version
const Deck &Player::getDeck() const { return deck; } // Const version

// --- Game Mechanics ---
void Player::takeDamage(const Card &damage_card)
{
  std::cout << "🩸 " << name << " ได้รับ 1 ดาเมจ! การ์ดที่ตก Damage Zone: " << damage_card.getName() << std::endl;
  damage_zone.push_back(damage_card);
  if (getDamageCount() >= 6)
  {
    std::cout << "*** 💀 " << name << " ได้รับ 6 ดาเมจแล้ว! " << name << " แพ้แล้ว! 💀 ***" << std::endl;
    // Game over logic will be handled in main.cpp
  }
}

void Player::placeCardIntoSoul(const Card &card)
{
  soul.push_back(card);
  // std::cout << "'" << card.getName() << "' ถูกวางลงใน Soul ของ " << name << "." << std::endl;
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
    // std::cout << "การ์ดใน Guardian Zone (" << name << "): ";
    for (const auto &card : guardian_zone)
    {
      // std::cout << "[" << card.getName() << "] ";
      drop_zone.push_back(card);
    }
    // std::cout << "ถูกย้ายไป Drop Zone." << std::endl;
    guardian_zone.clear();
  }
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
  std::cout << std::endl;
}

int Player::addCardToGuardianZoneFromHand(size_t hand_card_index)
{
  if (hand_card_index >= hand.size())
  {
    return -1; // Invalid index
  }
  Card card_to_guard = hand[hand_card_index];
  guardian_zone.push_back(card_to_guard);
  hand.erase(hand.begin() + static_cast<long>(hand_card_index)); // Ensure correct type for erase
  // std::cout << name << " ใช้ '" << card_to_guard.getName() << "' (Shield: " << card_to_guard.getShield() << ") ในการ Guard." << std::endl;
  return card_to_guard.getShield();
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
