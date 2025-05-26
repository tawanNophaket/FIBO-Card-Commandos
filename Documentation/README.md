# 🃏 FIBO Card Commandos

> **ธีม**: ดวลการ์ดในห้องเรียนโปรแกรมมิ่ง—เรียกอาจารย์ TA และเพื่อน FrabXI ลงสนาม ผลัดกันตีโจทย์เพื่อทำดาเมจใส่คู่แข่ง!

## 🎯 เป้าหมายของเกม

- ทำดาเมจใส่คู่แข่งครบ **6 แต้ม** ก่อน
- หรือทำให้เด็คของคู่แข่ง "จั่วไม่ออก" (หมดกอง) → ชนะทันที

## 🚀 วิธีการติดตั้งและรัน

### 📋 ความต้องการระบบ

- **Compiler**: g++ หรือ clang++ ที่รองรับ C++17
- **OS**: Windows, macOS, Linux
- **RAM**: อย่างน้อย 1GB
- **Storage**: อย่างน้อย 50MB

### 🔧 การติดตั้ง

#### วิธีที่ 1: ใช้ Script (แนะนำ)

**สำหรับ Linux/macOS:**
```bash
chmod +x compile_and_run.sh
./compile_and_run.sh
```

**สำหรับ Windows:**
```cmd
compile_and_run.bat
```

#### วิธีที่ 2: ใช้ Makefile

```bash
# Build และรัน
make run

# หรือ build อย่างเดียว
make

# Debug build
make debug

# ล้างไฟล์ build
make clean
```

#### วิธีที่ 3: Manual Compilation

```bash
g++ -std=c++17 -Wall -Wextra -O2 \
    main_improved.cpp \
    Card.cpp \
    Deck.cpp \
    Player_Improved.cpp \
    UIHelper.cpp \
    MenuSystem.cpp \
    -o fibo_card_commandos

./fibo_card_commandos
```

## 🎮 คู่มือการเล่น

### 🕹️ การควบคุมเกม

| คำสั่ง | ฟังก์ชัน |
|--------|----------|
| `h` | แสดงความช่วยเหลือ |
| `s` | แสดงสถานะเต็ม |
| `f` | แสดงสนามรวม |
| `q` | ออกจากเกม |
| `ESC` | ย้อนกลับ |
| `Tab` | เปลี่ยนมุมมอง |
| `Space` | ข้าม Phase |

### 🔄 ลำดับเทิร์น

1. **🏃 Stand Phase**: ยูนิตทั้งหมดลุก (Stand)
2. **📝 Draw Phase**: จั่วการ์ด 1 ใบ
3. **👑 Ride Phase**: อัปเกรด Vanguard (เกรด +1)
4. **🃏 Main Phase**: Call ยูนิตลง Rear Guard
5. **⚔️ Battle Phase**: โจมตีและป้องกัน
6. **🌙 End Phase**: ล้างสถานะและจบเทิร์น

### ⚔️ ระบบการต่อสู้

#### 🎯 ขั้นตอนการโจมตี

1. **เลือก Attacker**: VC หรือ RC แถวหน้า
2. **เลือก Booster**: ยูนิตแถวหลัง (G0-1) เพื่อเพิ่มพลัง
3. **เลือกเป้าหมาย**: VC หรือ RC แถวหน้าของฝ่ายตรงข้าม
4. **Drive Check**: ถ้า VG โจมตี (G3+ = 2 ครั้ง, อื่นๆ = 1 ครั้ง)
5. **Guard Phase**: ฝ่ายตรงข้ามใช้การ์ดป้องกัน
6. **คำนวณผล**: Attack Power vs Defense Power + Shield

#### 🎲 ระบบ Trigger

| Trigger | เอฟเฟกต์ | ไอคอน |
|---------|----------|-------|
| **Critical** | +10000 Power + 1 Critical | 💥 |
| **Draw** | +10000 Power + จั่ว 1 ใบ | 📝 |
| **Heal** | +10000 Power + Heal 1 (เงื่อนไข) | 💚 |

## 🎨 ฟีเจอร์ UI/UX ใหม่

### 🌈 สี และ ไอคอน

