// UIHelper.h - ไฟล์ Header สำหรับ UI และการจัดรูปแบบ
#ifndef UIHELPER_H
#define UIHELPER_H

#include <string>
#include <iostream>
#include <thread>
#include <chrono>

// ANSI Color Codes
namespace Colors
{
  const std::string RESET = "\033[0m";
  const std::string BLACK = "\033[30m";
  const std::string RED = "\033[31m";
  const std::string GREEN = "\033[32m";
  const std::string YELLOW = "\033[33m";
  const std::string BLUE = "\033[34m";
  const std::string MAGENTA = "\033[35m";
  const std::string CYAN = "\033[36m";
  const std::string WHITE = "\033[37m";

  // Bright colors
  const std::string BRIGHT_BLACK = "\033[90m";
  const std::string BRIGHT_RED = "\033[91m";
  const std::string BRIGHT_GREEN = "\033[92m";
  const std::string BRIGHT_YELLOW = "\033[93m";
  const std::string BRIGHT_BLUE = "\033[94m";
  const std::string BRIGHT_MAGENTA = "\033[95m";
  const std::string BRIGHT_CYAN = "\033[96m";
  const std::string BRIGHT_WHITE = "\033[97m";

  // Text styles
  const std::string BOLD = "\033[1m";
  const std::string UNDERLINE = "\033[4m";
  const std::string ITALIC = "\033[3m";

  // Background colors
  const std::string BG_BLACK = "\033[40m";
  const std::string BG_RED = "\033[41m";
  const std::string BG_GREEN = "\033[42m";
  const std::string BG_YELLOW = "\033[43m";
  const std::string BG_BLUE = "\033[44m";
  const std::string BG_MAGENTA = "\033[45m";
  const std::string BG_CYAN = "\033[46m";
  const std::string BG_WHITE = "\033[47m";
}

// Unicode Icons
namespace Icons
{
  const std::string CARD = "🃏";
  const std::string SWORD = "⚔️";
  const std::string SHIELD = "🛡️";
  const std::string HEART = "💚";
  const std::string DIAMOND = "💎";
  const std::string STAR = "⭐";
  const std::string FIRE = "🔥";
  const std::string LIGHTNING = "⚡";
  const std::string CROWN = "👑";
  const std::string TARGET = "🎯";
  const std::string RUNNER = "🏃";
  const std::string SLEEP = "💤";
  const std::string MAGIC = "✨";
  const std::string HEAL = "💚";
  const std::string DRAW = "📝";
  const std::string CRITICAL = "💥";
  const std::string DAMAGE = "🩸";
  const std::string SKULL = "💀";
  const std::string VICTORY = "🏆";
  const std::string PLAYER = "👤";
  const std::string TURN = "🔄";
  const std::string PHASE = "⏰";
  const std::string MENU = "📋";
  const std::string HELP = "💡";
  const std::string EXIT = "🚪";
  const std::string NEXT = "▶️";
  const std::string BACK = "◀️";
  const std::string UP = "⬆️";
  const std::string DOWN = "⬇️";
  const std::string CONFIRM = "✅";
  const std::string CANCEL = "❌";
  const std::string WARNING = "⚠️";
  const std::string INFO = "ℹ️";
  const std::string DECK = "📚";
  const std::string HAND = "🤲";
  const std::string FIELD = "🏟️";
  const std::string SOUL = "👻";
  const std::string DROP = "🗑️";
  const std::string GUARD = "🛡️";
}

// Grade Icons
namespace GradeIcons
{
  const std::string G0 = "⚪";
  const std::string G1 = "🔵";
  const std::string G2 = "🟡";
  const std::string G3 = "🟠";
  const std::string G4 = "🔴";
}

class UIHelper
{
public:
  // Screen Management
  static void ClearScreen();
  static void PauseForUser(const std::string &message = "กด Enter เพื่อดำเนินการต่อ...");
  static void ShowLoadingAnimation(const std::string &message, int duration_ms = 1500);

  // Box Drawing
  static void PrintHorizontalLine(char c = '=', int length = 70, const std::string &color = Colors::CYAN);
  static void PrintBox(const std::string &content, const std::string &title = "",
                       const std::string &border_color = Colors::CYAN,
                       const std::string &text_color = Colors::WHITE);
  static void PrintSectionHeader(const std::string &title, const std::string &icon = Icons::STAR,
                                 const std::string &color = Colors::BRIGHT_YELLOW);
  static void PrintPhaseHeader(const std::string &phase_name, const std::string &player_name,
                               int turn_number = 0, const std::string &color = Colors::BRIGHT_CYAN);

  // Message Types
  static void PrintSuccess(const std::string &message);
  static void PrintError(const std::string &message);
  static void PrintWarning(const std::string &message);
  static void PrintInfo(const std::string &message);

  // Game Status
  static void PrintGameTitle();
  static void PrintQuickStatus(const std::string &player_name, int hand_size, int deck_size,
                               int damage_count, int soul_count, int drop_count);
  static void PrintShortcuts();

  // Progress/Animation
  static void PrintProgressBar(int current, int max, const std::string &label = "");
  static void AnimateText(const std::string &text, int delay_ms = 50);
  static void ShowDriveCheckAnimation();
  static void ShowDamageAnimation();
  static void ShowPhaseTransition(const std::string &from, const std::string &to);

  // Input Helpers
  static std::string ColorText(const std::string &text, const std::string &color);
  static std::string FormatCard(const std::string &name, int grade, bool is_standing = true,
                                bool show_icons = true);
  static std::string GetGradeIcon(int grade);
  static std::string GetStatusIcon(bool is_standing);

  // Card Display Helpers
  static std::string FormatPowerShield(int power, int shield);
  static std::string FormatCardShort(const std::string &code, const std::string &name, int grade);

  // Layout Helpers
  static void PrintCardPreview(const std::string &name, int grade, int power, int shield,
                               int critical, const std::string &skill, const std::string &type);
  static void PrintBattleStats(const std::string &attacker_name, int attack_power, int critical,
                               const std::string &defender_name, int defense_power);
};

#endif // UIHELPER_H