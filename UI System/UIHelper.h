// UIHelper.h - ไฟล์ Header สำหรับระบบการแสดงผลและการจัดรูปแบบ
// ไฟล์นี้รวบรวมค่าคงที่และฟังก์ชันสำหรับการจัดการการแสดงผลในเกม
#ifndef UIHELPER_H
#define UIHELPER_H

#include <string>
#include <iostream>
#include <thread>
#include <chrono>

// รหัสสีแบบ ANSI สำหรับการแสดงผลในเทอร์มินัล
namespace Colors
{
  // สีพื้นฐาน
  const std::string RESET = "\033[0m";    // รีเซ็ตการจัดรูปแบบทั้งหมด
  const std::string BLACK = "\033[30m";   // สีดำ
  const std::string RED = "\033[31m";     // สีแดง
  const std::string GREEN = "\033[32m";   // สีเขียว
  const std::string YELLOW = "\033[33m";  // สีเหลือง
  const std::string BLUE = "\033[34m";    // สีน้ำเงิน
  const std::string MAGENTA = "\033[35m"; // สีม่วง
  const std::string CYAN = "\033[36m";    // สีฟ้า
  const std::string WHITE = "\033[37m";   // สีขาว

  // สีสว่าง (Bright colors)
  const std::string BRIGHT_BLACK = "\033[90m";   // สีดำสว่าง
  const std::string BRIGHT_RED = "\033[91m";     // สีแดงสว่าง
  const std::string BRIGHT_GREEN = "\033[92m";   // สีเขียวสว่าง
  const std::string BRIGHT_YELLOW = "\033[93m";  // สีเหลืองสว่าง
  const std::string BRIGHT_BLUE = "\033[94m";    // สีน้ำเงินสว่าง
  const std::string BRIGHT_MAGENTA = "\033[95m"; // สีม่วงสว่าง
  const std::string BRIGHT_CYAN = "\033[96m";    // สีฟ้าสว่าง
  const std::string BRIGHT_WHITE = "\033[97m";   // สีขาวสว่าง

  // รูปแบบข้อความ (Text styles)
  const std::string BOLD = "\033[1m";      // ตัวหนา
  const std::string UNDERLINE = "\033[4m"; // ขีดเส้นใต้
  const std::string ITALIC = "\033[3m";    // ตัวเอียง

  // สีพื้นหลัง (Background colors)
  const std::string BG_BLACK = "\033[40m";   // พื้นหลังสีดำ
  const std::string BG_RED = "\033[41m";     // พื้นหลังสีแดง
  const std::string BG_GREEN = "\033[42m";   // พื้นหลังสีเขียว
  const std::string BG_YELLOW = "\033[43m";  // พื้นหลังสีเหลือง
  const std::string BG_BLUE = "\033[44m";    // พื้นหลังสีน้ำเงิน
  const std::string BG_MAGENTA = "\033[45m"; // พื้นหลังสีม่วง
  const std::string BG_CYAN = "\033[46m";    // พื้นหลังสีฟ้า
  const std::string BG_WHITE = "\033[47m";   // พื้นหลังสีขาว
}

// ไอคอน Unicode สำหรับการแสดงผลในเกม
namespace Icons
{
  // ไอคอนพื้นฐานของเกม
  const std::string CARD = "🃏";      // ไอคอนการ์ด
  const std::string SWORD = "⚔️";      // ไอคอนการโจมตี
  const std::string SHIELD = "🛡️";     // ไอคอนการป้องกัน
  const std::string HEART = "💚";     // ไอคอนพลังชีวิต
  const std::string DIAMOND = "💎";   // ไอคอนเพชร/ของมีค่า
  const std::string STAR = "⭐";      // ไอคอนดาว/พิเศษ
  const std::string FIRE = "🔥";      // ไอคอนไฟ/พลังโจมตี
  const std::string LIGHTNING = "⚡"; // ไอคอนสายฟ้า
  const std::string CROWN = "👑";     // ไอคอนมงกุฎ
  const std::string TARGET = "🎯";    // ไอคอนเป้าหมาย
  const std::string RUNNER = "🏃";    // ไอคอนการวิ่ง
  const std::string SLEEP = "💤";     // ไอคอนการพัก
  const std::string MAGIC = "✨";     // ไอคอนเวทมนตร์
  const std::string HEAL = "💚";      // ไอคอนการรักษา
  const std::string DRAW = "📝";      // ไอคอนการจั่วการ์ด
  const std::string CRITICAL = "💥";  // ไอคอนการโจมตีรุนแรง
  const std::string DAMAGE = "🩸";    // ไอคอนความเสียหาย
  const std::string SKULL = "💀";     // ไอคอนหัวกะโหลก
  const std::string VICTORY = "🏆";   // ไอคอนชัยชนะ
  const std::string PLAYER = "👤";    // ไอคอนผู้เล่น
  const std::string TURN = "🔄";      // ไอคอนการเปลี่ยนเทิร์น
  const std::string PHASE = "⏰";     // ไอคอนเฟส
  const std::string MENU = "📋";      // ไอคอนเมนู
  const std::string HELP = "💡";      // ไอคอนช่วยเหลือ
  const std::string EXIT = "🚪";      // ไอคอนออกจากเกม
  const std::string NEXT = "▶️";       // ไอคอนถัดไป
  const std::string BACK = "◀️";       // ไอคอนย้อนกลับ
  const std::string UP = "⬆️";         // ไอคอนขึ้น
  const std::string DOWN = "⬇️";       // ไอคอนลง
  const std::string CONFIRM = "✅";   // ไอคอนยืนยัน
  const std::string CANCEL = "❌";    // ไอคอนยกเลิก
  const std::string WARNING = "⚠️";    // ไอคอนคำเตือน
  const std::string INFO = "ℹ️";       // ไอคอนข้อมูล
  const std::string DECK = "📚";      // ไอคอนสำรับการ์ด
  const std::string HAND = "🤲";      // ไอคอนมือ
  const std::string FIELD = "🏟️";     // ไอคอนสนาม
  const std::string SOUL = "👻";      // ไอคอนวิญญาณ
  const std::string DROP = "🗑️";      // ไอคอนการ์ดที่ถูกทิ้ง
  const std::string GUARD = "🛡️";      // ไอคอนการ์ดป้องกัน
}

