// Player.h - ไฟล์ Header สำหรับคลาส Player
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <array>
#include <optional>
#include <utility> // สำหรับ std::pair
#include "Deck.h"  // คลาส Deck
#include "Card.h"  // คลาส Card

// กำหนดค่าคงที่สำหรับตำแหน่ง Rear Guard Circles เพื่อให้อ่านง่าย
// Front Row: [RC_FL] [VC] [RC_FR]
// Back Row : [RC_BL] [RC_BC] [RC_BR]
constexpr size_t RC_FRONT_LEFT = 0;
constexpr size_t RC_FRONT_RIGHT = 1;
constexpr size_t RC_BACK_LEFT = 2;
constexpr size_t RC_BACK_CENTER = 3; // ตำแหน่งหลัง Vanguard โดยตรง
constexpr size_t RC_BACK_RIGHT = 4;
const int NUM_REAR_GUARD_CIRCLES = 5;

// Index สำหรับ unit_is_standing (0=VC, 1-5=RCs ตามลำดับ FL,FR,BL,BC,BR)
constexpr size_t UNIT_STATUS_VC_IDX = 0;
constexpr size_t UNIT_STATUS_RC_FL_IDX = RC_FRONT_LEFT + 1;
constexpr size_t UNIT_STATUS_RC_FR_IDX = RC_FRONT_RIGHT + 1;
constexpr size_t UNIT_STATUS_RC_BL_IDX = RC_BACK_LEFT + 1;
constexpr size_t UNIT_STATUS_RC_BC_IDX = RC_BACK_CENTER + 1;
constexpr size_t UNIT_STATUS_RC_BR_IDX = RC_BACK_RIGHT + 1;
const int NUM_FIELD_UNITS = 1 + NUM_REAR_GUARD_CIRCLES; // 1 VC + 5 RCs

class Player
{
private:
  std::string name;
  Deck deck; // ผู้เล่นเป็นเจ้าของ Deck นี้

  std::vector<Card> hand;
  std::optional<Card> vanguard_circle;
  std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> rear_guard_circles;
  std::array<bool, NUM_FIELD_UNITS> unit_is_standing; // สถานะ Stand/Rest ของยูนิตในสนาม

  std::vector<Card> damage_zone;
  std::vector<Card> soul;
  std::vector<Card> drop_zone;
  std::vector<Card> guardian_zone;

  // Helper function to get unit status index from RC slot index
  size_t getUnitStatusIndexForRC(size_t rc_slot_idx) const
  {
    return rc_slot_idx + 1; // +1 เพราะ index 0 คือ VC
  }
  // Static helper for printing lines, specific to Player display methods
  static void printDisplayLine(char c = '-', int length = 70);

public:
  // Constructor: ใช้ move semantics สำหรับ Deck
  Player(const std::string &player_name, Deck &&player_deck);

  // --- Setup Phase ---
  bool setupGame(const std::string &starter_code_name, int initial_hand_size = 5);

  // --- Turn Phases ---
  void performStandPhase();
  bool performDrawPhase();

  // --- Battle Phase Actions ---
  std::vector<std::pair<int, std::string>> chooseAttacker();
  // int chooseTarget(const Player& opponent); // จะ implement เมื่อมี opponent object
  int chooseBooster(int attacker_unit_status_idx);
  void restUnit(int unit_status_idx);
  bool isUnitStanding(int unit_status_idx) const;
  std::optional<Card> getUnitAtStatusIndex(int unit_status_idx) const;
  int getUnitPowerAtStatusIndex(int unit_status_idx, int booster_unit_status_idx = -1, bool for_defense = false) const; // เพิ่ม for_defense flag

  // *** เพิ่มเมธอดสำหรับ Guard ***
  // คืนค่า Shield รวมที่ได้จากการ Guard
  int performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt);

  // --- Basic Actions (เบื้องต้น) ---
  bool rideFromHand(size_t hand_card_index);
  bool callToRearGuard(size_t hand_card_index, size_t rc_slot_index);

  // --- Information Display ---
  void displayHand(bool show_details = false) const;
  void displayField(bool show_opponent_field_for_targeting = false) const;
  void displayFullStatus() const;
  void displayGuardianZone() const; // เพิ่มเมธอดแสดง Guardian Zone

  // --- Getters ---
  std::string getName() const;
  size_t getHandSize() const;
  const std::vector<Card> &getHand() const;
  size_t getDamageCount() const;
  const std::optional<Card> &getVanguard() const;
  const std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> &getRearGuards() const;

  // --- Game Mechanics ---
  void takeDamage(const Card &damage_card);
  void placeCardIntoSoul(const Card &card);
  void discardFromHandToDrop(size_t hand_card_index);
  void clearGuardianZoneAndMoveToDrop(); // เปลี่ยนชื่อให้ชัดเจน

private:
  void drawCards(int num_to_draw);
};

#endif // PLAYER_H
