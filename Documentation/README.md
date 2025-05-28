# 🃏 FIBO Card Commandos

> **ธีม**: ดวลการ์ดในห้องเรียนโปรแกรมมิ่ง—เรียกอาจารย์ TA และเพื่อน FrabXI ลงสนาม ผลัดกันตีโจทย์เพื่อทำดาเมจใส่คู่แข่ง!

## 🎯 เป้าหมายของเกม

- ทำดาเมจใส่คู่แข่งครบ **2 แต้ม** ก่อน
- หรือทำให้เด็คของคู่แข่ง "จั่วไม่ออก" (หมดกอง) → ชนะทันที

## 🚀 วิธีการติดตั้งและรัน

### 📋 ความต้องการระบบ

- **Compiler**: g++ หรือ clang++ ที่รองรับ C++17
- **OS**: Windows, macOS, Linux

### 🔧 การติดตั้ง

#### ใช้ Script

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