- **สีแยกตามฟังก์ชัน**: เกรดการ์ด, สถานะ, ข้อความ
- **ไอคอน Unicode**: ทำให้อ่านง่ายและสวยงาม
- **Animation**: Loading, Drive Check, Damage

### 📋 ระบบเมนูใหม่

- **Menu แบบโต้ตอบ**: เลือกได้ง่าย ดูข้อมูลครบ
- **Context Help**: คำแนะนำตามสถานการณ์
- **Smart Display**: แสดงเฉพาะข้อมูลที่เกี่ยวข้อง

### 🖥️ การแสดงผลปรับปรุง

- **Field Layout**: ตารางสวยงาม มีกรอบและสี
- **Card Preview**: แสดงรายละเอียดการ์ดครบถ้วน
- **Status Bar**: ข้อมูลสำคัญแสดงตลอดเวลา

## 📚 โครงสร้างไฟล์

```
📦 FIBO-Card-Commandos/
├── 🎮 Game Core
│   ├── main_improved.cpp      # Main game loop ปรับปรุงแล้ว
│   ├── Card.h/Card.cpp        # คลาสการ์ด
│   ├── Deck.h/Deck.cpp        # คลาสเด็ค
│   └── Player.h/Player_Improved.cpp # คลาสผู้เล่นปรับปรุงแล้ว
├── 🎨 UI System
│   ├── UIHelper.h/UIHelper.cpp     # ระบบ UI และสี
│   └── MenuSystem.h/MenuSystem.cpp # ระบบเมนูและ Input
├── 📊 Data
│   └── cards.json             # ฐานข้อมูลการ์ด
├── 🔧 Build Tools
│   ├── Makefile              # Make build system
│   ├── compile_and_run.sh    # Linux/macOS script
│   └── compile_and_run.bat   # Windows script
└── 📖 Documentation
    └── README.md             # คู่มือนี้
```

## 🃏 รายการการ์ด

### การ์ดเกรด 0 (Starter + Trigger)
- **G0-01 เบเบี้ตี๋** (Starter) - เริ่มเกม
- **G0-02 บูม** (Heal Trigger) - Heal ดาเมจ
- **G0-03 น้ำ** (Draw Trigger) - จั่วการ์ด
- **G0-04 ตะวัน** (Critical Trigger) - เพิ่มดาเมจ

### การ์ดเกรด 1 (Support)
- **G1-01 ดิว** (Booster) - เพิ่มพลัง
- **G1-02 เล้ง** (Perfect Guard) - ป้องกันสมบูรณ์
- **G1-03 คิน** (Interceptor) - ป้องกันจาก RC
- **G1-04 เกม** (Draw Engine) - จั่วและทิ้ง
- **G1-05 ปลั๊ก** (Searcher) - ค้นการ์ด

### การ์ดเกรด 2 (Attackers)
- **G2-01 พี่เทียน** (Front-Attacker) - บัฟเมื่อมีแถวหน้า
- **G2-02 พี่ไออุ่น** (Recharger) - จั่วเมื่อ Hit VG
- **G2-03 พี่ปีใหม่** (Soul-Boost) - บัฟตาม Soul
- **G2-04 พี่เต็งหนึ่ง** (Board Control) - ทำลายยูนิต

### การ์ดเกรด 3 (Teachers)
- **G3-01 อ.บลิ๊งค์** (Field Cleaner) - ทำลายเมื่อโจมตี
- **G3-02 อ.ปิ** (Sentinel Tutor) - ค้น Perfect Guard
- **G3-03 อ.ต้น** (Shield Support) - เพิ่ม Shield
- **G3-04 อ.หยุน** (Crit Pressure) - เพิ่ม Critical

### การ์ดเกรด 4 (Finisher)
- **G4-01 อ.ตี๋** (Ultimate) - Stand หลายยูนิต + Critical

## 🎯 Functional Requirements

### ✅ ระบบหลัก
- [x] **การจัดการการ์ด**: โหลดจาก JSON, จัดเก็บข้อมูลครบถ้วน
- [x] **ระบบเด็ค**: สับ, จั่ว, ค้นหา, เคลื่อนย้ายการ์ด
- [x] **ผู้เล่น 2 คน**: สลับเทิร์น, จัดการสถานะแยกกัน
- [x] **สนามการเล่น**: VC, RC 5 ตำแหน่ง, Damage Zone, Soul, Drop
- [x] **ระบบต่อสู้**: Attack, Guard, Drive Check, Damage Check
- [x] **ระบบ Trigger**: Critical, Draw, Heal

