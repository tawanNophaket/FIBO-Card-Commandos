// MenuSystem.cpp - Implementation ของระบบเมนู
#include "MenuSystem.h"
#include "UIHelper.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <sstream>

using namespace std;

// Private Helper Functions
string MenuSystem::GetUserInput(const string &prompt, bool clear_screen, Player *player_for_display)
{
  if (clear_screen)
  {
    UIHelper::ClearScreen();
  }

  if (player_for_display)
  {
    UIHelper::PrintQuickStatus(player_for_display->getName(),
                               player_for_display->getHandSize(),
                               player_for_display->getDeck().getSize(),
                               player_for_display->getDamageCount(),
                               0, 0); // TODO: Add soul and drop count getters
    player_for_display->displayField();
  }

  cout << "\n"
       << Colors::BRIGHT_CYAN << prompt << Colors::RESET;
  string input;
  getline(cin, input);
  return input;
}

void MenuSystem::DisplayMenuHeader(const string &title, const string &subtitle)
{
  UIHelper::PrintSectionHeader(title, Icons::MENU, Colors::BRIGHT_CYAN);
  if (!subtitle.empty())
  {
    cout << Colors::BRIGHT_BLACK << "   " << subtitle << Colors::RESET << "\n";
  }
}

void MenuSystem::DisplayMenuFooter()
{
  cout << "\n";
  UIHelper::PrintHorizontalLine("─", 70, Colors::BRIGHT_BLACK);
  UIHelper::PrintShortcuts();
}

// Core Menu Functions
MenuResult MenuSystem::ShowMenu(const string &title, const vector<MenuOption> &options,
                                const string &subtitle, Player *player_context)
{
  UIHelper::ClearScreen();

  if (player_context)
  {
    UIHelper::PrintQuickStatus(player_context->getName(),
                               player_context->getHandSize(),
                               player_context->getDeck().getSize(),
                               player_context->getDamageCount(),
                               0, 0);
  }

  DisplayMenuHeader(title, subtitle);

  // Display options
  for (size_t i = 0; i < options.size(); i++)
  {
    const auto &option = options[i];
    string color = option.enabled ? Colors::WHITE : Colors::BRIGHT_BLACK;
    string status_icon = option.enabled ? " " : Colors::BRIGHT_BLACK + " (ปิดใช้งาน)" + Colors::RESET;

    cout << color << "[" << option.key << "] " << option.icon << " "
         << option.display_text << status_icon << Colors::RESET;

    if (!option.description.empty())
    {
      cout << Colors::BRIGHT_BLACK << " - " << option.description << Colors::RESET;
    }
    cout << "\n";
  }

  DisplayMenuFooter();

  // Get input
  string input = GetUserInput("เลือก: ", false);

  // Handle special inputs first
  if (input == "h" || input == "H")
  {
    ShowGameHelp();
    return MenuResult("h", true, false);
  }
  if (input == "q" || input == "Q" || input == "quit")
  {
    return MenuResult("q", true, true);
  }
  if (input == "s" || input == "S")
  {
    if (player_context)
    {
      player_context->displayFullStatus();
      WaitForKeyPress();
    }
    return MenuResult("s", true, false);
  }

  // Check if input matches any option
  for (const auto &option : options)
  {
    if (option.key == input && option.enabled)
    {
      return MenuResult(input, true, false);
    }
  }

  UIHelper::PrintError("ตัวเลือกไม่ถูกต้อง กรุณาลองใหม่");
  WaitForKeyPress("กด Enter เพื่อลองใหม่...");
  return MenuResult("", false, false);
}

