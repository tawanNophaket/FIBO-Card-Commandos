// main_improved.cpp - Main file ที่ปรับปรุงแล้วด้วย UI/UX ใหม่
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <optional>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "UIHelper.h"
#include "MenuSystem.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// --- Helper Functions ---
vector<Card> loadCardsFromJson(const string &filename)
{
  vector<Card> all_cards;
  ifstream file_stream(filename);
  if (!file_stream.is_open())
  {
    UIHelper::PrintError("เปิดไฟล์ JSON '" + filename + "' ไม่ได้");
    return all_cards;
  }

  json card_data_json;
  try
  {
    file_stream >> card_data_json;
  }
  catch (json::parse_error &e)
  {
    UIHelper::PrintError("Parse JSON ผิดพลาด: " + string(e.what()));
    file_stream.close();
    return all_cards;
  }
  file_stream.close();

  if (!card_data_json.is_array())
  {
    UIHelper::PrintError("ข้อมูล JSON ไม่ได้อยู่ในรูปแบบ Array");
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
      UIHelper::PrintWarning("Type error ขณะอ่าน JSON object: " + string(e.what()));
    }
  }

  UIHelper::PrintSuccess("โหลดข้อมูลการ์ด " + to_string(all_cards.size()) + " ใบเรียบร้อย");
  return all_cards;
}

int chooseTargetFromOpponent(Player *attacker, Player *defender)
{
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("เลือกเป้าหมายการโจมตี", Icons::TARGET);

  attacker->displayField();
  cout << "\n"
       << Colors::BRIGHT_RED << "--- สนามของฝ่ายป้องกัน ("
       << defender->getName() << ") ---" << Colors::RESET << "\n";
  defender->displayField(true);

  vector<pair<int, string>> available_targets;
  if (defender->getVanguard().has_value())
  {
    available_targets.push_back({UNIT_STATUS_VC_IDX, Icons::CROWN + " VC: " + defender->getVanguard().value().getName()});
  }

  const auto &opp_rcs = defender->getRearGuards();
  const size_t opp_front_row_rcs_indices[] = {RC_FRONT_LEFT, RC_FRONT_RIGHT};
  for (size_t rc_idx : opp_front_row_rcs_indices)
  {
    if (opp_rcs[rc_idx].has_value())
    {
      string rc_name = (rc_idx == RC_FRONT_LEFT) ? "FL" : "FR";
      available_targets.push_back({(int)defender->getUnitStatusIndexForRC(rc_idx),
                                   Icons::SWORD + " RC " + rc_name + ": " + opp_rcs[rc_idx].value().getName()});
    }
  }

  if (available_targets.empty())
  {
    UIHelper::PrintError("ฝ่ายตรงข้ามไม่มียูนิตให้โจมตี!");
    MenuSystem::WaitForKeyPress();
    return -1;
  }

  cout << "\n"
       << Colors::BRIGHT_YELLOW << Icons::TARGET << " เป้าหมายที่โจมตีได้:" << Colors::RESET << "\n";
  for (size_t i = 0; i < available_targets.size(); ++i)
  {
    cout << Colors::CYAN << "[" << i << "] " << Colors::RESET << available_targets[i].second << "\n";
  }

  int choice_idx = MenuSystem::GetIntegerInput("เลือกหมายเลขเป้าหมาย: ", 0, available_targets.size() - 1);
  return available_targets[static_cast<size_t>(choice_idx)].first;
}

bool performRidePhase(Player *current_player)
{
  while (true)
  {
    MenuResult ride_result = MenuSystem::ShowRideMenu(current_player);

    if (!ride_result.is_valid || ride_result.selected_key.empty())
    {
      UIHelper::PrintInfo("ข้าม Ride Phase");
      return false;
    }

    if (ride_result.should_exit)
    {
      return true; // Exit game
    }

    int card_idx = stoi(ride_result.selected_key);
    if (card_idx == -1)
    {
      UIHelper::PrintInfo("ยกเลิกการ Ride ในเทิร์นนี้");
      return false;
    }

    if (current_player->rideFromHand(static_cast<size_t>(card_idx)))
    {
      UIHelper::PrintSuccess("Ride สำเร็จ!");
      MenuSystem::WaitForKeyPress();
      return false;
    }
    else
    {
      UIHelper::PrintError("ไม่สามารถ Ride การ์ดที่เลือกได้ (ตรวจสอบเกรด)");
      MenuSystem::WaitForKeyPress();
    }
  }
}

