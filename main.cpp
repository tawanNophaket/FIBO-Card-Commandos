// main.cpp - ตัวอย่างการใช้งาน Player class และ Main Phase แบบโต้ตอบ
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <optional>
#include <limits>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "json.hpp"

using json = nlohmann::json;

// --- ฟังก์ชัน Helper สำหรับ Output Formatting ---
void ClearScreen()
{
#ifdef _WIN32
  HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD count;
  DWORD cellCount;
  COORD homeCoords = {0, 0};
  if (hStdOut == INVALID_HANDLE_VALUE)
    return;
  if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
    return;
  cellCount = csbi.dwSize.X * csbi.dwSize.Y;
  if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count))
    return;
  if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count))
    return;
  SetConsoleCursorPosition(hStdOut, homeCoords);
#else
  std::system("clear");
#endif
}

void printLine(char c = '-', int length = 70)
{
  std::cout << std::string(length, c) << std::endl;
}

void printSectionHeader(const std::string &title, char border_char = '=', bool add_newline_before = true)
{
  if (add_newline_before)
    std::cout << std::endl;
  printLine(border_char);
  int title_len = 0;
  for (char ch : title)
  {
    title_len++;
  }
  int padding = (70 - title_len) / 2;
  if (padding < 0)
    padding = 0;
  std::cout << std::string(padding, ' ') << title << std::endl;
  printLine(border_char);
}

void printBoxedMessage(const std::string &message, char border_char = '*')
{
  std::cout << std::endl;
  std::string border_line(message.length() + 4, border_char);
  std::cout << border_line << std::endl;
  std::cout << border_char << " " << message << " " << border_char << std::endl;
  std::cout << border_line << std::endl;
}

void printError(const std::string &error_message)
{
  std::cout << "\n*** 🛑 ERROR: " << error_message << " 🛑 ***" << std::endl;
}

void printWarning(const std::string &warning_message)
{
  std::cout << "\n--- ⚠️ Warning: " << warning_message << " ---" << std::endl;
}

std::string getPlayerNameInput(const std::string &prompt)
{
  std::string name;
  while (true)
  {
    std::cout << prompt;
    std::getline(std::cin, name);
    if (!name.empty())
    {
      return name;
    }
    printError("ชื่อผู้เล่นไม่สามารถเว้นว่างได้");
  }
}

std::vector<Card> loadCardsFromJson(const std::string &filename)
{
  std::vector<Card> all_cards;
  std::ifstream file_stream(filename);
  if (!file_stream.is_open())
  {
    printError("เปิดไฟล์ JSON '" + filename + "' ไม่ได้");
    return all_cards;
  }
  json card_data_json;
  try
  {
    file_stream >> card_data_json;
  }
  catch (json::parse_error &e)
  {
    printError("Parse JSON ผิดพลาด: " + std::string(e.what()));
    file_stream.close();
    return all_cards;
  }
  file_stream.close();
  if (!card_data_json.is_array())
  {
    printError("ข้อมูล JSON ไม่ได้อยู่ในรูปแบบ Array");
    return all_cards;
  }
  for (const auto &obj : card_data_json)
  {
    try
    {
      all_cards.emplace_back(obj.value("code_name", "N/A"),
                             obj.value("name", "Unknown"),
                             obj.value("grade", -1),
                             obj.value("power", 0),
                             obj.value("shield", 0),
                             obj.value("skill_description", ""),
                             obj.value("type_role", "Unknown"),
                             obj.value("critical", 1));
    }
    catch (json::type_error &e)
    {
      printWarning("Type error ขณะอ่าน JSON object: " + std::string(e.what()));
    }
  }
  return all_cards;
}

