// UIHelper.cpp - Implementation ของ UI Helper functions
#include "UIHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

using namespace std;

// Screen Management
void UIHelper::ClearScreen()
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
  system("clear");
#endif
}

void UIHelper::PauseForUser(const string &message)
{
  cout << "\n"
       << Colors::BRIGHT_BLACK << Icons::INFO << " " << message << Colors::RESET << endl;
  cin.get();
}

void UIHelper::ShowLoadingAnimation(const string &message, int duration_ms)
{
  cout << "\n"
       << Colors::BRIGHT_CYAN << message << " ";
  cout.flush();

  string spinner = "|/-\\";
  int steps = duration_ms / 100;

  for (int i = 0; i < steps; i++)
  {
    cout << "\b" << spinner[i % 4];
    cout.flush();
    this_thread::sleep_for(chrono::milliseconds(100));
  }
  cout << "\b " << Icons::CONFIRM << Colors::RESET << endl;
}

// Box Drawing
void UIHelper::PrintHorizontalLine(char c, int length, const string &color)
{
  cout << color << string(length, c) << Colors::RESET << endl;
}

void UIHelper::PrintBox(const string &content, const string &title,
                        const string &border_color, const string &text_color)
{
  int content_length = content.length();
  int title_length = title.length();
  int box_width = max(content_length + 4, title_length + 4);
  box_width = max(box_width, 50); // minimum width

  // Top border
  cout << border_color << "╔" << string(box_width - 2, '═') << "╗" << Colors::RESET << endl;

  // Title (if provided)
  if (!title.empty())
  {
    int title_padding = (box_width - title_length - 2) / 2;
    cout << border_color << "║" << Colors::RESET;
    cout << string(title_padding, ' ') << Colors::BOLD << text_color << title << Colors::RESET;
    cout << string(box_width - title_length - title_padding - 2, ' ');
    cout << border_color << "║" << Colors::RESET << endl;
    cout << border_color << "╠" << string(box_width - 2, '═') << "╣" << Colors::RESET << endl;
  }

  // Content
  int content_padding = (box_width - content_length - 2) / 2;
  cout << border_color << "║" << Colors::RESET;
  cout << string(content_padding, ' ') << text_color << content << Colors::RESET;
  cout << string(box_width - content_length - content_padding - 2, ' ');
  cout << border_color << "║" << Colors::RESET << endl;

  // Bottom border
  cout << border_color << "╚" << string(box_width - 2, '═') << "╝" << Colors::RESET << endl;
}

void UIHelper::PrintSectionHeader(const string &title, const string &icon, const string &color)
{
  cout << "\n";
  PrintHorizontalLine('═', 70, color);
  string header_text = icon + " " + title + " " + icon;
  int padding = (70 - header_text.length()) / 2;
  cout << color << string(padding, ' ') << header_text << Colors::RESET << endl;
  PrintHorizontalLine('═', 70, color);
}

void UIHelper::PrintPhaseHeader(const string &phase_name, const string &player_name,
                                int turn_number, const string &color)
{
  string turn_info = turn_number > 0 ? " - TURN " + to_string(turn_number) : "";
  string full_header = Icons::PHASE + " " + phase_name + " | " + Icons::PLAYER + " " + player_name + turn_info;

  cout << "\n";
  PrintHorizontalLine('─', 70, color);
  cout << color << Colors::BOLD << full_header << Colors::RESET << endl;
  PrintHorizontalLine('─', 70, color);
}

// Message Types
void UIHelper::PrintSuccess(const string &message)
{
  cout << "\n"
       << Colors::BRIGHT_GREEN << Icons::CONFIRM << " SUCCESS: " << message << Colors::RESET << endl;
}

void UIHelper::PrintError(const string &message)
{
  cout << "\n"
       << Colors::BRIGHT_RED << Icons::CANCEL << " ERROR: " << message << Colors::RESET << endl;
}

void UIHelper::PrintWarning(const string &message)
{
  cout << "\n"
       << Colors::BRIGHT_YELLOW << Icons::WARNING << " WARNING: " << message << Colors::RESET << endl;
}

void UIHelper::PrintInfo(const string &message)
{
  cout << "\n"
       << Colors::BRIGHT_CYAN << Icons::INFO << " INFO: " << message << Colors::RESET << endl;
}

