// main.cpp - ตัวอย่างการใช้งาน Player class และ Main Phase แบบโต้ตอบ
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <optional>
#include <limits>  // สำหรับ std::numeric_limits
#include <iomanip> // สำหรับ std::setw, std::left, std::setfill
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
      all_cards.emplace_back(obj.value("code_name", "N/A"), obj.value("name", "Unknown"), obj.value("grade", -1),
                             obj.value("power", 0), obj.value("shield", 0), obj.value("skill_description", ""),
                             obj.value("type_role", "Unknown"));
    }
    catch (json::type_error &e)
    {
      printWarning("Type error ขณะอ่าน JSON object: " + std::string(e.what()));
    }
  }
  return all_cards;
}

int getIntegerInput(const std::string &prompt, Player *player_for_display, int min_val = -1000000, int max_val = 1000000)
{
  int choice = -1;
  std::string input_str;
  while (true)
  {
    // ClearScreen(); // ไม่เคลียร์ทุกครั้งที่ถามตัวเลข แต่จะเคลียร์ก่อนเริ่ม action ใหญ่
    if (player_for_display)
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
  }
}

char getActionInput(const std::string &prompt, Player *player_for_display)
{
  char choice_char = ' ';
  std::string input_line;
  while (true)
  {
    // ClearScreen(); // ไม่เคลียร์ทุกครั้งที่ถาม y/n
    if (player_for_display)
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
        continue;
      }
    }
    printError("INPUT ERROR: กรุณาตอบ 'y', 'n', หรือคำสั่ง (h)");
  }
}