// Game-Specific Menus
MenuResult MenuSystem::ShowMainPhaseMenu(Player *current_player)
{
  vector<MenuOption> options = {
      MenuOption("1", "Ride การ์ด", Icons::CROWN, "เปลี่ยน Vanguard"),
      MenuOption("2", "Call ยูนิต", Icons::CARD, "วางการ์ดลง Rear Guard"),
      MenuOption("3", "ดูมือ (รายละเอียด)", Icons::HAND, "แสดงการ์ดบนมือทั้งหมด"),
      MenuOption("4", "ดูสนามรวม", Icons::FIELD, "แสดงสนามทั้งสองฝ่าย"),
      MenuOption("5", "ไป Battle Phase", Icons::SWORD, "เริ่มการต่อสู้"),
      MenuOption("h", "ช่วยเหลือ", Icons::HELP, "แสดงคำแนะนำ"),
      MenuOption("q", "ออกจากเกม", Icons::EXIT, "จบเกม")};

  // Check conditions for enabling/disabling options
  if (current_player->getHandSize() == 0)
  {
    options[0].enabled = false; // Can't ride
    options[1].enabled = false; // Can't call
  }

  return ShowMenu("MAIN PHASE - เลือกการกระทำ", options,
                  "วางแผนและเตรียมพร้อมสำหรับการต่อสู้", current_player);
}

MenuResult MenuSystem::ShowBattlePhaseMenu(Player *current_player)
{
  vector<MenuOption> options = {
      MenuOption("1", "เลือก Attacker", Icons::SWORD, "เลือกยูนิตที่จะโจมตี"),
      MenuOption("2", "ดูสนามรวม", Icons::FIELD, "ดูสนามทั้งสองฝ่าย"),
      MenuOption("3", "จบ Battle Phase", Icons::NEXT, "ไปยัง End Phase"),
      MenuOption("h", "ช่วยเหลือ", Icons::HELP, "คำแนะนำการต่อสู้"),
      MenuOption("q", "ออกจากเกม", Icons::EXIT, "จบเกม")};

  return ShowMenu("BATTLE PHASE - การต่อสู้", options,
                  "เลือกยูนิตเพื่อโจมตีฝ่ายตรงข้าม", current_player);
}

MenuResult MenuSystem::ShowRideMenu(Player *current_player)
{
  if (current_player->getHandSize() == 0)
  {
    UIHelper::PrintWarning("ไม่มีการ์ดบนมือให้ Ride");
    WaitForKeyPress();
    return MenuResult("", false, false);
  }

  UIHelper::ClearScreen();
  UIHelper::PrintQuickStatus(current_player->getName(),
                             current_player->getHandSize(),
                             current_player->getDeck().getSize(),
                             current_player->getDamageCount(), 0, 0);

  UIHelper::PrintSectionHeader("RIDE PHASE - เลือกการ์ดเพื่อ Ride", Icons::CROWN);

  // Show current VG
  if (current_player->getVanguard().has_value())
  {
    cout << Colors::YELLOW << "Vanguard ปัจจุบัน: " << Colors::RESET;
    const Card &vg = current_player->getVanguard().value();
    cout << UIHelper::FormatCard(vg.getName(), vg.getGrade()) << endl;
    cout << Colors::BRIGHT_BLACK << "คุณสามารถ Ride เกรด " << (vg.getGrade() + 1)
         << " หรือเกรดเดียวกัน" << Colors::RESET << "\n\n";
  }

  // Show hand
  current_player->displayHand(true);

  int selection = GetCardSelectionFromHand(current_player, "Ride");
  return MenuResult(to_string(selection), selection >= 0, false);
}

MenuResult MenuSystem::ShowCallMenu(Player *current_player)
{
  if (current_player->getHandSize() == 0)
  {
    UIHelper::PrintWarning("ไม่มีการ์ดบนมือให้ Call");
    WaitForKeyPress();
    return MenuResult("", false, false);
  }

  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("CALL PHASE - เรียกยูนิต", Icons::CARD);

  current_player->displayField();
  current_player->displayHand(true);

  int card_selection = GetCardSelectionFromHand(current_player, "Call");
  if (card_selection < 0)
  {
    return MenuResult("", false, false);
  }

  int rc_selection = GetRCPositionSelection("Call");
  if (rc_selection < 0)
  {
    return MenuResult("", false, false);
  }

  return MenuResult(to_string(card_selection) + "," + to_string(rc_selection), true, false);
}

