// Player.h - ไฟล์ Header สำหรับคลาส Player (แก้ไขแล้ว)
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <array>
#include <optional>
#include <utility> // สำหรับใช้งาน std::pair
#include "Deck.h"  // สำหรับจัดการสำรับไพ่
#include "Card.h"  // สำหรับข้อมูลการ์ด

// ค่าคงที่สำหรับตำแหน่งวางการ์ดบนสนาม (Rear-guard Circles)
constexpr size_t RC_FRONT_LEFT = 0;   // ตำแหน่งซ้ายแถวหน้า
constexpr size_t RC_FRONT_RIGHT = 1;  // ตำแหน่งขวาแถวหน้า
constexpr size_t RC_BACK_LEFT = 2;    // ตำแหน่งซ้ายแถวหลัง
constexpr size_t RC_BACK_CENTER = 3;  // ตำแหน่งกลางแถวหลัง
constexpr size_t RC_BACK_RIGHT = 4;   // ตำแหน่งขวาแถวหลัง
const int NUM_REAR_GUARD_CIRCLES = 5; // จำนวนตำแหน่ง Rear-guard ทั้งหมด

// ดัชนีสำหรับสถานะของยูนิตในแต่ละตำแหน่ง
constexpr size_t UNIT_STATUS_VC_IDX = 0;                     // ดัชนีสำหรับ Vanguard
constexpr size_t UNIT_STATUS_RC_FL_IDX = RC_FRONT_LEFT + 1;  // ดัชนีสำหรับ Rear-guard ซ้ายแถวหน้า
constexpr size_t UNIT_STATUS_RC_FR_IDX = RC_FRONT_RIGHT + 1; // ดัชนีสำหรับ Rear-guard ขวาแถวหน้า
constexpr size_t UNIT_STATUS_RC_BL_IDX = RC_BACK_LEFT + 1;   // ดัชนีสำหรับ Rear-guard ซ้ายแถวหลัง
constexpr size_t UNIT_STATUS_RC_BC_IDX = RC_BACK_CENTER + 1; // ดัชนีสำหรับ Rear-guard กลางแถวหลัง
constexpr size_t UNIT_STATUS_RC_BR_IDX = RC_BACK_RIGHT + 1;  // ดัชนีสำหรับ Rear-guard ขวาแถวหลัง
const int NUM_FIELD_UNITS = 1 + NUM_REAR_GUARD_CIRCLES;      // จำนวนยูนิตทั้งหมดบนสนาม (Vanguard + Rear-guards)

// โครงสร้างสำหรับเก็บผลลัพธ์จากการเช็ค Trigger
struct TriggerOutput
{
  int extra_power = 0;        // พลังโจมตีเพิ่มเติม
  int extra_crit = 0;         // คริติคอลเพิ่มเติม
  bool card_drawn = false;    // เช็คว่าได้จั่วการ์ดหรือไม่
  bool damage_healed = false; // เช็คว่าได้ฮีลหรือไม่

  // ฟิลด์สำรองสำหรับการใช้งานในอนาคต
  int power_to_chosen_unit = 0;    // พลังโจมตีที่จะให้กับยูนิตที่เลือก
  int crit_to_chosen_unit = 0;     // คริติคอลที่จะให้กับยูนิตที่เลือก
  int chosen_unit_status_idx = -1; // ดัชนีของยูนิตที่ถูกเลือก
};

// คลาส Player - จัดการข้อมูลและการกระทำของผู้เล่น
class Player
{
private:
  // ข้อมูลพื้นฐานของผู้เล่น
  std::string name; // ชื่อผู้เล่น
  Deck deck;        // สำรับไพ่
  int turn_count;   // จำนวนรอบที่เล่น

  // การ์ดในส่วนต่างๆ
  std::vector<Card> hand;                                                     // การ์ดบนมือ
  std::optional<Card> vanguard_circle;                                        // การ์ดในตำแหน่ง Vanguard
  std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> rear_guard_circles; // การ์ดในตำแหน่ง Rear-guard
  std::array<bool, NUM_FIELD_UNITS> unit_is_standing;                         // สถานะการยืน/หมุนของยูนิต