// ไอคอนแสดงเกรดของการ์ด
namespace GradeIcons
{
  const std::string G0 = "⚪"; // เกรด 0 (สีขาว)
  const std::string G1 = "🔵"; // เกรด 1 (สีน้ำเงิน)
  const std::string G2 = "🟡"; // เกรด 2 (สีเหลือง)
  const std::string G3 = "🟠"; // เกรด 3 (สีส้ม)
  const std::string G4 = "🔴"; // เกรด 4 (สีแดง)
}

class UIHelper
{
public:
  // การจัดการหน้าจอ
  static void ClearScreen();                                                            // ล้างหน้าจอ
  static void PauseForUser(const std::string &message = "กด Enter เพื่อดำเนินการต่อ...");   // หยุดรอผู้ใช้
  static void ShowLoadingAnimation(const std::string &message, int duration_ms = 1500); // แสดงแอนิเมชันโหลด

  // การวาดกล่อง
  static void PrintHorizontalLine(char c = '=', int length = 70, const std::string &color = Colors::CYAN);    // วาดเส้นแนวนอน
  static void PrintHorizontalLine(const std::string &s, int length, const std::string &color = Colors::CYAN); // วาดเส้นแนวนอนด้วยข้อความ
  static void PrintBox(const std::string &content, const std::string &title = "",
                       const std::string &border_color = Colors::CYAN,
                       const std::string &text_color = Colors::WHITE); // วาดกล่องข้อความ
  static void PrintSectionHeader(const std::string &title, const std::string &icon = Icons::STAR,
                                 const std::string &color = Colors::BRIGHT_YELLOW); // แสดงหัวข้อส่วน
  static void PrintPhaseHeader(const std::string &phase_name, const std::string &player_name,
                               int turn_number = 0, const std::string &color = Colors::BRIGHT_CYAN); // แสดงหัวข้อเฟส

  // ประเภทข้อความ
  static void PrintSuccess(const std::string &message); // แสดงข้อความสำเร็จ
  static void PrintError(const std::string &message);   // แสดงข้อความผิดพลาด
  static void PrintWarning(const std::string &message); // แสดงข้อความเตือน
  static void PrintInfo(const std::string &message);    // แสดงข้อความข้อมูล

  // สถานะเกม
  static void PrintGameTitle(); // แสดงชื่อเกม
  static void PrintQuickStatus(const std::string &player_name, int hand_size, int deck_size,
                               int damage_count, int soul_count, int drop_count); // แสดงสถานะเกมแบบย่อ
  static void PrintShortcuts();                                                   // แสดงทางลัด

  // ความคืบหน้า/แอนิเมชัน
  static void PrintProgressBar(int current, int max, const std::string &label = ""); // แสดงแถบความคืบหน้า
  static void AnimateText(const std::string &text, int delay_ms = 50);               // แสดงข้อความแบบแอนิเมชัน
  static void ShowDriveCheckAnimation();                                             // แสดงแอนิเมชันตรวจสอบการ์ด
  static void ShowDamageAnimation();                                                 // แสดงแอนิเมชันความเสียหาย
  static void ShowPhaseTransition(const std::string &from, const std::string &to);   // แสดงการเปลี่ยนเฟส

  // ตัวช่วยสำหรับการป้อนข้อมูล
  static std::string ColorText(const std::string &text, const std::string &color); // เปลี่ยนสีข้อความ
  static std::string FormatCard(const std::string &name, int grade, bool is_standing = true,
                                bool show_icons = true); // จัดรูปแบบการ์ด
  static std::string GetGradeIcon(int grade);            // รับไอคอนเกรด
  static std::string GetStatusIcon(bool is_standing);    // รับไอคอนสถานะ

  // ตัวช่วยแสดงการ์ด
  static std::string FormatPowerShield(int power, int shield);                                     // จัดรูปแบบพลังและโล่
  static std::string FormatCardShort(const std::string &code, const std::string &name, int grade); // จัดรูปแบบการ์ดแบบย่อ

  // ตัวช่วยการจัดวาง
  static void PrintCardPreview(const std::string &name, int grade, int power, int shield,
                               int critical, const std::string &skill, const std::string &type); // แสดงตัวอย่างการ์ด
  static void PrintBattleStats(const std::string &attacker_name, int attack_power, int critical,
                               const std::string &defender_name, int defense_power); // แสดงสถิติการต่อสู้

  static int GetDisplayWidth(const std::string &str); // รับความกว้างของการแสดงผล

private:
  static std::string RepeatChar(char c, int n);               // ทำซ้ำตัวอักษร
  static std::string RepeatChar(wchar_t wc, int n);           // ทำซ้ำตัวอักษรแบบ wide
  static std::string RepeatChar(const std::string &s, int n); // ทำซ้ำข้อความ
};

#endif // UIHELPER_H