MenuResult MenuSystem::ShowAttackerMenu(Player *current_player)
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("เลือก Attacker", Icons::SWORD);

  current_player->displayField();

  vector<pair<int, string>> attackers = current_player->chooseAttacker();

  if (attackers.empty())
  {
    UIHelper::PrintWarning("ไม่มียูนิตที่สามารถโจมตีได้");
    WaitForKeyPress();
    return MenuResult("", false, false);
  }

  vector<MenuOption> options;
  for (size_t i = 0; i < attackers.size(); i++)
  {
    string icon = (attackers[i].first == 0) ? Icons::CROWN : Icons::SWORD;
    options.emplace_back(to_string(i), attackers[i].second, icon);
  }
  options.emplace_back("-1", "ยกเลิก", Icons::BACK);

  MenuResult result = ShowMenu("เลือกยูนิตที่จะโจมตี", options, "", current_player);

  if (result.is_valid && result.selected_key != "-1")
  {
    int index = stoi(result.selected_key);
    return MenuResult(to_string(attackers[index].first), true, false);
  }

  return MenuResult("", false, false);
}

MenuResult MenuSystem::ShowGuardMenu(Player *current_player, int incoming_power)
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("GUARD PHASE - ป้องกัน", Icons::SHIELD);

  cout << Colors::BRIGHT_RED << Icons::SWORD << " พลังโจมตีที่เข้ามา: "
       << incoming_power << Colors::RESET << "\n\n";

  current_player->displayHand(true);
  current_player->displayGuardianZone();

  cout << "\n"
       << Colors::BRIGHT_BLUE << Icons::SHIELD << " Shield รวมปัจจุบัน: "
       << current_player->getGuardianZoneShieldTotal() << Colors::RESET << "\n\n";

  vector<MenuOption> options = {
      MenuOption("1", "เลือกการ์ดเพื่อ Guard", Icons::SHIELD, "ป้องกันด้วยการ์ดจากมือ"),
      MenuOption("2", "จบการ Guard", Icons::CONFIRM, "ยอมรับการโจมตี"),
      MenuOption("h", "ช่วยเหลือ", Icons::HELP, "คำแนะนำการป้องกัน")};

  if (current_player->getHandSize() == 0)
  {
    options[0].enabled = false;
  }

  return ShowMenu("เลือกการกระทำ Guard", options, "", current_player);
}

// Input Functions
int MenuSystem::GetCardSelectionFromHand(Player *player, const string &action_name, bool allow_cancel)
{
  if (player->getHandSize() == 0)
  {
    UIHelper::PrintError("ไม่มีการ์ดบนมือ");
    return -1;
  }

  string prompt = "เลือกการ์ดเพื่อ " + action_name + " (0-" +
                  to_string(player->getHandSize() - 1) + ")";
  if (allow_cancel)
  {
    prompt += " หรือ -1 เพื่อยกเลิก";
  }
  prompt += ": ";

  int min_val = allow_cancel ? -1 : 0;
  int max_val = static_cast<int>(player->getHandSize()) - 1;

  return GetIntegerInput(prompt, min_val, max_val, player);
}

int MenuSystem::GetRCPositionSelection(const std::string & /*action_name*/ )
{
  UIHelper::PrintInfo("เลือกตำแหน่ง Rear Guard Circle:");
  cout << Colors::CYAN << "[0] FL (Front Left)   [1] FR (Front Right)" << Colors::RESET << "\n";
  cout << Colors::CYAN << "[2] BL (Back Left)    [3] BC (Back Center)    [4] BR (Back Right)" << Colors::RESET << "\n";

  return GetIntegerInput("เลือกตำแหน่ง RC (0-4) หรือ -1 เพื่อยกเลิก: ", -1, 4);
}