int getIntegerInput(const std::string &prompt, Player *player_for_display, int min_val = -1000000, int max_val = 1000000, bool clear_before_prompt = true, bool show_field_before_prompt = true)
{
  int choice = -1;
  std::string input_str;
  while (true)
  {
    if (clear_before_prompt)
      ClearScreen();
    if (player_for_display && show_field_before_prompt)
    {
      player_for_display->displayField();
    }
    std::cout << prompt;
    std::cin >> input_str;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (player_for_display)
    {
      if (input_str == "h" || input_str == "H")
      {
        ClearScreen();
        player_for_display->displayField();
        player_for_display->displayHand(true);
        std::cout << "(กด Enter เพื่อกลับไปเลือก...)" << std::endl;
        std::cin.get();
        continue;
      }
    }

    try
    {
      choice = std::stoi(input_str);
      if (choice >= min_val && choice <= max_val)
      {
        return choice;
      }
      else
      {
        printError("INPUT ERROR: ตัวเลขไม่อยู่ในช่วงที่กำหนด (" + std::to_string(min_val) + " ถึง " + std::to_string(max_val) + ")");
      }
    }
    catch (const std::invalid_argument &ia)
    {
      printError("INPUT ERROR: กรุณาใส่เป็นตัวเลข หรือคำสั่ง (h)");
    }
    catch (const std::out_of_range &oor)
    {
      printError("INPUT ERROR: ตัวเลขมีขนาดใหญ่หรือเล็กเกินไป");
    }
    std::cout << "(กด Enter เพื่อลองใหม่...)" << std::endl;
    std::cin.get();
  }
}

char getActionInput(const std::string &prompt, Player *player_for_display, bool clear_before_prompt = true, bool show_field_before_prompt = true)
{
  char choice_char = ' ';
  std::string input_line;
  while (true)
  {
    if (clear_before_prompt)
      ClearScreen();
    if (player_for_display && show_field_before_prompt)
    {
      player_for_display->displayField();
    }
    std::cout << prompt;
    std::getline(std::cin, input_line);

    if (input_line.length() == 1)
    {
      choice_char = std::tolower(input_line[0]);
      if (choice_char == 'y' || choice_char == 'n')
      {
        return choice_char;
      }
      else if (player_for_display && (choice_char == 'h'))
      {
        ClearScreen();
        player_for_display->displayField();
        player_for_display->displayHand(true);
        std::cout << "(กด Enter เพื่อกลับไปเลือก...)" << std::endl;
        std::cin.get();
        continue;
      }
    }
    printError("INPUT ERROR: กรุณาตอบ 'y', 'n', หรือคำสั่ง (h)");
    std::cout << "(กด Enter เพื่อลองใหม่...)" << std::endl;
    std::cin.get();
  }
}

int chooseTargetFromOpponent(Player *attacker, Player *defender)
{
  ClearScreen();
  // attacker->displayField();
  printSectionHeader("สนามของฝ่ายป้องกัน (" + defender->getName() + ")", '~', false);
  defender->displayField(true);

  std::vector<std::pair<int, std::string>> available_targets;
  if (defender->getVanguard().has_value())
  {
    available_targets.push_back({UNIT_STATUS_VC_IDX, "VC: " + defender->getVanguard().value().getName()});
  }
  const auto &opp_rcs = defender->getRearGuards();
  const size_t opp_front_row_rcs_indices[] = {RC_FRONT_LEFT, RC_FRONT_RIGHT};
  for (size_t rc_idx : opp_front_row_rcs_indices)
  {
    if (opp_rcs[rc_idx].has_value())
    {
      available_targets.push_back({(int)defender->getUnitStatusIndexForRC(rc_idx),
                                   std::to_string(rc_idx) + ": RC " + opp_rcs[rc_idx].value().getName()});
    }
  }

  if (available_targets.empty())
  {
    printError("ฝ่ายตรงข้ามไม่มียูนิตให้โจมตี!");
    std::cout << "(กด Enter เพื่อดำเนินการต่อ...)" << std::endl;
    std::cin.get();
    return -1;
  }

  std::cout << "\nเลือกเป้าหมายการโจมตีบนสนามของ " << defender->getName() << ":" << std::endl;
  for (size_t i = 0; i < available_targets.size(); ++i)
  {
    std::cout << i << ": " << available_targets[i].second << std::endl;
  }

  int choice_idx = getIntegerInput("เลือกหมายเลขเป้าหมาย: ", nullptr, 0, available_targets.size() - 1, false, false);
  return available_targets[static_cast<size_t>(choice_idx)].first;
}

