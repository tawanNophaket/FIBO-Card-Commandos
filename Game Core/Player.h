// Player.h - ไฟล์ Header สำหรับคลาส Player (แก้ไขแล้ว)
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <array>
#include <optional>
#include <utility> // สำหรับ std::pair
#include "Deck.h"
#include "Card.h"

constexpr size_t RC_FRONT_LEFT = 0;
constexpr size_t RC_FRONT_RIGHT = 1;
constexpr size_t RC_BACK_LEFT = 2;
constexpr size_t RC_BACK_CENTER = 3;
constexpr size_t RC_BACK_RIGHT = 4;
const int NUM_REAR_GUARD_CIRCLES = 5;

constexpr size_t UNIT_STATUS_VC_IDX = 0;
constexpr size_t UNIT_STATUS_RC_FL_IDX = RC_FRONT_LEFT + 1;
constexpr size_t UNIT_STATUS_RC_FR_IDX = RC_FRONT_RIGHT + 1;
constexpr size_t UNIT_STATUS_RC_BL_IDX = RC_BACK_LEFT + 1;
constexpr size_t UNIT_STATUS_RC_BC_IDX = RC_BACK_CENTER + 1;
constexpr size_t UNIT_STATUS_RC_BR_IDX = RC_BACK_RIGHT + 1;
const int NUM_FIELD_UNITS = 1 + NUM_REAR_GUARD_CIRCLES;

// แก้ไข TriggerOutput struct ให้ตรงกับการใช้งานใน Player_Improved.cpp
struct TriggerOutput
{
  int extra_power = 0;
  int extra_crit = 0;
  bool card_drawn = false;
  bool damage_healed = false;

  // เก็บ legacy fields ไว้เผื่อใช้ในอนาคต
  int power_to_chosen_unit = 0;
  int crit_to_chosen_unit = 0;
  int chosen_unit_status_idx = -1;
};

class Player
{
private:
  std::string name;
  Deck deck;
  int turn_count;

  std::vector<Card> hand;
  std::optional<Card> vanguard_circle;
  std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> rear_guard_circles;
  std::array<bool, NUM_FIELD_UNITS> unit_is_standing;

  std::vector<Card> damage_zone;
  std::vector<Card> soul;
  std::vector<Card> drop_zone;
  std::vector<Card> guardian_zone;

  std::array<int, NUM_FIELD_UNITS> current_battle_power_buffs;
  std::array<int, NUM_FIELD_UNITS> current_battle_crit_buffs;

  void drawCards(int num_to_draw);

public:
  Player(const std::string &player_name, Deck &&player_deck);

  // Helper for printing lines (ควรจะเป็น global หรือ utility)
  static void printDisplayLine(char c = '-', int length = 70);

  size_t getUnitStatusIndexForRC(size_t rc_slot_idx) const
  {
    return rc_slot_idx + 1;
  }

  // --- Setup & Mulligan ---
  bool setupGame(const std::string &starter_code_name, int initial_hand_size = 5);

  // --- Turn Phases ---
  void performStandPhase();
  bool performDrawPhase();

  // --- Battle Phase Actions ---
  std::vector<std::pair<int, std::string>> chooseAttacker();
  int chooseBooster(int attacker_unit_status_idx);
  void restUnit(int unit_status_idx);
  bool isUnitStanding(int unit_status_idx) const;
  std::optional<Card> getUnitAtStatusIndex(int unit_status_idx) const;
  int getUnitPowerAtStatusIndex(int unit_status_idx, int booster_unit_status_idx = -1, bool for_defense = false) const;

  // แก้ไขฟังก์ชัน Trigger ให้ตรงกับการใช้งาน
  TriggerOutput performDriveCheck(int num_drives, Player *opponent_for_heal_check);
  TriggerOutput handleDamageCheckTrigger(const Card &damage_card, Player *opponent_for_heal_check);
  int chooseUnitForTriggerEffect(const std::string &trigger_effect_description);
  bool healOneDamage();

  // --- Guarding & Intercept ---
  int addCardToGuardianZoneFromHand(size_t hand_card_index);
  int getGuardianZoneShieldTotal() const;
  int performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt);

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

  void addCardToHand(const Card &card);
};

#endif // PLAYER_H