bool performMainPhase(Player *current_player)
{
  while (true)
  {
    MenuResult main_result = MenuSystem::ShowMainPhaseMenu(current_player);

    if (!main_result.is_valid)
    {
      continue; // Show menu again
    }

    if (main_result.should_exit)
    {
      return true; // Exit game
    }

    if (main_result.selected_key == "1") // Ride
    {
      if (performRidePhase(current_player))
        return true; // Exit game
    }
    else if (main_result.selected_key == "2") // Call
    {
      MenuResult call_result = MenuSystem::ShowCallMenu(current_player);
      if (call_result.is_valid && !call_result.selected_key.empty())
      {
        if (call_result.should_exit)
          return true;

        size_t comma_pos = call_result.selected_key.find(',');
        if (comma_pos != string::npos)
        {
          int card_idx = stoi(call_result.selected_key.substr(0, comma_pos));
          int rc_idx = stoi(call_result.selected_key.substr(comma_pos + 1));

          if (current_player->callToRearGuard(static_cast<size_t>(card_idx), static_cast<size_t>(rc_idx)))
          {
            UIHelper::PrintSuccess("Call สำเร็จ!");
          }
          else
          {
            UIHelper::PrintError("ไม่สามารถ Call การ์ดลงตำแหน่งที่เลือกได้");
          }
          MenuSystem::WaitForKeyPress();
        }
      }
    }
    else if (main_result.selected_key == "3") // Show hand
    {
      current_player->displayHand(true);
      MenuSystem::WaitForKeyPress();
    }
    else if (main_result.selected_key == "4") // Show field overview
    {
      MenuSystem::ShowFieldOverview(current_player, nullptr, current_player);
    }
    else if (main_result.selected_key == "5") // Go to Battle Phase
    {
      UIHelper::PrintInfo("ไปยัง Battle Phase");
      return false;
    }
    else if (main_result.selected_key == "h") // Help
    {
      MenuSystem::ShowGameHelp("MAIN PHASE");
    }
    else if (main_result.selected_key == "s") // Status
    {
      // Already handled in ShowMenu
      continue;
    }
  }
}