int main()
{
  ClearScreen();
  printSectionHeader("🃏 FIBO CARD COMMANDOS - GAME START 🃏", '#', false);

  char startGameChoice = getActionInput("คุณต้องการเริ่มเกมหรือไม่? (y/n): ", nullptr, false, false);
  if (startGameChoice == 'n')
  {
    std::cout << "ไว้พบกันใหม่โอกาสหน้า! 👋" << std::endl;
    return 0;
  }
  ClearScreen();

  std::vector<Card> full_card_list = loadCardsFromJson("cards.json");
  if (full_card_list.empty())
  {
    printError("CRITICAL ERROR: ไม่สามารถโหลดฐานข้อมูลการ์ดได้ โปรแกรมจะจบการทำงาน");
    return 1;
  }

  std::string p1_name = getPlayerNameInput("👤 ใส่ชื่อผู้เล่น 1: ");
  ClearScreen();
  std::string p2_name = getPlayerNameInput("👤 ใส่ชื่อผู้เล่น 2: ");
  ClearScreen();

  std::map<std::string, int> deck_recipe_v1_3 = {
      {"G0-01", 1}, {"G0-02", 4}, {"G0-03", 4}, {"G0-04", 8}, {"G1-01", 3}, {"G1-02", 4}, {"G1-03", 3}, {"G1-04", 2}, {"G1-05", 1}, {"G2-01", 3}, {"G2-02", 3}, {"G2-03", 3}, {"G2-04", 2}, {"G3-01", 2}, {"G3-02", 2}, {"G3-03", 2}, {"G3-04", 2}, {"G4-01", 1}};

  Player player1(p1_name, Deck(full_card_list, deck_recipe_v1_3));
  Player player2(p2_name, Deck(full_card_list, deck_recipe_v1_3));

  // --- Mulligan Phase ---
  ClearScreen();
  printSectionHeader("MULLIGAN PHASE", '~');
  std::cout << player1.getName() << ", ตาคุณ Mulligan:" << std::endl;
  player1.performMulligan();
  std::cout << "(กด Enter เมื่อ " << player1.getName() << " Mulligan เสร็จ...)" << std::endl;
  std::cin.get();
  ClearScreen();

  std::cout << player2.getName() << ", ตาคุณ Mulligan:" << std::endl;
  player2.performMulligan();
  std::cout << "(กด Enter เมื่อ " << player2.getName() << " Mulligan เสร็จ...)" << std::endl;
  std::cin.get();
  ClearScreen();

  player1.setupGame("G0-01");
  player2.setupGame("G0-01");

  Player *currentPlayer = nullptr;
  Player *opponentPlayer = nullptr;

  printSectionHeader("เลือกผู้เล่นเริ่มก่อน", '~');
  int first_player_choice = getIntegerInput("ใครจะเริ่มเล่นก่อน? (1 สำหรับ " + p1_name + ", 2 สำหรับ " + p2_name + "): ", nullptr, 1, 2, true, false);
  if (first_player_choice == 1)
  {
    currentPlayer = &player1;
    opponentPlayer = &player2;
  }
  else
  {
    currentPlayer = &player2;
    opponentPlayer = &player1;
  }
  ClearScreen();
  printBoxedMessage(currentPlayer->getName() + " ได้เริ่มเล่นก่อน! ✨", '+');
  std::cout << "(กด Enter เพื่อเริ่มเทิร์นแรก...)" << std::endl;
  std::cin.get();

  int turn_count = 0;
  bool game_over = false;

  // --- Game Loop ---
  while (!game_over)
  {
    turn_count++;
    ClearScreen();
    printSectionHeader("เทิร์นที่ " + std::to_string(turn_count) + " ของ " + currentPlayer->getName() + " ⚔️", '*');

    // 1. Stand Phase
    currentPlayer->performStandPhase();
    std::cout << "✔️ ยูนิตทั้งหมด Stand แล้ว" << std::endl;

    // 2. Draw Phase
    if (!currentPlayer->performDrawPhase())
    {
      ClearScreen();
      printBoxedMessage("GAME OVER: " + currentPlayer->getName() + " แพ้เพราะเด็คหมดและจั่วการ์ดไม่ได้! 💀", '!');
      game_over = true;
      break;
    }
    std::cout << "✔️ " << currentPlayer->getName() << " จั่วการ์ด 1 ใบ (มือปัจจุบัน: " << currentPlayer->getHandSize() << " ใบ)" << std::endl;
    currentPlayer->displayHand(true);
    std::cout << "(กด Enter เพื่อเข้า Ride Phase...)" << std::endl;
    std::cin.get();

    // 3. Ride Phase
    ClearScreen();
    printSectionHeader(currentPlayer->getName() + ": RIDE PHASE 🏍️", '-');
    bool ride_successful_this_turn = false;
    while (!ride_successful_this_turn)
    {
      char ride_choice_char = getActionInput("คุณต้องการ Ride การ์ดหรือไม่? (y/n): ", currentPlayer, false);
      if (ride_choice_char == 'n')
      {
        std::cout << "ข้าม Ride Phase" << std::endl;
        break;
      }

      if (currentPlayer->getHandSize() == 0)
      {
        ClearScreen();
        currentPlayer->displayField();
        std::cout << "ไม่มีการ์ดบนมือให้ Ride" << std::endl;
        std::cout << "(กด Enter เพื่อดำเนินการต่อ...)" << std::endl;
        std::cin.get();
        break;
      }

      ClearScreen();
      currentPlayer->displayField();
      currentPlayer->displayHand(true);
      int card_idx_ride = getIntegerInput("เลือกการ์ดบนมือเพื่อ Ride (-1 เพื่อยกเลิก/ข้าม): ", nullptr, -1, currentPlayer->getHandSize() - 1, false, false);
      if (card_idx_ride == -1)
      {
        std::cout << "ยกเลิกการ Ride ในเทิร์นนี้" << std::endl;
        break;
      }

      if (currentPlayer->rideFromHand(static_cast<size_t>(card_idx_ride)))
      {
        ClearScreen();
        printBoxedMessage("Ride สำเร็จ! 🚀", '+');
        ride_successful_this_turn = true;
      }
      else
      {
        printError("RIDE FAILED: ไม่สามารถ Ride การ์ดที่เลือกได้");
        std::cout << "(กด Enter เพื่อลองใหม่ หรือเลือกยกเลิก...)" << std::endl;
        std::cin.get();
        ClearScreen();
      }
    }
    std::cout << "(กด Enter เพื่อเข้า Main Phase...)" << std::endl;
    std::cin.get();

    // 4. Main Phase (Call Units)
    ClearScreen();
    printSectionHeader(currentPlayer->getName() + ": MAIN PHASE (Call Units) 🃏", '-');
    while (true)
    {
      if (currentPlayer->getHandSize() == 0)
      {
        ClearScreen();
        currentPlayer->displayField();
        std::cout << "ไม่มีการ์ดบนมือให้ Call แล้ว" << std::endl;
        std::cout << "(กด Enter เพื่อดำเนินการต่อ...)" << std::endl;
        std::cin.get();
        break;
      }
      char call_choice_char = getActionInput("คุณต้องการ Call ยูนิตหรือไม่? (y/n): ", currentPlayer, false);
      if (call_choice_char == 'n')
      {
        std::cout << "จบการ Call ยูนิตใน Main Phase นี้" << std::endl;
        break;
      }

      ClearScreen();
      currentPlayer->displayField();
      currentPlayer->displayHand(true);
      int card_idx_call = getIntegerInput("เลือกการ์ดบนมือเพื่อ Call (-1 เพื่อยกเลิกครั้งนี้): ", nullptr, -1, currentPlayer->getHandSize() - 1, false, false);
      if (card_idx_call == -1)
      {
        std::cout << "ยกเลิกการ Call ครั้งนี้" << std::endl;
        continue;
      }

      ClearScreen();
      currentPlayer->displayField();
      std::cout << "เลือกตำแหน่ง RC (0:FL, 1:FR, 2:BL, 3:BC, 4:BR): ";
      int rc_idx_call = getIntegerInput("ใส่หมายเลขช่อง RC: ", nullptr, 0, NUM_REAR_GUARD_CIRCLES - 1, false, false);
      if (currentPlayer->callToRearGuard(static_cast<size_t>(card_idx_call), static_cast<size_t>(rc_idx_call)))
      {
        ClearScreen();
        printBoxedMessage("Call สำเร็จ! ✨", '+');
      }
      else
      {
        printError("CALL FAILED: ไม่สามารถ Call การ์ดลงตำแหน่งที่เลือกได้");
        std::cout << "(กด Enter เพื่อดำเนินการต่อ...)" << std::endl;
        std::cin.get();
      }
    }
    std::cout << "(กด Enter เพื่อเข้า Battle Phase...)" << std::endl;
    std::cin.get();

    // 5. Battle Phase
    ClearScreen();
    printSectionHeader(currentPlayer->getName() + ": BATTLE PHASE 💥", '-');
    currentPlayer->clearGuardianZoneAndMoveToDrop();
    opponentPlayer->clearGuardianZoneAndMoveToDrop();
    currentPlayer->resetBattleBuffs();
    opponentPlayer->resetBattleBuffs();

    char attack_again_choice = 'y';
    while (attack_again_choice == 'y' && !game_over)
    {
      ClearScreen();
      currentPlayer->displayField();

      std::vector<std::pair<int, std::string>> attackers_options = currentPlayer->chooseAttacker();
      int attacker_status_idx = -1;
      if (!attackers_options.empty())
      {
        int choice_idx = getIntegerInput("เลือกหมายเลขยูนิตที่จะโจมตี (-1 เพื่อข้าม Battle Phase): ", nullptr, -1, attackers_options.size() - 1, false, false);
        if (choice_idx != -1)
        {
          attacker_status_idx = attackers_options[static_cast<size_t>(choice_idx)].first;
        }
      }

      if (attacker_status_idx == -1)
      {
        std::cout << "ข้าม Battle Phase" << std::endl;
        break;
      }

      std::optional<Card> attacker_card_opt = currentPlayer->getUnitAtStatusIndex(attacker_status_idx);
      ClearScreen();
      printBoxedMessage("⚔️ " + currentPlayer->getName() + " เลือก " + attacker_card_opt.value().getName() + " เป็น Attacker. ⚔️", '!');
      std::cout << "(กด Enter เพื่อเลือกเป้าหมาย...)" << std::endl;
      std::cin.get();

      int target_status_idx = chooseTargetFromOpponent(currentPlayer, opponentPlayer);
      if (target_status_idx == -1)
      {
        std::cout << "ข้ามการโจมตีนี้ (ไม่มีเป้าหมายที่ถูกต้อง)" << std::endl;
        attack_again_choice = getActionInput("\nคุณ (" + currentPlayer->getName() + ") ต้องการโจมตีอีกครั้งหรือไม่? (y/n): ", currentPlayer, true, true);
        continue;
      }
      std::optional<Card> target_card_opt = opponentPlayer->getUnitAtStatusIndex(target_status_idx);
      if (!target_card_opt.has_value())
      {
        printError("CRITICAL ERROR: ไม่พบการ์ดเป้าหมายที่เลือก");
        break;
      }
      ClearScreen();
      currentPlayer->displayField();
      std::cout << "\n--- สนามของฝ่ายป้องกัน (" << opponentPlayer->getName() << ") ---" << std::endl;
      opponentPlayer->displayField(true);
      printBoxedMessage(attacker_card_opt.value().getName() + " 🎯 โจมตี " + target_card_opt.value().getName() + "!", '!');
      std::cout << "(กด Enter เพื่อเลือก Booster...)" << std::endl;
      std::cin.get();

      int potential_booster_idx = currentPlayer->chooseBooster(attacker_status_idx);
      int booster_status_idx = -1;
      if (potential_booster_idx != -1)
      {
        char boost_confirm_choice = getActionInput("คุณต้องการ Boost หรือไม่? (y/n): ", currentPlayer);
        if (boost_confirm_choice == 'y')
        {
          booster_status_idx = potential_booster_idx;
          std::optional<Card> booster_card_opt = currentPlayer->getUnitAtStatusIndex(booster_status_idx);
          if (booster_card_opt.has_value())
          {
            std::cout << "🛡️ " << currentPlayer->getName() << " เลือก " << booster_card_opt.value().getName() << " เป็น Booster." << std::endl;
          }
        }
      }

      int final_attacker_power = currentPlayer->getUnitPowerAtStatusIndex(attacker_status_idx, booster_status_idx, false);
      int final_attacker_crit = currentPlayer->getUnitCriticalAtStatusIndex(attacker_status_idx);

      if (attacker_status_idx == UNIT_STATUS_VC_IDX)
      {
        ClearScreen();
        currentPlayer->displayField();
        printSectionHeader(currentPlayer->getName() + " ทำการ DRIVE CHECK 💎", '~');
        int num_drives = (attacker_card_opt.value().getGrade() >= 3) ? 2 : 1;
        TriggerOutput drive_effects = currentPlayer->performDriveCheck(num_drives, opponentPlayer, attacker_status_idx);
        // Power และ Crit ที่ได้จาก Drive Trigger จะถูก update ใน current_battle_buffs โดย performDriveCheck/applyTriggerEffect
        // ดังนั้น เราดึงค่าล่าสุดอีกครั้ง
        final_attacker_power = currentPlayer->getUnitPowerAtStatusIndex(attacker_status_idx, booster_status_idx, false);
        final_attacker_crit = currentPlayer->getUnitCriticalAtStatusIndex(attacker_status_idx);
        std::cout << "(กด Enter เพื่อให้ฝ่ายตรงข้ามป้องกัน...)" << std::endl;
        std::cin.get();
      }

      std::cout << "🔥 พลังโจมตีสุดท้ายของ " << attacker_card_opt.value().getName() << ": " << final_attacker_power << " (Critical: " << final_attacker_crit << ")" << std::endl;
      std::cout << "(กด Enter เพื่อให้ฝ่ายตรงข้ามป้องกัน...)" << std::endl;
      std::cin.get();

      ClearScreen();
      std::cout << "\n🛡️ --- " << opponentPlayer->getName() << " เตรียมป้องกัน --- 🛡️" << std::endl;
      opponentPlayer->displayField(true);
      std::cout << "⚔️ พลังโจมตีที่เข้ามา: " << final_attacker_power << " (Critical: " << final_attacker_crit << ")" << std::endl;
      int opponent_target_base_power = opponentPlayer->getUnitPowerAtStatusIndex(target_status_idx, -1, true);
      std::cout << "🎯 เป้าหมายคือ: " << target_card_opt.value().getName()
                << " (Power ปัจจุบัน: " << opponent_target_base_power << ")" << std::endl;

      bool pg_activated_by_opponent = false; // Flag สำหรับ Perfect Guard
      int total_shield_from_guard = opponentPlayer->performGuardStep(final_attacker_power, target_card_opt, currentPlayer, pg_activated_by_opponent);

      int opponent_total_defense_power = opponent_target_base_power + (pg_activated_by_opponent ? 0 : total_shield_from_guard);
      ClearScreen();
      printSectionHeader("ผลการต่อสู้", '=');
      std::cout << currentPlayer->getName() << " (" << attacker_card_opt.value().getName() << ") Power: " << final_attacker_power << " Crit: " << final_attacker_crit << std::endl;
      std::cout << opponentPlayer->getName() << " (" << target_card_opt.value().getName() << ") Defense Power (รวม Guard): " << opponent_total_defense_power << std::endl;

      bool is_hit = false;
      if (pg_activated_by_opponent)
      {
        printBoxedMessage("🛡️💥 PERFECT GUARD!! การโจมตีถูกยกเลิก! 💥🛡️", '!');
        is_hit = false;
      }
      else if (final_attacker_power >= opponent_total_defense_power)
      {
        printBoxedMessage("⚔️💥 การโจมตี HIT! 💥⚔️", '!');
        is_hit = true;
      }
      else
      {
        printBoxedMessage("🛡️ การโจมตี MISS! (ถูกป้องกันได้) 🛡️", '-');
      }
      opponentPlayer->clearGuardianZoneAndMoveToDrop();
      std::cout << "(กด Enter เพื่อดำเนินการต่อ...)" << std::endl;
      std::cin.get();

      if (is_hit)
      {
        printSectionHeader(opponentPlayer->getName() + " ทำการ Damage Check " + std::to_string(final_attacker_crit) + " ครั้ง", '~');
        for (int i = 0; i < final_attacker_crit && !game_over; ++i)
        {
          ClearScreen();
          opponentPlayer->displayField();
          std::cout << "Damage Check ครั้งที่ " << (i + 1) << "/" << final_attacker_crit << std::endl;
          if (opponentPlayer->getDeck().isEmpty())
          {
            ClearScreen();
            printBoxedMessage("GAME OVER: " + opponentPlayer->getName() + " แพ้เพราะไม่สามารถทำ Damage Check ได้ (เด็คหมด)! 💀", '!');
            game_over = true;
            break;
          }
          std::optional<Card> damage_card_opt = opponentPlayer->getDeck().draw();
          if (damage_card_opt.has_value())
          {
            Card actual_damage_card = damage_card_opt.value();
            std::cout << "เปิดได้: " << actual_damage_card << std::endl;
            opponentPlayer->takeDamage(actual_damage_card);

            if (actual_damage_card.getTypeRole().find("Trigger") != std::string::npos)
            {
              int defending_vg_idx = UNIT_STATUS_VC_IDX;
              TriggerOutput dmg_effect = opponentPlayer->applyTriggerEffect(actual_damage_card, false, currentPlayer, defending_vg_idx);
              // Power ที่ได้จาก Damage Trigger จะถูกเก็บใน current_battle_buffs ของ opponentPlayer
              // และจะมีผลถ้า Vanguard นั้นถูกโจมตีอีก หรือในเทิร์นหน้า (ถ้าไม่ถูกล้าง)
            }

            opponentPlayer->displayField();
            if (opponentPlayer->getDamageCount() >= 6)
            {
              game_over = true;
              break;
            }
          }
          if (!game_over && i < final_attacker_crit - 1)
          {
            std::cout << "(กด Enter เพื่อดูดาเมจต่อไป...)" << std::endl;
            std::cin.get();
          }
        }
        if (game_over)
          break;
      }

      currentPlayer->restUnit(attacker_status_idx);
      if (booster_status_idx != -1)
      {
        currentPlayer->restUnit(booster_status_idx);
      }

      if (game_over)
        break;

      ClearScreen();
      currentPlayer->displayField();
      attack_again_choice = getActionInput("\nคุณ (" + currentPlayer->getName() + ") ต้องการโจมตีอีกครั้งหรือไม่? (y/n): ", currentPlayer, false);
    }
    if (!game_over)
    {
      std::cout << "จบ Battle Phase ของ " << currentPlayer->getName() << std::endl;
      std::cout << "(กด Enter เพื่อดำเนินการต่อ...)" << std::endl;
      std::cin.get();
    }

    ClearScreen();
    printSectionHeader(currentPlayer->getName() + ": END PHASE 🌙", '-');
    currentPlayer->clearGuardianZoneAndMoveToDrop();
    opponentPlayer->clearGuardianZoneAndMoveToDrop();
    currentPlayer->resetBattleBuffs();
    opponentPlayer->resetBattleBuffs();
    std::cout << "(Placeholder: สกิลจบเทิร์น, ล้างสถานะชั่วคราว)" << std::endl;

    printSectionHeader("เทิร์นของ " + currentPlayer->getName() + " สิ้นสุด", '*');
    currentPlayer->displayField();
    opponentPlayer->displayField();
    std::cout << "(กด Enter เพื่อให้ผู้เล่นถัดไปเริ่มเทิร์น...)" << std::endl;
    std::cin.get();

    if (currentPlayer->getDamageCount() >= 6)
    {
      game_over = true;
    }
    if (opponentPlayer->getDamageCount() >= 6 && !game_over)
    {
      game_over = true;
    }
    // ตรวจสอบ Deck Out ที่นี่อีกครั้ง หลังจากทุกการกระทำในเทิร์นเสร็จสิ้น
    if (currentPlayer->getDeck().isEmpty() && !game_over)
    {
      // สมมติว่าถ้าเด็คหมดตอนจบเทิร์นของผู้เล่นคนนั้น แล้วเทิร์นหน้าเขาต้องจั่วแล้วจั่วไม่ได้ = แพ้
      // การตรวจสอบที่แม่นยำคือตอน Draw Phase
    }

    if (game_over)
      break;

    if (currentPlayer == &player1)
    {
      currentPlayer = &player2;
      opponentPlayer = &player1;
    }
    else
    {
      currentPlayer = &player1;
      opponentPlayer = &player2;
    }
  }

  ClearScreen();
  printSectionHeader("🏁 GAME END 🏁", '#');
  if (game_over)
  {
    Player *winner = nullptr;
    Player *loser = nullptr;

    bool p1_deck_out = player1.getDeck().isEmpty() && !player1.performDrawPhase(); // ตรวจสอบว่าถ้าต้องจั่วแล้วจั่วไม่ได้
    bool p2_deck_out = player2.getDeck().isEmpty() && !player2.performDrawPhase();

    if (player1.getDamageCount() >= 6)
    {
      loser = &player1;
      winner = &player2;
    }
    else if (player2.getDamageCount() >= 6)
    {
      loser = &player2;
      winner = &player1;
    }
    else if (p1_deck_out && currentPlayer == &player1)
    { // ถ้า P1 เด็คหมดในเทิร์น P1 (ตอน Draw Phase)
      loser = &player1;
      winner = &player2;
    }
    else if (p2_deck_out && currentPlayer == &player2)
    { // ถ้า P2 เด็คหมดในเทิร์น P2
      loser = &player2;
      winner = &player1;
    }
    // กรณีที่ opponent เด็คหมดจากการ Damage Check ก็จะถูกดักจับไปแล้วโดย game_over = true

    if (winner && loser)
    {
      printBoxedMessage(winner->getName() + " เป็นผู้ชนะ! (เนื่องจาก " + loser->getName() +
                            (loser->getDamageCount() >= 6 ? " มี 6 ดาเมจ" : " เด็คหมด") + ") 🎉",
                        '+');
    }
    else
    {
      printBoxedMessage("เกมจบลง! (ไม่สามารถระบุผู้ชนะจากเงื่อนไขปัจจุบัน)", '*');
    }
  }
  else
  {
    std::cout << "จบการจำลอง " << turn_count << " เทิร์น (ยังไม่มีผู้ชนะ)" << std::endl;
  }

  std::cout << "\nขอบคุณที่เล่น FIBO Card Commandos!" << std::endl;
  std::cout << "(กด Enter เพื่อออกจากโปรแกรม...)" << std::endl;
  std::cin.get();

  return 0;
}