int MenuSystem::GetTargetSelection(Player *attacker, Player *defender)
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("เลือกเป้าหมายการโจมตี", Icons::TARGET);

  attacker->displayField();
  cout << "\n"
       << Colors::BRIGHT_RED << "--- สนามของฝ่ายป้องกัน ("
       << defender->getName() << ") ---" << Colors::RESET << "\n";
  defender->displayField(true);

  vector<pair<int, string>> targets;

  // Add VG as target
  if (defender->getVanguard().has_value())
  {
    targets.push_back({0, "VC: " + defender->getVanguard().value().getName()});
  }

  // Add front row RCs as targets
  const auto &rcs = defender->getRearGuards();
  if (rcs[0].has_value())
  { // RC_FRONT_LEFT
    targets.push_back({1, "RC FL: " + rcs[0].value().getName()});
  }
  if (rcs[1].has_value())
  { // RC_FRONT_RIGHT
    targets.push_back({2, "RC FR: " + rcs[1].value().getName()});
  }

  if (targets.empty())
  {
    UIHelper::PrintError("ไม่มีเป้าหมายที่ถูกต้อง");
    return -1;
  }

  cout << "\n"
       << Colors::BRIGHT_YELLOW << "เป้าหมายที่โจมตีได้:" << Colors::RESET << "\n";
  for (size_t i = 0; i < targets.size(); i++)
  {
    cout << "[" << i << "] " << targets[i].second << "\n";
  }

  int selection = GetIntegerInput("เลือกเป้าหมาย: ", 0, static_cast<int>(targets.size()) - 1);
  return selection >= 0 ? targets[selection].first : -1;
}

// Confirmation Functions
bool MenuSystem::ShowConfirmation(const string &message, const string &details)
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("ยืนยันการกระทำ", Icons::HELP);

  cout << Colors::BRIGHT_YELLOW << Icons::WARNING << " " << message << Colors::RESET << "\n";
  if (!details.empty())
  {
    cout << Colors::BRIGHT_BLACK << details << Colors::RESET << "\n";
  }

  return ShowYesNoPrompt("คุณต้องการดำเนินการต่อหรือไม่?");
}

bool MenuSystem::ShowYesNoPrompt(const string &question, bool default_yes)
{
  string prompt = question + " (" +
                  (default_yes ? Colors::GREEN + "Y" + Colors::RESET + "/n" : "y/" + Colors::GREEN + "N" + Colors::RESET) + "): ";

  char response = GetCharInput(prompt, "yYnN");

  if (response == 'y' || response == 'Y')
    return true;
  if (response == 'n' || response == 'N')
    return false;
  return default_yes;
}

// Information Display
void MenuSystem::ShowCardDetails(const Card &card)
{
  UIHelper::ClearScreen();
  UIHelper::PrintCardPreview(card.getName(), card.getGrade(), card.getPower(),
                             card.getShield(), card.getCritical(),
                             card.getSkillDescription(), card.getTypeRole());
  WaitForKeyPress();
}

void MenuSystem::ShowFieldOverview(Player *player1, Player *player2, Player *current_player)
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("FIELD OVERVIEW", Icons::FIELD);
  // Show current player's field
  cout << Colors::BRIGHT_CYAN << "=== " << current_player->getName() << " (Your Field) ===" << Colors::RESET << "\n";
  current_player->displayField();
  // Show opponent's field if provided
  if (player1 && player2)
  {
    Player *opponent = (current_player == player1) ? player2 : player1;
    cout << "\n"
         << Colors::BRIGHT_RED << "=== " << opponent->getName() << " (Opponent Field) ===" << Colors::RESET << "\n";
    opponent->displayField();
  }
  WaitForKeyPress("กด Enter เพื่อกลับ...");
}

