// MenuSystem.h - ไฟล์ Header สำหรับระบบเมนู
#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <string>
#include <vector>
#include <functional>
#include "../Game Core/Player.h"

// Menu Option Structure
struct MenuOption
{
  std::string key;          // ปุ่มที่กด เช่น "1", "r", "h"
  std::string display_text; // ข้อความที่แสดง
  std::string icon;         // ไอคอน
  std::string description;  // คำอธิบายเพิ่มเติม
  bool enabled;             // เปิดใช้งานหรือไม่

  MenuOption(const std::string &k, const std::string &text, const std::string &ic = "",
             const std::string &desc = "", bool en = true)
      : key(k), display_text(text), icon(ic), description(desc), enabled(en) {}
};

// Menu Result
struct MenuResult
{
  std::string selected_key;
  bool is_valid;
  bool should_exit;

  MenuResult(const std::string &key = "", bool valid = false, bool exit = false)
      : selected_key(key), is_valid(valid), should_exit(exit) {}
};

class MenuSystem
{
private:
  static std::string GetUserInput(const std::string &prompt, bool clear_screen = true,
                                  Player *player_for_display = nullptr);
  static void DisplayMenuHeader(const std::string &title, const std::string &subtitle = "");
  static void DisplayMenuFooter();

public:
  // Core Menu Functions
  static MenuResult ShowMenu(const std::string &title, const std::vector<MenuOption> &options,
                             const std::string &subtitle = "", Player *player_context = nullptr);

  // Game-Specific Menus
  static MenuResult ShowMainPhaseMenu(Player *current_player);
  static MenuResult ShowBattlePhaseMenu(Player *current_player);
  static MenuResult ShowRideMenu(Player *current_player);
  static MenuResult ShowCallMenu(Player *current_player);
  static MenuResult ShowAttackerMenu(Player *current_player);
  static MenuResult ShowGuardMenu(Player *current_player, int incoming_power);

  // Input Menus
  static int GetCardSelectionFromHand(Player *player, const std::string &action_name,
                                      bool allow_cancel = true);
  static int GetRCPositionSelection(const std::string &action_name = "Call");
  static int GetTargetSelection(Player *attacker, Player *defender);
  static int GetBoosterSelection(Player *current_player, int attacker_status_idx);

  // Confirmation Menus
  static bool ShowConfirmation(const std::string &message, const std::string &details = "");
  static bool ShowYesNoPrompt(const std::string &question, bool default_yes = false);

  // Information Display
  static void ShowCardDetails(const Card &card);
  static void ShowFieldOverview(Player *player1, Player *player2, Player *current_player);
  static void ShowGameHelp(const std::string &current_phase = "");
  static void ShowPhaseHelp(const std::string &phase_name);

  // Game State Menus
  static MenuResult ShowGameStartMenu();
  static void ShowGameOverScreen(Player *winner, Player *loser, const std::string &reason);
  static void ShowTurnStartScreen(Player *current_player, int turn_number);

  // Utility Functions
  static void WaitForKeyPress(const std::string &message = "");
  static std::string FormatMenuOption(const MenuOption &option, int index = -1, bool numbered = true);
  static void ShowLoadingScreen(const std::string &message, int duration_ms = 1000);

  // Advanced Input
  static std::string GetPlayerName(const std::string &prompt);
  static int GetIntegerInput(const std::string &prompt, int min_val, int max_val,
                             Player *player_context = nullptr);
  static char GetCharInput(const std::string &prompt, const std::string &valid_chars,
                           Player *player_context = nullptr);
};

#endif // MENUSYSTEM_H