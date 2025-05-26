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

**สำหรับ Windows (PowerShell):**

```powershell
# รัน compile script จาก Build Tools
& ".\Build Tools\compile_and_run.bat"

# รันเกม
& ".\fibo_card_commandos.exe"
```

#### วิธีที่ 2: ใช้ Makefile

```bash
# Build และรัน
make run

# หรือ build อย่างเดียว
make

# ล้างไฟล์ build
make clean
```

## 📦 โครงสร้างโปรเจค

```
📦 FIBO-Card-Commandos/
├── 🎮 Game Core/           # โค้ดหลักของเกม
│   ├── Main.cpp           # จุดเริ่มต้นโปรแกรม
│   ├── Card.h/.cpp        # คลาสการ์ด
│   ├── Deck.h/.cpp        # คลาสเด็ค
│   └── Player.h/.cpp      # คลาสผู้เล่น
├── 🎨 UI System/          # ระบบส่วนติดต่อผู้ใช้
│   ├── UIHelper.h/.cpp    # เครื่องมือ UI และสี
│   └── MenuSystem.h/.cpp  # ระบบเมนู
├── 📊 Data/               # ข้อมูลเกม
│   └── cards.json         # ฐานข้อมูลการ์ด
├── 🔧 Build Tools/        # เครื่องมือสำหรับ build
│   ├── Makefile
│   ├── compile_and_run.sh
│   └── compile_and_run.bat
└── 📖 Documentation/      # เอกสาร
    └── README.md
```

## 🎮 การควบคุมเกม

| คำสั่ง  | ฟังก์ชัน          |
| ------- | ----------------- |
| `h`     | แสดงความช่วยเหลือ |
| `s`     | แสดงสถานะเต็ม     |
| `f`     | แสดงสนามรวม       |
| `q`     | ออกจากเกม         |
| `ESC`   | ย้อนกลับ          |
| `Tab`   | เปลี่ยนมุมมอง     |
| `Space` | ข้าม Phase        |

## 🔄 ลำดับเทิร์น

1. **🏃 Stand Phase**: ยูนิตทั้งหมดลุก (Stand)
2. **📝 Draw Phase**: จั่วการ์ด 1 ใบ
3. **👑 Ride Phase**: อัปเกรด Vanguard (เกรด +1)
4. **🃏 Main Phase**: Call ยูนิตลง Rear Guard
5. **⚔️ Battle Phase**: โจมตีและป้องกัน
6. **🌙 End Phase**: ล้างสถานะและจบเทิร์น

## 👥 ผู้พัฒนา

**FIBO Card Commandos** พัฒนาโดย:

- 🎓 นักศึกษา FRA142/FRA143
- 🏫 สถาบันวิทยาการหุ่นยนต์ภาคสนาม
- 🎯 มหาวิทยาลัยเทคโนโลยีพระจอมเกล้าธนบุรี

## 🐛 การแก้ไขปัญหาที่พบบ่อย

1. **สีไม่แสดงใน Windows**

   - ใช้ Windows Terminal หรือ PowerShell
   - เปิดใช้งาน ANSI color support

2. **ไอคอน Unicode แสดงผลไม่ถูกต้อง**

   - ตั้งค่า font ที่รองรับ Unicode
   - ตั้งค่า terminal encoding เป็น UTF-8

3. **ไม่พบไฟล์ cards.json**

   - ตรวจสอบว่าไฟล์อยู่ใน Data/
   - รันจากโฟลเดอร์หลักของโปรเจค

4. **Compilation error**
   - ตรวจสอบว่าใช้ g++ เวอร์ชัน 7+ หรือ Clang 5+
   - ตรวจสอบว่าติดตั้ง C++17 แล้ว

## 📝 License

Copyright © 2025 FIBO KMUTT. All rights reserved.