bool performBattlePhase(Player *current_player, Player *opponent_player, bool &game_over)
{
  current_player->clearGuardianZoneAndMoveToDrop();
  opponent_player->clearGuardianZoneAndMoveToDrop();

  while (true)
  {
    MenuResult battle_result = MenuSystem::ShowBattlePhaseMenu(current_player);

    if (!battle_result.is_valid)
    {
      continue;
    }

    if (battle_result.should_exit)
    {
      return true; // Exit game
    }

    if (battle_result.selected_key == "1") // Choose Attacker
    {
      MenuResult attacker_result = MenuSystem::ShowAttackerMenu(current_player);

      if (!attacker_result.is_valid || attacker_result.selected_key.empty())
      {
        continue;
      }

      if (attacker_result.should_exit)
        return true;

      int attacker_status_idx = stoi(attacker_result.selected_key);
      optional<Card> attacker_card_opt = current_player->getUnitAtStatusIndex(attacker_status_idx);

      if (!attacker_card_opt.has_value())
      {
        UIHelper::PrintError("ไม่พบการ์ด Attacker ที่เลือก");
        continue;
      }

      // Choose target
      int target_status_idx = chooseTargetFromOpponent(current_player, opponent_player);
      if (target_status_idx == -1)
        continue;

      optional<Card> target_card_opt = opponent_player->getUnitAtStatusIndex(target_status_idx);
      if (!target_card_opt.has_value())
      {
        UIHelper::PrintError("ไม่พบเป้าหมายที่เลือก");
        continue;
      }

      UIHelper::ClearScreen();
      UIHelper::PrintSectionHeader("BATTLE DECLARATION", Icons::SWORD, Colors::BRIGHT_RED);
      cout << Colors::BRIGHT_CYAN << Icons::SWORD << " Attacker: " << Colors::BOLD
           << attacker_card_opt.value().getName() << Colors::RESET << "\n";
      cout << Colors::BRIGHT_YELLOW << Icons::TARGET << " Target: " << Colors::BOLD
           << target_card_opt.value().getName() << Colors::RESET << "\n\n";

      // Choose booster
      int potential_booster_idx = current_player->chooseBooster(attacker_status_idx);
      int booster_status_idx = -1;

      if (potential_booster_idx != -1)
      {
        if (MenuSystem::ShowYesNoPrompt("คุณต้องการ Boost หรือไม่?"))
        {
          booster_status_idx = potential_booster_idx;
          optional<Card> booster_card_opt = current_player->getUnitAtStatusIndex(booster_status_idx);
          if (booster_card_opt.has_value())
          {
            cout << Colors::BRIGHT_GREEN << Icons::MAGIC << " Booster: "
                 << booster_card_opt.value().getName() << Colors::RESET << "\n";
          }
        }
      }

      int final_attacker_power = current_player->getUnitPowerAtStatusIndex(attacker_status_idx, booster_status_idx);
      int final_attacker_crit = attacker_card_opt.value().getCritical();
      TriggerOutput drive_trigger_effects;

      // Drive Check for VG attack
      if (attacker_status_idx == UNIT_STATUS_VC_IDX)
      {
        UIHelper::ClearScreen();
        current_player->displayField();
        int num_drives = (attacker_card_opt.value().getGrade() >= 3) ? 2 : 1;
        drive_trigger_effects = current_player->performDriveCheck(num_drives, opponent_player);
        final_attacker_power += drive_trigger_effects.extra_power;
        final_attacker_crit += drive_trigger_effects.extra_crit;
        MenuSystem::WaitForKeyPress("กด Enter เพื่อให้ฝ่ายตรงข้ามป้องกัน...");
      }

      // Show final attack stats
      UIHelper::PrintBattleStats(attacker_card_opt.value().getName(), final_attacker_power,
                                 final_attacker_crit, target_card_opt.value().getName(),
                                 opponent_player->getUnitPowerAtStatusIndex(target_status_idx, -1, true));

      // Guard Phase
      int total_shield_from_guard = 0;
      if (MenuSystem::ShowYesNoPrompt("คุณ (" + opponent_player->getName() + ") ต้องการ Guard หรือไม่?"))
      {
        total_shield_from_guard = opponent_player->performGuardStep(final_attacker_power, target_card_opt);
      }

      // Calculate battle result
      int opponent_target_base_power = opponent_player->getUnitPowerAtStatusIndex(target_status_idx, -1, true);
      int opponent_total_defense_power = opponent_target_base_power + total_shield_from_guard;

      bool is_hit = final_attacker_power >= opponent_total_defense_power;

      UIHelper::ClearScreen();
      UIHelper::PrintBattleStats(attacker_card_opt.value().getName(), final_attacker_power,
                                 final_attacker_crit, target_card_opt.value().getName(),
                                 opponent_total_defense_power);

      opponent_player->clearGuardianZoneAndMoveToDrop();

      if (is_hit)
      {
        // Damage Check
        UIHelper::PrintSectionHeader(opponent_player->getName() + ": DAMAGE CHECK x" + to_string(final_attacker_crit),
                                     Icons::DAMAGE, Colors::BRIGHT_RED);

        for (int i = 0; i < final_attacker_crit && !game_over; ++i)
        {
          UIHelper::ClearScreen();
          opponent_player->displayField();
          cout << Colors::BRIGHT_RED << "Damage Check ครั้งที่ " << (i + 1)
               << "/" << final_attacker_crit << Colors::RESET << "\n";

          if (opponent_player->getDeck().isEmpty())
          {
            UIHelper::ClearScreen();
            MenuSystem::ShowGameOverScreen(current_player, opponent_player,
                                           opponent_player->getName() + " ไม่สามารถทำ Damage Check ได้ (เด็คหมด)");
            game_over = true;
            break;
          }

          optional<Card> damage_card_opt = opponent_player->getDeck().draw();
          if (damage_card_opt.has_value())
          {
            Card actual_damage_card = damage_card_opt.value();
            cout << Colors::BRIGHT_CYAN << "เปิดได้: " << Colors::RESET
                 << UIHelper::FormatCard(actual_damage_card.getName(), actual_damage_card.getGrade()) << "\n";

            opponent_player->takeDamage(actual_damage_card);

            TriggerOutput dmg_trigger_effects = opponent_player->handleDamageCheckTrigger(actual_damage_card, current_player);
            if (dmg_trigger_effects.extra_power > 0)
            {
              cout << Colors::BRIGHT_MAGENTA << "Damage Trigger Effect: +"
                   << dmg_trigger_effects.extra_power << " Power!" << Colors::RESET << "\n";
            }

            opponent_player->displayField();
            if (opponent_player->getDamageCount() >= 6)
            {
              MenuSystem::ShowGameOverScreen(current_player, opponent_player,
                                             opponent_player->getName() + " ได้รับ 6 ดาเมจ");
              game_over = true;
              break;
            }
          }

          if (!game_over && i < final_attacker_crit - 1)
          {
            MenuSystem::WaitForKeyPress("กด Enter เพื่อดูดาเมจต่อไป...");
          }
        }
      }

      // Rest units
      current_player->restUnit(attacker_status_idx);
      if (booster_status_idx != -1)
      {
        current_player->restUnit(booster_status_idx);
      }

      if (game_over)
        return false;

      MenuSystem::WaitForKeyPress("กด Enter เพื่อดำเนินการต่อ...");
    }
    else if (battle_result.selected_key == "2") // Show field overview
    {
      MenuSystem::ShowFieldOverview(current_player, opponent_player, current_player);
    }
    else if (battle_result.selected_key == "3") // End Battle Phase
    {
      UIHelper::PrintInfo("จบ Battle Phase");
      return false;
    }
    else if (battle_result.selected_key == "h") // Help
    {
      MenuSystem::ShowGameHelp("BATTLE PHASE");
    }
  }
}