int main()
{
  ClearScreen();
  printSectionHeader("🃏 FIBO CARD COMMANDOS - GAME START 🃏", '#', false);

  char startGameChoice = getActionInput("คุณต้องการเริ่มเกมหรือไม่? (y/n): ", nullptr);
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

  player1.setupGame("G0-01");
  player2.setupGame("G0-01");

  Player *currentPlayer = nullptr;
  Player *opponentPlayer = nullptr;

  // printSectionHeader("เลือกผู้เล่นเริ่มก่อน", '~'); // ไม่ต้องแสดงแล้ว
  int first_player_choice = getIntegerInput("ใครจะเริ่มเล่นก่อน? (1 สำหรับ " + p1_name + ", 2 สำหรับ " + p2_name + "): ", nullptr, 1, 2);
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
  std::cout << "(กด Enter เพื่อดำเนินการต่อ...)" << std::endl;
  std::cin.get();

  int turn_count = 0;
  bool game_over = false;

  while (!game_over && turn_count < 20)
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
      printBoxedMessage("GAME OVER: " + currentPlayer->getName() + " แพ้เพราะจั่วการ์ดไม่ได้! 💀", '!');
      game_over = true;
      break;
    }
    std::cout << "✔️ " << currentPlayer->getName() << " จั่วการ์ด 1 ใบ (มือปัจจุบัน: " << currentPlayer->getHandSize() << " ใบ)" << std::endl;
    currentPlayer->displayHand(true);

    // 3. Ride Phase
    printSectionHeader(currentPlayer->getName() + ": RIDE PHASE 🏍️", '-');
    bool ride_successful_this_turn = false;
    while (!ride_successful_this_turn)
    {
      char ride_choice_char = getActionInput("คุณต้องการ Ride การ์ดหรือไม่? (y/n): ", currentPlayer);
      if (ride_choice_char == 'n')
      {
        std::cout << "ข้าม Ride Phase" << std::endl;
        break;
      }

      if (currentPlayer->getHandSize() == 0)
      {
        std::cout << "ไม่มีการ์ดบนมือให้ Ride" << std::endl;
        break;
      }

      currentPlayer->displayHand(true);
      int card_idx_ride = getIntegerInput("เลือกการ์ดบนมือเพื่อ Ride (-1 เพื่อยกเลิก/ข้าม): ", currentPlayer, -1, currentPlayer->getHandSize() - 1);
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
      }
    }

    // 4. Main Phase (Call Units)
    printSectionHeader(currentPlayer->getName() + ": MAIN PHASE (Call Units) 🃏", '-');
    while (true)
    {
      if (currentPlayer->getHandSize() == 0)
      {
        std::cout << "ไม่มีการ์ดบนมือให้ Call แล้ว" << std::endl;
        break;
      }

      char call_choice_char = getActionInput("คุณต้องการ Call ยูนิตหรือไม่? (y/n): ", currentPlayer);
      if (call_choice_char == 'n')
      {
        std::cout << "จบการ Call ยูนิตใน Main Phase นี้" << std::endl;
        break;
      }

      currentPlayer->displayHand(true);
      int card_idx_call = getIntegerInput("เลือกการ์ดบนมือเพื่อ Call (-1 เพื่อยกเลิกครั้งนี้): ", currentPlayer, -1, currentPlayer->getHandSize() - 1);
      if (card_idx_call == -1)
      {
        std::cout << "ยกเลิกการ Call ครั้งนี้" << std::endl;
        continue;
      }

      std::cout << "เลือกตำแหน่ง RC (0:FL, 1:FR, 2:BL, 3:BC, 4:BR): ";
      int rc_idx_call = getIntegerInput("ใส่หมายเลขช่อง RC: ", currentPlayer, 0, NUM_REAR_GUARD_CIRCLES - 1);

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

    // 5. Battle Phase
    printSectionHeader(currentPlayer->getName() + ": BATTLE PHASE 💥", '-');
    currentPlayer->clearGuardianZoneAndMoveToDrop();  // แก้ชื่อเมธอด
    opponentPlayer->clearGuardianZoneAndMoveToDrop(); // แก้ชื่อเมธอด

    char attack_again_choice = 'y';
    while (attack_again_choice == 'y')
    {
      ClearScreen();
      currentPlayer->displayField(); // แสดงสนามก่อนเลือกตัวโจมตี

      std::vector<std::pair<int, std::string>> attackers_options = currentPlayer->chooseAttacker();
      int attacker_status_idx = -1;

      if (!attackers_options.empty())
      {
        // ไม่ต้องเรียก displayField ซ้ำใน getIntegerInput ถ้าเพิ่งแสดงไป
        int choice_idx = getIntegerInput("เลือกหมายเลขยูนิตที่จะโจมตี (-1 เพื่อข้าม): ", nullptr, -1, attackers_options.size() - 1);
        if (choice_idx != -1)
        {
          attacker_status_idx = attackers_options[static_cast<size_t>(choice_idx)].first;
        }
      }
      else
      {
        // chooseAttacker() จะแสดงข้อความนี้เองแล้ว
      }

      if (attacker_status_idx == -1)
      {
        std::cout << "ไม่เลือกตัวโจมตี หรือข้ามการโจมตีครั้งนี้" << std::endl;
        break;
      }

      std::optional<Card> attacker_card_opt = currentPlayer->getUnitAtStatusIndex(attacker_status_idx);
      if (!attacker_card_opt.has_value())
      {
        printError("CRITICAL ERROR: ไม่พบการ์ด Attacker ที่เลือก");
        break;
      }
      ClearScreen();
      currentPlayer->displayField();
      printBoxedMessage("⚔️ " + currentPlayer->getName() + " เลือก " + attacker_card_opt.value().getName() + " เป็น Attacker. ⚔️", '!');

      int potential_booster_idx = currentPlayer->chooseBooster(attacker_status_idx);
      int booster_status_idx = -1;

      if (potential_booster_idx != -1)
      {
        char boost_confirm_choice = getActionInput("คุณต้องการ Boost หรือไม่? (y/n): ", currentPlayer); // แสดงสนามของ currentPlayer
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

      int current_attacker_power = currentPlayer->getUnitPowerAtStatusIndex(attacker_status_idx, booster_status_idx);
      std::cout << "🔥 พลังโจมตีของ " << attacker_card_opt.value().getName() << " (รวม Booster ถ้ามี): " << current_attacker_power << std::endl;

      // --- เลือกเป้าหมายการโจมตี (Targeting Opponent's VC for now) ---
      std::cout << "\n--- " << opponentPlayer->getName() << " เตรียมป้องกัน ---" << std::endl;
      opponentPlayer->displayField(); // แสดงสนามของฝ่ายตรงข้าม

      // สมมติว่าโจมตี VC ของฝ่ายตรงข้ามเสมอในขั้นนี้
      int target_unit_status_idx = UNIT_STATUS_VC_IDX; // โจมตี VC ของ opponent
      std::optional<Card> target_card_opt = opponentPlayer->getUnitAtStatusIndex(target_unit_status_idx);
      int opponent_target_base_power = 0;
      if (target_card_opt.has_value())
      {
        opponent_target_base_power = opponentPlayer->getUnitPowerAtStatusIndex(target_unit_status_idx, -1, true); // true for defense
        std::cout << "เป้าหมายคือ " << target_card_opt.value().getName() << " (Power: " << opponent_target_base_power << ")" << std::endl;
      }
      else
      {
        printError("ไม่พบ Vanguard ของฝ่ายตรงข้าม!"); // ควรจะไม่เกิดขึ้น
        break;
      }

      // --- (c) ให้คู่แข่ง Guard ---
      int total_shield_from_guard = opponentPlayer->performGuardStep(current_attacker_power, target_card_opt); // ส่ง target ไปด้วย
      int opponent_total_defense_power = opponent_target_base_power + total_shield_from_guard;
      std::cout << "🛡️ พลังป้องกันรวมของ " << opponentPlayer->getName() << " (รวม Guard): " << opponent_total_defense_power << std::endl;

      // --- (e) คำนวณ Hit/Miss ---
      bool is_hit = false;
      if (current_attacker_power >= opponent_total_defense_power)
      {
        printBoxedMessage("⚔️💥 การโจมตี HIT! 💥⚔️", '!');
        is_hit = true;
      }
      else
      {
        printBoxedMessage("🛡️ การโจมตี MISS! (ถูกป้องกันได้) 🛡️", '-');
      }

      opponentPlayer->clearGuardianZoneAndMoveToDrop(); // การ์ดที่ Guard จะไป Drop Zone

      // --- (f) ฝ่ายรับ Damage Check (ถ้า Hit) ---
      if (is_hit)
      {
        int critical_to_deal = 1; // สมมติ Critical = 1 สำหรับตอนนี้ (ยังไม่ดึงจาก Attacker)
        // TODO: ดึง Critical จาก attacker_card_opt.value().getCritical() (ต้องเพิ่มเมธอดนี้ใน Card)
        // และบวก Critical จาก Trigger effects ถ้ามี

        printSectionHeader(opponentPlayer->getName() + " ทำการ Damage Check " + std::to_string(critical_to_deal) + " ครั้ง", '~');
        for (int i = 0; i < critical_to_deal; ++i)
        {
          if (opponentPlayer->getDeck().isEmpty())
          {
            printBoxedMessage("GAME OVER: " + opponentPlayer->getName() + " แพ้เพราะไม่สามารถทำ Damage Check ได้ (เด็คหมด)! 💀", '!');
            game_over = true;
            break;
          }
          std::optional<Card> damage_card_opt = opponentPlayer->getDeck().draw();
          if (damage_card_opt.has_value())
          {
            opponentPlayer->takeDamage(damage_card_opt.value());
            // (ยังไม่ได้ทำ Trigger จาก Damage Check)
            if (opponentPlayer->getDamageCount() >= 6)
            {
              game_over = true; // Player::takeDamage จะแสดงข้อความแพ้
              break;
            }
          }
        }
        if (game_over)
          break; // ออกจาก loop โจมตีถ้าเกมจบ
      }

      currentPlayer->restUnit(attacker_status_idx);
      if (booster_status_idx != -1)
      {
        currentPlayer->restUnit(booster_status_idx);
      }
      std::cout << attacker_card_opt.value().getName() << " และ Booster (ถ้ามี) เข้าสู่สถานะ Rest." << std::endl;

      if (game_over)
        break; // ออกจาก loop โจมตีถ้าเกมจบ

      attack_again_choice = getActionInput("\nคุณ (" + currentPlayer->getName() + ") ต้องการโจมตีอีกครั้งหรือไม่? (y/n): ", currentPlayer);
    }
    std::cout << "จบ Battle Phase ของ " << currentPlayer->getName() << std::endl;

    printSectionHeader(currentPlayer->getName() + ": END PHASE 🌙", '-');
    currentPlayer->clearGuardianZoneAndMoveToDrop();  // เคลียร์ Guardian Zone ของ CurrentPlayer ด้วย (ถ้ามีการใช้ Intercept ในอนาคต)
    opponentPlayer->clearGuardianZoneAndMoveToDrop(); // เผื่อกรณีที่การโจมตีครั้งสุดท้ายถูกยกเลิกก่อนเคลียร์
    std::cout << "(Placeholder: สกิลจบเทิร์น, ล้างสถานะชั่วคราว)" << std::endl;

    ClearScreen();
    printSectionHeader("เทิร์นของ " + currentPlayer->getName() + " สิ้นสุด", '*');
    std::cout << "(กด Enter เพื่อให้ผู้เล่นถัดไปเริ่มเทิร์น...)" << std::endl;
    std::cin.get();

    if (currentPlayer->getDamageCount() >= 6)
    {
      // Player::takeDamage จะแสดงข้อความแพ้แล้ว
      game_over = true;
    }
    if (opponentPlayer->getDamageCount() >= 6 && !game_over)
    {
      // Player::takeDamage จะแสดงข้อความแพ้แล้ว
      game_over = true;
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
    if (player1.getDamageCount() >= 6 && player2.getDamageCount() < 6)
      printBoxedMessage(player2.getName() + " เป็นผู้ชนะ! 🎉", '+');
    else if (player2.getDamageCount() >= 6 && player1.getDamageCount() < 6)
      printBoxedMessage(player1.getName() + " เป็นผู้ชนะ! 🎉", '+');
    else
      printBoxedMessage("เกมจบลง! (อาจจะเสมอ หรือเงื่อนไขอื่นๆ)", '*'); // กรณี Deck out หรือเงื่อนไขอื่น
  }
  else
  {
    std::cout << "จบการจำลอง " << turn_count << " เทิร์น" << std::endl;
  }
  return 0;
}