// Game Status
void UIHelper::PrintGameTitle()
{
  ClearScreen();
  cout << "\n";
  PrintHorizontalLine('█', 70, Colors::BRIGHT_MAGENTA);
  cout << Colors::BRIGHT_MAGENTA << Colors::BOLD;
  cout << "███████ ██ ██████   ██████      ██████  █████  ██████  ██████  " << endl;
  cout << "██      ██ ██   ██ ██    ██    ██      ██   ██ ██   ██ ██   ██ " << endl;
  cout << "█████   ██ ██████  ██    ██    ██      ███████ ██████  ██   ██ " << endl;
  cout << "██      ██ ██   ██ ██    ██    ██      ██   ██ ██   ██ ██   ██ " << endl;
  cout << "██      ██ ██████   ██████      ██████ ██   ██ ██   ██ ██████  " << endl;
  cout << Colors::RESET;
  PrintHorizontalLine('█', 70, Colors::BRIGHT_MAGENTA);
  cout << Colors::BRIGHT_CYAN << Colors::BOLD;
  cout << "                    ⚔️ COMMANDOS BATTLE SYSTEM ⚔️" << endl;
  cout << Colors::RESET;
  PrintHorizontalLine('█', 70, Colors::BRIGHT_MAGENTA);
}

void UIHelper::PrintQuickStatus(const string &player_name, int hand_size, int deck_size,
                                int damage_count, int soul_count, int drop_count)
{
  cout << "\n";
  PrintHorizontalLine('─', 70, Colors::BRIGHT_BLACK);
  cout << Colors::BRIGHT_CYAN << Icons::PLAYER << " " << player_name << Colors::RESET;
  cout << " | " << Colors::YELLOW << Icons::HAND << " " << hand_size;
  cout << " | " << Colors::BLUE << Icons::DECK << " " << deck_size;
  cout << " | " << Colors::RED << Icons::DAMAGE << " " << damage_count << "/6";
  cout << " | " << Colors::MAGENTA << Icons::SOUL << " " << soul_count;
  cout << " | " << Colors::BRIGHT_BLACK << Icons::DROP << " " << drop_count << Colors::RESET << endl;
  PrintHorizontalLine('─', 70, Colors::BRIGHT_BLACK);
}

void UIHelper::PrintShortcuts()
{
  cout << Colors::BRIGHT_BLACK << "🚀 SHORTCUTS: ";
  cout << "[h]elp [s]tatus [f]ield [ESC]back [Space]skip [Tab]switch" << Colors::RESET << endl;
}

// Progress/Animation
void UIHelper::PrintProgressBar(int current, int max, const string &label)
{
  int bar_width = 20;
  float progress = (float)current / max;
  int filled = (int)(progress * bar_width);

  cout << label << " [";
  cout << Colors::GREEN << string(filled, '█') << Colors::RESET;
  cout << Colors::BRIGHT_BLACK << string(bar_width - filled, '░') << Colors::RESET;
  cout << "] " << current << "/" << max << " (" << (int)(progress * 100) << "%)" << endl;
}

void UIHelper::AnimateText(const string &text, int delay_ms)
{
  for (char c : text)
  {
    cout << c;
    cout.flush();
    this_thread::sleep_for(chrono::milliseconds(delay_ms));
  }
  cout << endl;
}

void UIHelper::ShowDriveCheckAnimation()
{
  cout << Colors::BRIGHT_YELLOW << Icons::TARGET << " Drive Check";
  for (int i = 0; i < 3; i++)
  {
    cout << ".";
    cout.flush();
    this_thread::sleep_for(chrono::milliseconds(400));
  }
  cout << " " << Icons::LIGHTNING << Colors::RESET << endl;
}

void UIHelper::ShowDamageAnimation()
{
  cout << Colors::BRIGHT_RED;
  for (int i = 0; i < 3; i++)
  {
    cout << Icons::DAMAGE << " ";
    cout.flush();
    this_thread::sleep_for(chrono::milliseconds(300));
  }
  cout << Colors::RESET << endl;
}

void UIHelper::ShowPhaseTransition(const string &from, const string &to)
{
  cout << "\n"
       << Colors::BRIGHT_CYAN << Icons::TURN << " " << from << " → " << to << Colors::RESET << endl;

  string progress = "▓▓▓▓▓▓▓▓▓▓";
  cout << Colors::GREEN << progress << " 100%" << Colors::RESET << endl;
  this_thread::sleep_for(chrono::milliseconds(800));
}