int main()
{
  // Game Start
  MenuResult start_result = MenuSystem::ShowGameStartMenu();

  if (start_result.should_exit || start_result.selected_key == "3")
  {
    cout << Colors::BRIGHT_CYAN << "ไว้พบกันใหม่โอกาสหน้า! " << Icons::EXIT << Colors::RESET << endl;
    return 0;
  }

  if (start_result.selected_key == "2")
  {
    MenuSystem::ShowGameHelp();
    return main(); // Restart
  }

  // Load cards
  UIHelper::ClearScreen();
  UIHelper::ShowLoadingAnimation("กำลังโหลดข้อมูลการ์ด...", 1500);

  vector<Card> full_card_list = loadCardsFromJson("cards.json");
  if (full_card_list.empty())
  {
    UIHelper::PrintError("ไม่สามารถโหลดฐานข้อมูลการ์ดได้ โปรแกรมจะจบการทำงาน");
    MenuSystem::WaitForKeyPress();
    return 1;
  }

  // Get player names
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("PLAYER SETUP", Icons::PLAYER);
  string p1_name = MenuSystem::GetPlayerName("👤 ใส่ชื่อผู้เล่น 1: ");
  string p2_name = MenuSystem::GetPlayerName("👤 ใส่ชื่อผู้เล่น 2: ");

  // Create decks and players
  map<string, int> deck_recipe_v1_3 = {
      {"G0-01", 1}, {"G0-02", 4}, {"G0-03", 4}, {"G0-04", 8}, {"G1-01", 3}, {"G1-02", 4}, {"G1-03", 3}, {"G1-04", 2}, {"G1-05", 1}, {"G2-01", 3}, {"G2-02", 3}, {"G2-03", 3}, {"G2-04", 2}, {"G3-01", 2}, {"G3-02", 2}, {"G3-03", 2}, {"G3-04", 2}, {"G4-01", 1}};

  UIHelper::ShowLoadingAnimation("กำลังสร้างเด็ค...", 1000);

  Player player1(p1_name, Deck(full_card_list, deck_recipe_v1_3));
  Player player2(p2_name, Deck(full_card_list, deck_recipe_v1_3));

  // Setup game
  if (!player1.setupGame("G0-01") || !player2.setupGame("G0-01"))
  {
    UIHelper::PrintError("ไม่สามารถตั้งค่าเกมได้");
    return 1;
  }

  // Choose first player
  UIHelper::ClearScreen();
  UIHelper::PrintSectionHeader("GAME START", Icons::CONFIRM);
  cout << Colors::BRIGHT_CYAN << "ใครจะเริ่มเล่นก่อน?" << Colors::RESET << "\n";
  cout << Colors::YELLOW << "[1] " << p1_name << Colors::RESET << "\n";
  cout << Colors::YELLOW << "[2] " << p2_name << Colors::RESET << "\n";

  int first_player_choice = MenuSystem::GetIntegerInput("เลือก: ", 1, 2);

  Player *currentPlayer = (first_player_choice == 1) ? &player1 : &player2;
  Player *opponentPlayer = (first_player_choice == 1) ? &player2 : &player1;

  UIHelper::PrintSuccess(currentPlayer->getName() + " ได้เริ่มเล่นก่อน!");
  MenuSystem::WaitForKeyPress("กด Enter เพื่อเริ่มเกม...");

  // Main game loop
  int turn_count = 0;
  bool game_over = false;
  bool should_exit = false;

  while (!game_over && !should_exit && turn_count < 50)
  {
    turn_count++;

    // Turn Start
    MenuSystem::ShowTurnStartScreen(currentPlayer, turn_count);

    // Stand Phase
    currentPlayer->performStandPhase();

    // Draw Phase
    if (!currentPlayer->performDrawPhase())
    {
      MenuSystem::ShowGameOverScreen(opponentPlayer, currentPlayer,
                                     currentPlayer->getName() + " ไม่สามารถจั่วการ์ดได้ (เด็คหมด)");
      game_over = true;
      break;
    }

    UIHelper::ShowPhaseTransition("DRAW PHASE", "MAIN PHASE");

    // Main Phase
    should_exit = performMainPhase(currentPlayer);
    if (should_exit)
      break;

    UIHelper::ShowPhaseTransition("MAIN PHASE", "BATTLE PHASE");

    // Battle Phase
    should_exit = performBattlePhase(currentPlayer, opponentPlayer, game_over);
    if (should_exit || game_over)
      break;

    // End Phase
    UIHelper::PrintSectionHeader(currentPlayer->getName() + ": END PHASE", Icons::PHASE);
    currentPlayer->clearGuardianZoneAndMoveToDrop();
    opponentPlayer->clearGuardianZoneAndMoveToDrop();

    // Check win conditions
    if (currentPlayer->getDamageCount() >= 6 || opponentPlayer->getDamageCount() >= 6)
    {
      Player *winner = (currentPlayer->getDamageCount() < 6) ? currentPlayer : opponentPlayer;
      Player *loser = (currentPlayer->getDamageCount() >= 6) ? currentPlayer : opponentPlayer;
      MenuSystem::ShowGameOverScreen(winner, loser, "ดาเมจครบ 6 แต้ม");
      game_over = true;
      break;
    }

    UIHelper::PrintInfo("เทิร์นของ " + currentPlayer->getName() + " สิ้นสุด");
    MenuSystem::WaitForKeyPress("กด Enter เพื่อให้ผู้เล่นถัดไปเริ่มเทิร์น...");

    // Switch players
    swap(currentPlayer, opponentPlayer);
  }

  if (!should_exit && !game_over)
  {
    UIHelper::PrintInfo("จบการจำลอง " + to_string(turn_count) + " เทิร์น");
  }

  if (!should_exit)
  {
    cout << "\n"
         << Colors::BRIGHT_MAGENTA << Icons::VICTORY
         << " ขอบคุณที่เล่น FIBO Card Commandos! " << Icons::VICTORY << Colors::RESET << "\n";
    MenuSystem::WaitForKeyPress("กด Enter เพื่อออกจากโปรแกรม...");
  }

  return 0;
}