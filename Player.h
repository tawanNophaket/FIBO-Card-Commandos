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
  int power_to_chosen_unit = 0;    // Power ที่จะให้ยูนิตที่ผู้เล่นเลือก (อาจจะเป็น Attacker หรือ Vanguard)
  int crit_to_chosen_unit = 0;     // Critical ที่จะให้ยูนิตที่ผู้เล่นเลือก (สำหรับ Drive Check)
  int chosen_unit_status_idx = -1; // Index ของยูนิตที่รับผล Power/Crit
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

  // Buff ชั่วคราวสำหรับ Battle ปัจจุบันของแต่ละยูนิตในสนาม
  std::array<int, NUM_FIELD_UNITS> current_battle_power_buffs;
  std::array<int, NUM_FIELD_UNITS> current_battle_crit_buffs;

  static void printDisplayLine(char c = '-', int length = 70);
  void drawCards(int num_to_draw);

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
  int getUnitCriticalAtStatusIndex(int unit_status_idx) const;

  // --- Trigger Handling ---
  // unit_for_power_crit_idx: คือ unit status index ของยูนิตที่ควรจะได้รับ Power/Crit (เช่น Attacking VG, หรือ Defending VG)
  TriggerOutput applyTriggerEffect(const Card &trigger_card, bool is_drive_check, Player *opponent_for_heal_check, int unit_for_power_crit_idx);
  TriggerOutput performDriveCheck(int num_drives, Player *opponent_for_heal_check, int attacking_vg_idx);
  int chooseUnitForTriggerEffect(const std::string &trigger_effect_description, bool can_choose_any_unit = true);
  bool healOneDamage();
  void resetBattleBuffs();

  int addCardToGuardianZoneFromHand(size_t hand_card_index);
  int getGuardianZoneShieldTotal() const;
  // performGuardStep จะคืนค่า Shield รวมที่ได้จากการ Guard และ Intercept ทั้งหมด
  int performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt, Player *attacker); // เพิ่ม attacker เข้ามาเผื่อต้องใช้

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