### ✅ ระบบ UI/UX
- [x] **เมนูแบบโต้ตอบ**: Menu System ครบถ้วน
- [x] **แสดงผลสี**: ANSI Colors, Unicode Icons
- [x] **Animation**: Loading, Drive Check, Damage
- [x] **Help System**: Context-sensitive help
- [x] **Input Validation**: ป้องกัน input ผิด

### ✅ การจัดการเกม
- [x] **เงื่อนไขชนะ-แพ้**: 6 Damage, Deck Out
- [x] **Turn Management**: ครบทุก Phase
- [x] **Error Handling**: จัดการ edge cases
- [x] **Save State**: แสดงสถานะครบถ้วน

## 🔧 Non-Functional Requirements

### 🚀 ประสิทธิภาพ
- **Response Time**: < 100ms สำหรับ UI interactions
- **Memory Usage**: < 100MB RAM usage
- **Startup Time**: < 3 วินาที

### 🖥️ ความเข้ากันได้
- **Cross-Platform**: Windows, macOS, Linux
- **Terminal**: รองรับ ANSI colors และ Unicode
- **Compiler**: C++17 standard compliance

### 👥 การใช้งาน
- **Intuitive UI**: เมนูเข้าใจง่าย มี shortcuts
- **Clear Feedback**: ข้อความชัดเจน มีสีแยกประเภท
- **Error Recovery**: แจ้งข้อผิดพลาดและแนะนำแก้ไข

### 🔒 ความมั่นคง
- **Input Validation**: ตรวจสอบ input ทุกจุด
- **Memory Safety**: ใช้ smart pointers และ RAII
- **Exception Handling**: จัดการ runtime errors

## 🐛 การแก้ไขปัญหา

### ❗ ปัญหาที่พบบ่อย

**Q: โปรแกรมไม่แสดงสี**
A: ตรวจสอบ terminal รองรับ ANSI colors หรือใช้ modern terminal

**Q: ไม่แสดง Unicode icons**
A: ตั้งค่า terminal encoding เป็น UTF-8

**Q: Compilation error**
A: ตรวจสอบ g++ version รองรับ C++17 (GCC 7+ หรือ Clang 5+)

**Q: ไฟล์ cards.json หาไม่เจอ**
A: ตรวจสอบไฟล์อยู่ในโฟลเดอร์เดียวกับ executable

## 🚧 การพัฒนาในอนาคต

### 🔮 ฟีเจอร์ที่วางแผน
- [ ] **Skill System**: implement สกิลของการ์ดแต่ละใบ
- [ ] **AI Player**: เพิ่มผู้เล่น AI
- [ ] **Deck Builder**: ระบบสร้างเด็คเอง
- [ ] **Replay System**: บันทึกและดูเกมซ้ำ
- [ ] **Network Play**: เล่นผ่าน network
- [ ] **Tournament Mode**: ระบบแข่งขัน

### 🎨 การปรับปรุง UI
- [ ] **Animated Cards**: การ์ดเคลื่อนไหว
- [ ] **Sound Effects**: เสียงประกอบ
- [ ] **Custom Themes**: ธีมสีที่เลือกได้
- [ ] **Resolution Scaling**: ปรับขนาดหน้าจอ

## 👥 ทีมพัฒนา

**FIBO Card Commandos** พัฒนาโดย:
- 🎓 นักศึกษา FRA142/FRA143
- 🎯 Final Project: OOP Card Battle Game
- 🏫 มหาวิทยาลัย [ชื่อมหาวิทยาลัย]

## 📜 License

This project is created for educational purposes as part of FRA142/FRA143 coursework.

---

## 🎮 Quick Start

```bash
# Clone หรือ download project
# cd เข้าโฟลเดอร์โปรเจกต์

# รันเกมด้วยคำสั่งเดียว
./compile_and_run.sh

# หรือใช้ Make
make run
```

**เริ่มเล่น FIBO Card Commandos กันเลย! 🚀**