void MenuSystem::ShowGameHelp(const string &current_phase)
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("GAME HELP", Icons::HELP);

  cout << Colors::BRIGHT_CYAN << "📖 การควบคุมเกม:" << Colors::RESET << "\n";
  cout << "[h] - แสดงความช่วยเหลือ\n";
  cout << "[s] - แสดงสถานะเต็ม\n";
  cout << "[f] - แสดงสนามรวม\n";
  cout << "[q] - ออกจากเกม\n\n";

  cout << Colors::BRIGHT_YELLOW << "🎯 วัตถุประสงค์:" << Colors::RESET << "\n";
  cout << "• ทำดาเมจใส่คู่แข่งครบ 6 แต้ม\n";
  cout << "• หรือทำให้เด็คของคู่แข่งหมด\n\n";

  cout << Colors::BRIGHT_GREEN << "⚔️ การต่อสู้:" << Colors::RESET << "\n";
  cout << "• เลือก Attacker จาก VC หรือ RC แถวหน้า\n";
  cout << "• เลือก Booster จากแถวหลัง (ถ้ามี)\n";
  cout << "• VG โจมตี = Drive Check\n";
  cout << "• Hit = ฝ่ายตรงข้าม Damage Check\n\n";

  if (!current_phase.empty())
  {
    ShowPhaseHelp(current_phase);
  }

  WaitForKeyPress();
}

void MenuSystem::ShowPhaseHelp(const string &phase_name)
{
  cout << Colors::BRIGHT_MAGENTA << "📋 " << phase_name << " Help:" << Colors::RESET << "\n";

  if (phase_name == "MAIN PHASE")
  {
    cout << "• Ride: เปลี่ยน VG (เกรด +1 หรือเท่าเดิม)\n";
    cout << "• Call: วางยูนิตลง RC (เกรด ≤ VG)\n";
  }
  else if (phase_name == "BATTLE PHASE")
  {
    cout << "• เลือก Attacker (VC/RC แถวหน้า)\n";
    cout << "• เลือก Booster (แถวหลัง G0-1)\n";
    cout << "• โจมตีได้หลายครั้งต่อเทิร์น\n";
  }
  else if (phase_name == "GUARD PHASE")
  {
    cout << "• ใช้การ์ดจากมือเพื่อป้องกัน\n";
    cout << "• Shield รวม ≥ Attack Power = ป้องกันสำเร็จ\n";
  }
  cout << "\n";
}

// Game State Functions
MenuResult MenuSystem::ShowGameStartMenu()
{
  UIHelper::PrintGameTitle();

  vector<MenuOption> options = {
      MenuOption("1", "เริ่มเกมใหม่", Icons::CONFIRM, "เริ่มเล่น FIBO Card Commandos"),
      MenuOption("2", "คำแนะนำการเล่น", Icons::HELP, "เรียนรู้วิธีเล่น"),
      MenuOption("3", "ออกจากเกม", Icons::EXIT, "ปิดโปรแกรม")};

  return ShowMenu("MAIN MENU", options, "ยินดีต้อนรับสู่ FIBO Card Commandos!");
}

void MenuSystem::ShowGameOverScreen(Player *winner, Player *loser, const string &reason)
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("GAME OVER", Icons::VICTORY, Colors::BRIGHT_YELLOW);

  cout << "\n"
       << Colors::BRIGHT_GREEN << Icons::VICTORY << " ผู้ชนะ: "
       << Colors::BOLD << winner->getName() << Colors::RESET << "\n";
  cout << Colors::BRIGHT_RED << Icons::SKULL << " ผู้แพ้: "
       << loser->getName() << Colors::RESET << "\n\n";

  cout << Colors::BRIGHT_CYAN << "สาเหตุ: " << reason << Colors::RESET << "\n\n";

  // Show final stats
  cout << Colors::BRIGHT_YELLOW << "📊 สถิติสุดท้าย:" << Colors::RESET << "\n";
  cout << winner->getName() << " - ดาเมจ: " << winner->getDamageCount()
       << "/6, เด็ค: " << winner->getDeck().getSize() << " ใบ\n";
  cout << loser->getName() << " - ดาเมจ: " << loser->getDamageCount()
       << "/6, เด็ค: " << loser->getDeck().getSize() << " ใบ\n\n";

  UIHelper::PrintBox("ขอบคุณที่เล่น FIBO Card Commandos!", "THANK YOU",
                     Colors::BRIGHT_MAGENTA, Colors::BRIGHT_WHITE);
}