  // โซนพิเศษต่างๆ
  std::vector<Card> damage_zone;   // โซนเก็บความเสียหาย
  std::vector<Card> soul;          // โซน Soul
  std::vector<Card> drop_zone;     // โซนทิ้งการ์ด
  std::vector<Card> guardian_zone; // โซนการ์ดป้องกัน

  // บัฟต่างๆ ในรอบการต่อสู้
  std::array<int, NUM_FIELD_UNITS> current_battle_power_buffs; // บัฟพลังโจมตี
  std::array<int, NUM_FIELD_UNITS> current_battle_crit_buffs;  // บัฟคริติคอล

  // ฟังก์ชันภายในสำหรับจั่วการ์ด
  void drawCards(int num_to_draw);

public:
  // Constructor
  Player(const std::string &player_name, Deck &&player_deck);

  // ฟังก์ชันช่วยแสดงเส้นคั่น
  static void printDisplayLine(char c = '-', int length = 70);

  // ฟังก์ชันแปลงดัชนี Rear-guard เป็นดัชนีสถานะ
  size_t getUnitStatusIndexForRC(size_t rc_slot_idx) const
  {
    return rc_slot_idx + 1;
  }

  // --- ฟังก์ชันเกี่ยวกับการเซ็ตอัพและเริ่มเกม ---
  bool setupGame(const std::string &starter_code_name, int initial_hand_size = 5);

  // --- ฟังก์ชันเกี่ยวกับเฟสต่างๆ ในเกม ---
  void performStandPhase(); // เฟสทำให้การ์ดกลับมายืน
  bool performDrawPhase();  // เฟสจั่วการ์ด

  // --- ฟังก์ชันเกี่ยวกับการต่อสู้ ---
  std::vector<std::pair<int, std::string>> chooseAttacker();
  int chooseBooster(int attacker_unit_status_idx);
  void restUnit(int unit_status_idx);
  bool isUnitStanding(int unit_status_idx) const;
  std::optional<Card> getUnitAtStatusIndex(int unit_status_idx) const;
  int getUnitPowerAtStatusIndex(int unit_status_idx, int booster_unit_status_idx = -1, bool for_defense = false) const;

  // --- ฟังก์ชันเกี่ยวกับการตรวจสอบ Trigger ---
  TriggerOutput performDriveCheck(int num_drives, Player *opponent_for_heal_check);
  TriggerOutput handleDamageCheckTrigger(const Card &damage_card, Player *opponent_for_heal_check);
  int chooseUnitForTriggerEffect(const std::string &trigger_effect_description);
  bool healOneDamage();

  // --- ฟังก์ชันเกี่ยวกับการป้องกันและ Intercept ---
  int addCardToGuardianZoneFromHand(size_t hand_card_index);
  int getGuardianZoneShieldTotal() const;
  int performGuardStep(int incoming_attack_power, const std::optional<Card> &target_unit_opt);

  // --- ฟังก์ชันเกี่ยวกับการวางการ์ดบนสนาม ---
  bool rideFromHand(size_t hand_card_index);
  bool callToRearGuard(size_t hand_card_index, size_t rc_slot_index);

  // --- ฟังก์ชันแสดงข้อมูล ---
  void displayHand(bool show_details = false) const;
  void displayField(bool show_opponent_field_for_targeting = false) const;
  void displayFullStatus() const;
  void displayGuardianZone() const;

  // --- ฟังก์ชันเข้าถึงข้อมูล (Getters) ---
  std::string getName() const;
  size_t getHandSize() const;
  const std::vector<Card> &getHand() const;
  size_t getDamageCount() const;
  const std::optional<Card> &getVanguard() const;
  const std::array<std::optional<Card>, NUM_REAR_GUARD_CIRCLES> &getRearGuards() const;
  Deck &getDeck();
  const Deck &getDeck() const;

  // --- ฟังก์ชันจัดการการ์ดและความเสียหาย ---
  void takeDamage(const Card &damage_card);
  void placeCardIntoSoul(const Card &card);
  void discardFromHandToDrop(size_t hand_card_index);
  void clearGuardianZoneAndMoveToDrop();

  void addCardToHand(const Card &card);
};

#endif // PLAYER_H