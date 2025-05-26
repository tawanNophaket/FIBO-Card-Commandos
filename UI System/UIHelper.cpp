// UIHelper.cpp - Implementation ของ UI Helper functions
#include "UIHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cwchar>
#include <locale>
#include <cstring>
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

// Helper function to repeat a character n times
string UIHelper::RepeatChar(char c, int n) {
    return string(n, c);
}

// Helper function to repeat a string n times (used for multi-byte characters like box drawing)
string UIHelper::RepeatChar(const string& s, int n) {
    string result = "";
    for (int i = 0; i < n; ++i) {
        result += s;
    }
    return result;
}

// Box Drawing
void UIHelper::PrintHorizontalLine(char c, int length, const string &color)
{
  cout << color << RepeatChar(c, length) << Colors::RESET << endl;
}

void UIHelper::PrintHorizontalLine(const string& s, int length, const string &color)
{
    cout << color << RepeatChar(s, length) << Colors::RESET << endl;
}

void UIHelper::PrintBox(const string &content, const string &title,
                        const string &border_color, const string &text_color)
{
  int content_length = GetDisplayWidth(content);
  int title_length = GetDisplayWidth(title);
  int box_width = max(content_length + 4, title_length + 4);
  box_width = max(box_width, 50); // minimum width

  // Top border
  cout << border_color << "╔" << RepeatChar("═", box_width - 2) << "╗" << Colors::RESET << endl;

  // Title (if provided)
  if (!title.empty())
  {
    int title_padding_total = box_width - GetDisplayWidth(title) - 2;
    int title_padding_left = title_padding_total / 2;
    int title_padding_right = title_padding_total - title_padding_left;
    cout << border_color << "║" << Colors::RESET;
    cout << RepeatChar(' ', title_padding_left) << Colors::BOLD << text_color << title << Colors::RESET;
    cout << RepeatChar(' ', title_padding_right);
    cout << border_color << "║" << Colors::RESET << endl;
    cout << border_color << "╠" << RepeatChar("═", box_width - 2) << "╣" << Colors::RESET << endl;
  }

  // Content
  int content_padding_total = box_width - GetDisplayWidth(content) - 2;
  int content_padding_left = content_padding_total / 2;
  int content_padding_right = content_padding_total - content_padding_left;
  cout << border_color << "║" << Colors::RESET;
  cout << RepeatChar(' ', content_padding_left) << text_color << content << Colors::RESET;
  cout << RepeatChar(' ', content_padding_right);
  cout << border_color << "║" << Colors::RESET << endl;

  // Bottom border
  cout << border_color << "╚" << RepeatChar("═", box_width - 2) << "╝" << Colors::RESET << endl;
}

void UIHelper::PrintSectionHeader(const string &title, const string &icon, const string &color)
{
  cout << "\n";
  PrintHorizontalLine("═", 70, color);
  string header_text = icon + " " + title + " " + icon;
  int display_width_header = GetDisplayWidth(header_text);
  int padding = (70 - display_width_header) / 2;
  padding = max(0, padding); // Ensure padding is not negative
  cout << color << RepeatChar(' ', padding) << header_text << Colors::RESET << endl;
  PrintHorizontalLine("═", 70, color);
}

void UIHelper::PrintPhaseHeader(const string &phase_name, const string &player_name,
                                int turn_number, const string &color)
{
  string turn_info = turn_number > 0 ? " - TURN " + to_string(turn_number) : "";
  string full_header = Icons::PHASE + " " + phase_name + " | " + Icons::PLAYER + " " + player_name + turn_info;

  cout << "\n";
  PrintHorizontalLine("─", 70, color);
  int display_width_header = GetDisplayWidth(full_header);
  int padding = (70 - display_width_header) / 2;
  if (padding > 0) {
      cout << color << RepeatChar(' ', padding) << Colors::BOLD << full_header << Colors::RESET << endl;
  } else {
      cout << color << Colors::BOLD << full_header << Colors::RESET << endl;
  }
  PrintHorizontalLine("─", 70, color);
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
  PrintHorizontalLine("█", 70, Colors::BRIGHT_MAGENTA);
  cout << Colors::BRIGHT_MAGENTA << Colors::BOLD;
  cout << "███████ ██ ██████   ██████      ██████  █████  ██████  ██████  " << endl;
  cout << "██      ██ ██   ██ ██    ██    ██      ██   ██ ██   ██ ██   ██ " << endl;
  cout << "█████   ██ ██████  ██    ██    ██      ███████ ██████  ██   ██ " << endl;
  cout << "██      ██ ██   ██ ██    ██    ██      ██   ██ ██   ██ ██   ██ " << endl;
  cout << "██      ██ ██████   ██████      ██████ ██   ██ ██   ██ ██████  " << endl;
  cout << Colors::RESET;
  PrintHorizontalLine("█", 70, Colors::BRIGHT_MAGENTA);
  cout << Colors::BRIGHT_CYAN << Colors::BOLD;
  string title_text = "                   ⚔️ COMMANDOS BATTLE SYSTEM ⚔️";
  int title_padding = (70 - GetDisplayWidth(title_text)) / 2;
  title_padding = max(0, title_padding);
  cout << RepeatChar(' ', title_padding) << title_text << endl;
  cout << Colors::RESET;
  PrintHorizontalLine("█", 70, Colors::BRIGHT_MAGENTA);
}