void MenuSystem::ShowTurnStartScreen(Player *current_player, int turn_number)
{
  UIHelper::ClearScreen();
  UIHelper::PrintPhaseHeader("TURN START", current_player->getName(), turn_number);

  current_player->displayField();

  cout << "\n"
       << Colors::BRIGHT_GREEN << Icons::CONFIRM << " ยูนิตทั้งหมด Stand แล้ว" << Colors::RESET << "\n";
  cout << Colors::BRIGHT_CYAN << Icons::CARD << " จั่วการ์ด 1 ใบ" << Colors::RESET << "\n\n";

  WaitForKeyPress("กด Enter เพื่อเริ่มเทิร์น...");
}

// Utility Functions
void MenuSystem::WaitForKeyPress(const string &message)
{
  UIHelper::PauseForUser(message.empty() ? "กด Enter เพื่อดำเนินการต่อ..." : message);
}

string MenuSystem::FormatMenuOption(const MenuOption &option, int index, bool numbered)
{
  string result = "";

  if (numbered && index >= 0)
  {
    result += "[" + to_string(index) + "] ";
  }
  else
  {
    result += "[" + option.key + "] ";
  }

  result += option.icon + " " + option.display_text;

  if (!option.description.empty())
  {
    result += " - " + option.description;
  }

  return result;
}

void MenuSystem::ShowLoadingScreen(const string &message, int duration_ms)
{
  UIHelper::ShowLoadingAnimation(message, duration_ms);
}

// Advanced Input
string MenuSystem::GetPlayerName(const string &prompt)
{
  string name;
  while (true)
  {
    cout << Colors::BRIGHT_CYAN << prompt << Colors::RESET;
    getline(cin, name);

    if (!name.empty() && name.length() <= 20)
    {
      return name;
    }

    UIHelper::PrintError("ชื่อผู้เล่นต้องมี 1-20 ตัวอักษร");
  }
}

int MenuSystem::GetIntegerInput(const string &prompt, int min_val, int max_val, Player *player_context)
{
  int value;
  string input;

  while (true)
  {
    cout << Colors::BRIGHT_CYAN << prompt << Colors::RESET;
    getline(cin, input);

    // Handle special commands
    if (input == "h" && player_context)
    {
      player_context->displayHand(true);
      continue;
    }
    if (input == "s" && player_context)
    {
      player_context->displayFullStatus();
      continue;
    }

    try
    {
      value = stoi(input);
      if (value >= min_val && value <= max_val)
      {
        return value;
      }
      UIHelper::PrintError("ตัวเลขต้องอยู่ระหว่าง " + to_string(min_val) +
                           " ถึง " + to_string(max_val));
    }
    catch (...)
    {
      UIHelper::PrintError("กรุณาใส่ตัวเลขที่ถูกต้อง");
    }
  }
}

char MenuSystem::GetCharInput(const std::string &prompt, const std::string &valid_chars, [[maybe_unused]] Player *player_context)
{
  string input;

  while (true)
  {
    cout << Colors::BRIGHT_CYAN << prompt << Colors::RESET;
    getline(cin, input);

    if (input.length() == 1)
    {
      char c = tolower(input[0]);
      if (valid_chars.find(c) != string::npos)
      {
        return c;
      }
    }

    UIHelper::PrintError("กรุณาเลือกจาก: " + valid_chars);
  }
}