// Input Helpers
string UIHelper::ColorText(const string &text, const string &color)
{
  return color + text + Colors::RESET;
}

string UIHelper::FormatCard(const string &name, int grade, bool is_standing, bool show_icons)
{
  string result = "";

  if (show_icons)
  {
    result += GetGradeIcon(grade) + " ";
  }

  result += "G" + to_string(grade) + " " + name;

  if (!is_standing)
  {
    result += " " + Colors::BRIGHT_BLACK + "(REST)" + Colors::RESET;
  }

  return result;
}

string UIHelper::GetGradeIcon(int grade)
{
  switch (grade)
  {
  case 0:
    return GradeIcons::G0;
  case 1:
    return GradeIcons::G1;
  case 2:
    return GradeIcons::G2;
  case 3:
    return GradeIcons::G3;
  case 4:
    return GradeIcons::G4;
  default:
    return "❓";
  }
}

string UIHelper::GetStatusIcon(bool is_standing)
{
  return is_standing ? Icons::RUNNER : Icons::SLEEP;
}

// Card Display Helpers
string UIHelper::FormatPowerShield(int power, int shield)
{
  return Colors::RED + Icons::SWORD + to_string(power) + Colors::RESET +
         " " + Colors::BLUE + Icons::SHIELD + to_string(shield) + Colors::RESET;
}

string UIHelper::FormatCardShort(const string &code, const string &name, int grade)
{
  return GetGradeIcon(grade) + " [" + code + "] " + name;
}

// Layout Helpers
void UIHelper::PrintCardPreview(const string &name, int grade, int power, int shield,
                                int critical, const string &skill, const string &type)
{
  cout << "\n";
  PrintHorizontalLine('─', 50, Colors::CYAN);
  cout << Colors::CYAN << "│" << Colors::RESET;
  cout << "  " << Colors::BOLD << Icons::CARD << " CARD PREVIEW" << Colors::RESET;
  cout << string(29, ' ') << Colors::CYAN << "│" << Colors::RESET << endl;
  PrintHorizontalLine('─', 50, Colors::CYAN);

  cout << Colors::CYAN << "│" << Colors::RESET;
  cout << " " << GetGradeIcon(grade) << " " << Colors::BOLD << name << Colors::RESET;
  cout << " (" << type << ")";
  int padding1 = 50 - name.length() - type.length() - 8;
  cout << string(max(padding1, 0), ' ') << Colors::CYAN << "│" << Colors::RESET << endl;

  cout << Colors::CYAN << "│" << Colors::RESET;
  cout << " " << FormatPowerShield(power, shield);
  cout << " " << Colors::YELLOW << Icons::CRITICAL << critical << Colors::RESET;
  cout << string(20, ' ') << Colors::CYAN << "│" << Colors::RESET << endl;

  cout << Colors::CYAN << "│" << Colors::RESET;
  cout << " " << Colors::MAGENTA << Icons::MAGIC << " " << skill.substr(0, 40) << Colors::RESET;
  int padding2 = 50 - min(40, (int)skill.length()) - 5;
  cout << string(max(padding2, 0), ' ') << Colors::CYAN << "│" << Colors::RESET << endl;

  PrintHorizontalLine('─', 50, Colors::CYAN);
}

void UIHelper::PrintBattleStats(const string &attacker_name, int attack_power, int critical,
                                const string &defender_name, int defense_power)
{
  cout << "\n";
  PrintSectionHeader("BATTLE CALCULATION", Icons::SWORD, Colors::BRIGHT_RED);

  cout << Colors::RED << Icons::SWORD << " ATTACKER: " << Colors::BOLD << attacker_name << Colors::RESET;
  cout << " | Power: " << Colors::BRIGHT_RED << attack_power << Colors::RESET;
  cout << " | Critical: " << Colors::YELLOW << critical << Colors::RESET << endl;

  cout << Colors::BLUE << Icons::SHIELD << " DEFENDER: " << Colors::BOLD << defender_name << Colors::RESET;
  cout << " | Defense: " << Colors::BRIGHT_BLUE << defense_power << Colors::RESET << endl;

  cout << "\n"
       << Colors::BRIGHT_YELLOW;
  if (attack_power >= defense_power)
  {
    cout << Icons::CONFIRM << " RESULT: HIT! 💥" << Colors::RESET << endl;
  }
  else
  {
    cout << Icons::CANCEL << " RESULT: MISS! 🛡️" << Colors::RESET << endl;
  }
}