void UIHelper::PrintQuickStatus(const string &player_name, int hand_size, int deck_size,
                                int damage_count, int soul_count, int drop_count)
{
  cout << "\n";
  PrintHorizontalLine("─", 70, Colors::BRIGHT_BLACK);
  cout << Colors::BRIGHT_CYAN << Icons::PLAYER << " " << player_name << Colors::RESET;
  cout << " | " << Colors::YELLOW << Icons::HAND << " " << hand_size;
  cout << " | " << Colors::BLUE << Icons::DECK << " " << deck_size;
  cout << " | " << Colors::RED << Icons::DAMAGE << " " << damage_count << "/6";
  cout << " | " << Colors::MAGENTA << Icons::SOUL << " " << soul_count;
  cout << " | " << Colors::BRIGHT_BLACK << Icons::DROP << " " << drop_count << Colors::RESET << endl;
  PrintHorizontalLine("─", 70, Colors::BRIGHT_BLACK);
}

void UIHelper::PrintShortcuts()
{
  cout << Colors::BRIGHT_BLACK << "🚀 SHORTCUTS: ";
  cout << "[h]elp [s]tatus [f]ield [ESC]back [Space]skip [Tab]switch" << Colors::RESET << endl;
}

// Progress/Animation
void UIHelper::PrintProgressBar(int current, int max_val, const string &label)
{
  int bar_width = 20;
  float progress = (max_val > 0) ? (float)current / max_val : 0;
  int filled = (int)(progress * bar_width);
  int empty = bar_width - filled;

  cout << label << " [";
  if (filled > 0) {
    cout << Colors::GREEN << RepeatChar("█", filled);
  }
  if (empty > 0) {
    cout << Colors::BRIGHT_BLACK << RepeatChar("░", empty);
  }
  cout << "] " << current << "/" << max_val << " (" << (int)(progress * 100) << "%)" << endl;
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
void UIHelper::PrintCardPreview(const std::string &name, int grade, int power, int shield,
                                int critical, const std::string &skill, const std::string &type)
{
  const int preview_width = 50;
  PrintHorizontalLine("─", preview_width, Colors::CYAN);
  std::cout << Colors::CYAN << "│" << Colors::BOLD << std::left << std::setw(preview_width - 2) << (" " + name) << Colors::RESET << Colors::CYAN << "│" << std::endl;
  PrintHorizontalLine("─", preview_width, Colors::CYAN);

  std::string grade_str = "Grade: " + GetGradeIcon(grade) + " " + std::to_string(grade);
  std::string type_str = "Type: " + type;
  std::cout << Colors::CYAN << "│ " << Colors::RESET << grade_str;
  std::cout << RepeatChar(' ', preview_width - GetDisplayWidth(Colors::CYAN + "│ " + Colors::RESET + grade_str + RepeatChar(' ',1) + type_str + Colors::CYAN + "│")) << type_str << " " << Colors::CYAN << "│" << std::endl;

  std::string power_str = "Power: " + Colors::GREEN + std::to_string(power) + Colors::RESET;
  std::string shield_str = "Shield: " + Colors::BLUE + std::to_string(shield) + Colors::RESET;
  std::cout << Colors::CYAN << "│ " << Colors::RESET << power_str;
  std::cout << RepeatChar(' ', preview_width - GetDisplayWidth(Colors::CYAN + "│ " + Colors::RESET + power_str + RepeatChar(' ',1) + shield_str + Colors::CYAN + "│")) << shield_str << " " << Colors::CYAN << "│" << std::endl;

  std::string critical_str = "Critical: " + Colors::RED + std::to_string(critical) + Colors::RESET;
  std::cout << Colors::CYAN << "│ " << Colors::RESET << critical_str << RepeatChar(' ', preview_width - GetDisplayWidth(Colors::CYAN + "│ " + Colors::RESET + critical_str + Colors::CYAN + "│")) << Colors::CYAN << "│" << std::endl;

  if (!skill.empty() && skill != "-") {
    PrintHorizontalLine("·", preview_width, Colors::BRIGHT_BLACK);
    std::string skill_label = Colors::YELLOW + Icons::MAGIC + " Skill: " + Colors::RESET;
    std::string current_skill_text = skill;
    bool first_line_of_skill = true;

    while (!current_skill_text.empty()) {
        std::cout << Colors::CYAN << "│ " << Colors::RESET;
        std::string line_prefix;
        if (first_line_of_skill) {
            line_prefix = skill_label;
            std::cout << line_prefix;
        } else {
            line_prefix = RepeatChar(' ', GetDisplayWidth(skill_label));
            std::cout << line_prefix; // Indent subsequent lines
        }

        int available_display_width = preview_width - GetDisplayWidth(Colors::CYAN + "│ " + Colors::RESET + line_prefix + Colors::CYAN + "│");
        
        std::string segment_to_print;
        // int chars_can_fit_approx = available_display_width; // Approximation (REMOVED as unused)
        
        if (GetDisplayWidth(current_skill_text) <= available_display_width) {
            segment_to_print = current_skill_text;
            current_skill_text.clear();
        } else {
            // Iterate to find how many actual characters fit
            int actual_chars_fit = 0;
            for (size_t i = 0; i < current_skill_text.length(); ++i) { // Changed i to size_t
                if (GetDisplayWidth(current_skill_text.substr(0, i + 1)) <= available_display_width) {
                    actual_chars_fit = i + 1;
                } else {
                    break;
                }
            }
            // Try to break at a space if possible within actual_chars_fit
            int break_at = -1;
            // Ensure actual_chars_fit -1 is not negative before loop
            for (int i = actual_chars_fit > 0 ? actual_chars_fit - 1 : 0; i >=0; --i) { 
                if (static_cast<size_t>(i) < current_skill_text.length() && current_skill_text[static_cast<size_t>(i)] == ' ') {
                    break_at = i;
                    break;
                }
            }

            if (break_at != -1) {
                segment_to_print = current_skill_text.substr(0, break_at);
                current_skill_text = current_skill_text.substr(break_at + 1); // Skip space
            } else { // No space, or word is too long, so break at actual_chars_fit
                segment_to_print = current_skill_text.substr(0, actual_chars_fit);
                current_skill_text = current_skill_text.substr(actual_chars_fit);
            }
        }
        std::cout << segment_to_print;
        // Calculate padding for the right side of the current line
        int current_line_content_display_width = GetDisplayWidth(line_prefix + segment_to_print);
        std::cout << RepeatChar(' ', preview_width - GetDisplayWidth(Colors::CYAN + "│ " + Colors::RESET) - current_line_content_display_width - GetDisplayWidth(Colors::CYAN + "│")) << Colors::CYAN << "│" << std::endl;
        first_line_of_skill = false;
    } // End of while loop for skill wrapping
  } // End of if skill not empty
  PrintHorizontalLine("─", preview_width, Colors::CYAN);
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

// Helper to estimate display width of a UTF-8 string (works on Windows, no wcwidth)
int UIHelper::GetDisplayWidth(const std::string &str)
{
  int width = 0;
  std::mbstate_t state = std::mbstate_t();
  const char *ptr = str.c_str();
  size_t len = str.length();
  wchar_t wc;
  size_t i = 0;
  while (i < len)
  {
    size_t bytes = std::mbrtowc(&wc, ptr + i, len - i, &state);
    if (bytes == 0 || bytes == (size_t)-1 || bytes == (size_t)-2)
    {
      ++width;
      ++i;
      memset(&state, 0, sizeof(state)); // Reset state on error
      continue;
    }
    // More robust check for wide characters, this is still a simplification
    // and might not be perfect for all Unicode characters or terminals.
    // For truly accurate width, a library like wcwidth() is needed, but not standard in C++ Windows without specific setup.
    if (wc > 127) { // A very basic check: if not ASCII, assume width 2. This is often incorrect.
                    // The previous complex check had issues with compiler warnings about always true/false comparisons.
                    // This simplification might lead to misalignments for some characters.
        width += 2;
    } else {
        width += 1;
    }
    i += bytes;
  }
  return width;
}