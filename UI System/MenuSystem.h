// MenuSystem.h - ไฟล์ Header สำหรับระบบเมนูของเกม
// ระบบนี้จัดการการแสดงผลและการโต้ตอบกับผู้เล่นทั้งหมด รวมถึงเมนูหลัก เมนูการเล่น และการแสดงสถานะเกม
#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <string>
#include <vector>
#include <functional>
#include "../Game Core/Player.h"

// โครงสร้างตัวเลือกเมนู (Menu Option Structure)
struct MenuOption
{
  std::string key;          // คีย์สำหรับการเลือก (เช่น "1", "r", "h")
  std::string display_text; // ข้อความที่แสดงในเมนู
  std::string icon;         // ไอคอนประกอบรายการเมนู
  std::string description;  // คำอธิบายเพิ่มเติมของตัวเลือก
  bool enabled;             // สถานะการเปิด/ปิดใช้งานตัวเลือก

  // คอนสตรัคเตอร์สำหรับสร้างตัวเลือกเมนูใหม่
  MenuOption(const std::string &k, const std::string &text, const std::string &ic = "",
             const std::string &desc = "", bool en = true)
      : key(k), display_text(text), icon(ic), description(desc), enabled(en) {}
};

// โครงสร้างผลลัพธ์จากการเลือกเมนู (Menu Result)
struct MenuResult
{
  std::string selected_key; // คีย์ที่ผู้เล่นเลือก
  bool is_valid;            // ตรวจสอบว่าการเลือกถูกต้องหรือไม่
  bool should_exit;         // ตรวจสอบว่าควรออกจากเมนูหรือไม่

  // คอนสตรัคเตอร์สำหรับผลลัพธ์เมนู
  MenuResult(const std::string &key = "", bool valid = false, bool exit = false)
      : selected_key(key), is_valid(valid), should_exit(exit) {}
};

// คลาสจัดการระบบเมนู (Menu System Class)
class MenuSystem
{
private:
  // ฟังก์ชันช่วยเหลือภายใน
  static std::string GetUserInput(const std::string &prompt, bool clear_screen = true,
                                  Player *player_for_display = nullptr);                     // รับข้อมูลจากผู้ใช้
  static void DisplayMenuHeader(const std::string &title, const std::string &subtitle = ""); // แสดงส่วนหัวของเมนู
  static void DisplayMenuFooter();                                                           // แสดงส่วนท้ายของเมนู

public:
  // ฟังก์ชันหลักของเมนู (Core Menu Functions)
  static MenuResult ShowMenu(const std::string &title, const std::vector<MenuOption> &options,
                             const std::string &subtitle = "", Player *player_context = nullptr); // แสดงเมนูทั่วไป

  // เมนูเฉพาะของเกม (Game-Specific Menus)
  static MenuResult ShowMainPhaseMenu(Player *current_player);                 // เมนูสำหรับ Main Phase
  static MenuResult ShowBattlePhaseMenu(Player *current_player);               // เมนูสำหรับ Battle Phase
  static MenuResult ShowRideMenu(Player *current_player);                      // เมนูสำหรับ Ride
  static MenuResult ShowCallMenu(Player *current_player);                      // เมนูสำหรับ Call
  static MenuResult ShowAttackerMenu(Player *current_player);                  // เมนูสำหรับ Attacker
  static MenuResult ShowGuardMenu(Player *current_player, int incoming_power); // เมนูสำหรับ Guard

  // เมนูสำหรับการเลือกข้อมูล (Input Menus)
  static int GetCardSelectionFromHand(Player *player, const std::string &action_name,
                                      bool allow_cancel = true);                   // เลือกการ์ดจากมือ
  static int GetRCPositionSelection(const std::string &action_name = "Call");      // เลือกตำแหน่ง RC
  static int GetTargetSelection(Player *attacker, Player *defender);               // เลือกเป้าหมาย
  static int GetBoosterSelection(Player *current_player, int attacker_status_idx); // เลือก Booster

  // เมนูสำหรับการยืนยัน (Confirmation Menus)
  static bool ShowConfirmation(const std::string &message, const std::string &details = ""); // แสดงข้อความยืนยัน
  static bool ShowYesNoPrompt(const std::string &question, bool default_yes = false);        // แสดงคำถาม Yes/No

  // การแสดงข้อมูล (Information Display)
  static void ShowCardDetails(const Card &card);                                           // แสดงรายละเอียดการ์ด
  static void ShowFieldOverview(Player *player1, Player *player2, Player *current_player); // แสดงภาพรวมของสนาม
  static void ShowGameHelp(const std::string &current_phase = "");                         // แสดงความช่วยเหลือของเกม
  static void ShowPhaseHelp(const std::string &phase_name);                                // แสดงความช่วยเหลือของ Phase

  // เมนูสถานะของเกม (Game State Menus)
  static MenuResult ShowGameStartMenu();                                                    // เมนูเริ่มเกม
  static void ShowGameOverScreen(Player *winner, Player *loser, const std::string &reason); // แสดงหน้าจอจบเกม
  static void ShowTurnStartScreen(Player *current_player, int turn_number);                 // แสดงหน้าจอเริ่มเทิร์น

  // ฟังก์ชันอรรถประโยชน์ (Utility Functions)
  static void WaitForKeyPress(const std::string &message = "");                                        // รอการกดปุ่มจากผู้ใช้
  static std::string FormatMenuOption(const MenuOption &option, int index = -1, bool numbered = true); // จัดรูปแบบตัวเลือกเมนู
  static void ShowLoadingScreen(const std::string &message, int duration_ms = 1000);                   // แสดงหน้าจอโหลด

  // การรับข้อมูลขั้นสูง (Advanced Input)
  static std::string GetPlayerName(const std::string &prompt); // รับชื่อผู้เล่น
  static int GetIntegerInput(const std::string &prompt, int min_val, int max_val,
                             Player *player_context = nullptr); // รับค่าตัวเลข
  static char GetCharInput(const std::string &prompt, const std::string &valid_chars,
                           Player *player_context = nullptr); // รับตัวอักษร
};

#endif // MENUSYSTEM_H