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
constexpr size_t RC_FRONT_LEFT = 0;
constexpr size_t RC_FRONT_RIGHT = 1;
constexpr size_t RC_BACK_LEFT = 2;
constexpr size_t RC_BACK_CENTER = 3;
constexpr size_t RC_BACK_RIGHT = 4;
const int NUM_REAR_GUARD_CIRCLES = 5;

// Index สำหรับ unit_is_standing (0=VC, 1-5=RCs ตามลำดับ FL,FR,BL,BC,BR)
constexpr size_t UNIT_STATUS_VC_IDX = 0;
constexpr size_t UNIT_STATUS_RC_FL_IDX = RC_FRONT_LEFT + 1;
constexpr size_t UNIT_STATUS_RC_FR_IDX = RC_FRONT_RIGHT + 1;
constexpr size_t UNIT_STATUS_RC_BL_IDX = RC_BACK_LEFT + 1;
constexpr size_t UNIT_STATUS_RC_BC_IDX = RC_BACK_CENTER + 1;
constexpr size_t UNIT_STATUS_RC_BR_IDX = RC_BACK_RIGHT + 1;
const int NUM_FIELD_UNITS = 1 + NUM_REAR_GUARD_CIRCLES;

// Struct สำหรับเก็บผลลัพธ์จาก Trigger
struct TriggerOutput
{
  int extra_power = 0;
  int extra_crit = 0;
  bool card_drawn = false;
  bool damage_healed = false;
};

class Player
{
private:
  std::string name;
  Deck deck;

  std::vector<Card> hand;
  std::optional<Card> vanguard_circle;
  std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> rear_guard_circles;
  std::array<bool, NUM_FIELD_UNITS> unit_is_standing;

  std::vector<Card> damage_zone;
  std::vector<Card> soul;
  std::vector<Card> drop_zone;
  std::vector<Card> guardian_zone;

  static void printDisplayLine(char c = '-', int length = 70);
  void drawCards(int num_to_draw); // อยู่ใน private ถูกต้องแล้ว

public:
  Player(const std::string &player_name, Deck &&player_deck);

  size_t getUnitStatusIndexForRC(size_t rc_slot_idx) const
  {
    return rc_slot_idx + 1;
  }

  bool setupGame(const std::string &starter_code_name, int initial_hand_size = 5);
  void performStandPhase();
  bool performDrawPhase();

  std::vector<std::pair<int, std::string>> chooseAttacker();
  int chooseBooster(int attacker_unit_status_idx);
  void restUnit(int unit_status_idx);
  bool isUnitStanding(int unit_status_idx) const;
  std::optional<Card> getUnitAtStatusIndex(int unit_status_idx) const;
  int getUnitPowerAtStatusIndex(int unit_status_idx, int booster_unit_status_idx = -1, bool for_defense = false) const;

  TriggerOutput performDriveCheck(int num_drives, Player *opponent_for_heal_check);
  TriggerOutput handleDamageCheckTrigger(const Card &damage_card, Player *opponent_for_heal_check);
  int chooseUnitForTriggerEffect(const std::string &trigger_effect_description);
  bool healOneDamage();

  // --- Guarding Methods ---
  // เมธอดนี้จะถูกเรียกจาก main.cpp และจะ loop ภายในเพื่อให้ผู้เล่นเลือกการ์ด Guard
  int performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt);
  // เมธอด helper สำหรับเพิ่มการ์ด 1 ใบจากมือไป Guardian Zone และคืนค่า shield
  int addCardToGuardianZoneFromHand(size_t hand_card_index);
  int getGuardianZoneShieldTotal() const;

  bool rideFromHand(size_t hand_card_index);
  bool callToRearGuard(size_t hand_card_index, size_t rc_slot_index);

  void displayHand(bool show_details = false) const;
  void displayField(bool show_opponent_field_for_targeting = false) const;
  void displayFullStatus() const;
  void displayGuardianZone() const;

  std::string getName() const;
  size_t getHandSize() const;
  const std::vector<Card> &getHand() const;
  size_t getDamageCount() const;
  const std::optional<Card> &getVanguard() const;
  const std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> &getRearGuards() const;
  Deck &getDeck();
  const Deck &getDeck() const;

  void takeDamage(const Card &damage_card);
  void placeCardIntoSoul(const Card &card);
  void discardFromHandToDrop(size_t hand_card_index);
  void clearGuardianZoneAndMoveToDrop();
};

#endif // PLAYER_H
