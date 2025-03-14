#include "cpu.h"

namespace cnes {


uint8_t CPU::fetch()
{
  return read(pc_++);
}

bool CPU::get_operand_address(CPU::ADDR_MODE mode, uint16_t& addr)
{
  addr = 0x0000;
  uint8_t lo, hi;
  bool page_crossed = false;
    uint16_t ptr = 0;
    uint16_t base = 0;
    
    switch (mode) {
      case IMP:  // 隐含寻址
        addr = 0;
        return false;

      case IMM:  // 立即寻址
        addr = pc_++;
        return false;

      case ZP0:  // 零页寻址
        addr = fetch();
        return false;

      case ZPX:  // 零页X变址
        addr = (fetch() + x_) & 0xFF;
        return false;

      case ZPY:  // 零页Y变址
        addr = (fetch() + y_) & 0xFF;
        return false;

      case REL:  // 相对寻址
        addr = fetch();
        if (addr & 0x80)
          addr |= 0xFF00;
        return false;

      case ABS:  // 绝对寻址
        lo = fetch();
        hi = fetch();
        addr = (hi << 8) | lo;
        return false;

      case ABX:  // 绝对X变址
        lo = fetch();
        hi = fetch();
        addr = ((hi << 8) | lo);
        page_crossed = ((addr & 0xFF00) != ((addr + x_) & 0xFF00));
        addr += x_;
        return page_crossed;

      case ABY:  // 绝对Y变址
        lo = fetch();
        hi = fetch();
        addr = ((hi << 8) | lo);
        page_crossed = ((addr & 0xFF00) != ((addr + y_) & 0xFF00));
        addr += y_;
        return page_crossed;

      case IND:  // 间接寻址
        lo = fetch();
        hi = fetch();
        ptr = (hi << 8) | lo;
        if (lo == 0xFF)  // 硬件bug模拟
          addr = (read(ptr & 0xFF00) << 8) | read(ptr);
        else
          addr = (read(ptr + 1) << 8) | read(ptr);
        return false;

      case IZX:  // 间接X变址
        ptr = (fetch() + x_) & 0xFF;
        addr = (read((ptr + 1) & 0xFF) << 8) | read(ptr);
        return false;

      case IZY:  // 间接Y变址
        addr = fetch();
        base = (read((addr + 1) & 0xFF) << 8) | read(addr);
        page_crossed = ((base & 0xFF00) != ((base + y_) & 0xFF00));
        addr = base + y_;
        return page_crossed;

      default:
        addr = 0x0000;
        return false;
    }
  }

  // 指令操作函数
  void CPU::LDA(uint16_t addr) { // 加载累加器
    uint8_t data = read(addr);
    a_ = data;
    set_flag(Z, a_ == 0);
    set_flag(N, a_ & 0x80);
  }

  void CPU::LDX(uint16_t addr) { // 加载X寄存器
    uint8_t data = read(addr);
    x_ = data;
    set_flag(Z, x_ == 0);
    set_flag(N, x_ & 0x80);
  }

  void CPU::LDY(uint16_t addr) { // 加载Y寄存器
    uint8_t data = read(addr);
    y_ = data;
    set_flag(Z, y_ == 0);
    set_flag(N, y_ & 0x80);
  }

  void CPU::STA(uint16_t addr) { // 存储累加器
    write(addr, a_);
  }

  void CPU::STX(uint16_t addr) { // 存储X寄存器
    write(addr, x_);
  }

  void CPU::STY(uint16_t addr) { // 存储Y寄存器
    write(addr, y_);
  }

  // 指令信息结构体
  struct Instruction {
    const char*   name;        // 名称
    uint8_t opcode;      // 操作码
    CPU::ADDR_MODE mode;      // 寻址模式
    uint8_t cycles;      // 基本周期数
    void (CPU::*operation)(uint16_t);  // 指令操作函数
  };

  // 指令操作函数
  void CPU::BEQ(uint16_t addr) { // 相等时分支
    if (get_flag(Z)) {
      uint16_t old_pc = pc_;
      pc_ += addr;
      branch_taken_ = true;
      page_crossed_ = (old_pc & 0xFF00) != (pc_ & 0xFF00);
    }
  }

  void CPU::BNE(uint16_t addr) { // 不相等时分支
    if (!get_flag(Z)) {
      uint16_t old_pc = pc_;
      pc_ += addr;
      branch_taken_ = true;
      page_crossed_ = (old_pc & 0xFF00) != (pc_ & 0xFF00);
    }
  }

  void CPU::BCS(uint16_t addr) { // 进位时分支
    if (get_flag(C)) {
      uint16_t old_pc = pc_;
      pc_ += addr;
      branch_taken_ = true;
      page_crossed_ = (old_pc & 0xFF00) != (pc_ & 0xFF00);
    }
  }

  void CPU::BCC(uint16_t addr) { // 无进位时分支
    if (!get_flag(C)) {
      uint16_t old_pc = pc_;
      pc_ += addr;
      branch_taken_ = true;
      page_crossed_ = (old_pc & 0xFF00) != (pc_ & 0xFF00);
    }
  }

  // 指令表
  static const Instruction instructions[] = {
    {"IMM", 0xA9, CPU::ADDR_MODE::IMM, 2, &CPU::LDA}, // LDA Immediate
    {"ZP0", 0xA5, CPU::ADDR_MODE::ZP0, 3, &CPU::LDA}, // LDA Zero Page
    {"ZPX", 0xB5, CPU::ADDR_MODE::ZPX, 4, &CPU::LDA}, // LDA Zero Page,X
    {"ABS", 0xAD, CPU::ADDR_MODE::ABS, 4, &CPU::LDA}, // LDA Absolute
    {"ABX", 0xBD, CPU::ADDR_MODE::ABX, 4, &CPU::LDA}, // LDA Absolute,X
    {"ABY", 0xB9, CPU::ADDR_MODE::ABY, 4, &CPU::LDA}, // LDA Absolute,Y
    {"IZX", 0xA1, CPU::ADDR_MODE::IZX, 6, &CPU::LDA}, // LDA (Indirect,X)
    {"IZY", 0xB1, CPU::ADDR_MODE::IZY, 5, &CPU::LDA}, // LDA (Indirect),Y
         
    {"IMM", 0xA2, CPU::ADDR_MODE::IMM, 2, &CPU::LDX}, // LDX Immediate
    {"ZP0", 0xA6, CPU::ADDR_MODE::ZP0, 3, &CPU::LDX}, // LDX Zero Page
    {"ZPY", 0xB6, CPU::ADDR_MODE::ZPY, 4, &CPU::LDX}, // LDX Zero Page,Y
    {"ABS", 0xAE, CPU::ADDR_MODE::ABS, 4, &CPU::LDX}, // LDX Absolute
    {"ABY", 0xBE, CPU::ADDR_MODE::ABY, 4, &CPU::LDX}, // LDX Absolute,Y
         
    {"IMM", 0xA0, CPU::ADDR_MODE::IMM, 2, &CPU::LDY}, // LDY Immediate
    {"ZP0", 0xA4, CPU::ADDR_MODE::ZP0, 3, &CPU::LDY}, // LDY Zero Page
    {"ZPX", 0xB4, CPU::ADDR_MODE::ZPX, 4, &CPU::LDY}, // LDY Zero Page,X
    {"ABS", 0xAC, CPU::ADDR_MODE::ABS, 4, &CPU::LDY}, // LDY Absolute
    {"ABX", 0xBC, CPU::ADDR_MODE::ABX, 4, &CPU::LDY}, // LDY Absolute,X
         
    {"ZP0", 0x85, CPU::ADDR_MODE::ZP0, 3, &CPU::STA}, // STA Zero Page
    {"ZPX", 0x95, CPU::ADDR_MODE::ZPX, 4, &CPU::STA}, // STA Zero Page,X
    {"ABS", 0x8D, CPU::ADDR_MODE::ABS, 4, &CPU::STA}, // STA Absolute
    {"ABX", 0x9D, CPU::ADDR_MODE::ABX, 5, &CPU::STA}, // STA Absolute,X
    {"ABY", 0x99, CPU::ADDR_MODE::ABY, 5, &CPU::STA}, // STA Absolute,Y
    {"IZX", 0x81, CPU::ADDR_MODE::IZX, 6, &CPU::STA}, // STA (Indirect,X)
    {"IZY", 0x91, CPU::ADDR_MODE::IZY, 6, &CPU::STA}, // STA (Indirect),Y
         
    {"ZP0", 0x86, CPU::ADDR_MODE::ZP0, 3, &CPU::STX}, // STX Zero Page
    {"ZPY", 0x96, CPU::ADDR_MODE::ZPY, 4, &CPU::STX}, // STX Zero Page,Y
    {"ABS", 0x8E, CPU::ADDR_MODE::ABS, 4, &CPU::STX}, // STX Absolute
         
    {"ZP0", 0x84, CPU::ADDR_MODE::ZP0, 3, &CPU::STY}, // STY Zero Page
    {"ZPX", 0x94, CPU::ADDR_MODE::ZPX, 4, &CPU::STY}, // STY Zero Page,X
    {"ABS", 0x8C, CPU::ADDR_MODE::ABS, 4, &CPU::STY}, // STY Absolute
         
    {"REL", 0xF0, CPU::ADDR_MODE::REL, 2, &CPU::BEQ}, // BEQ
    {"REL", 0xD0, CPU::ADDR_MODE::REL, 2, &CPU::BNE}, // BNE
    {"REL", 0xB0, CPU::ADDR_MODE::REL, 2, &CPU::BCS}, // BCS
    {"REL", 0x90, CPU::ADDR_MODE::REL, 2, &CPU::BCC}  // BCC
  };

  // 辅助函数：设置状态寄存器标志位
  void CPU::set_flag(FLAGS flag, bool value)
  {
    if (value)
      status_ |= flag;
    else
      status_ &= ~flag;
  }

  // 辅助函数：获取状态寄存器标志位
  bool CPU::get_flag(FLAGS flag)
  {
    return (status_ & flag) > 0;
  }

  void CPU::execute_instruction()
  {
    // 根据操作码查找指令
    for (const auto& inst : instructions) {
      if (inst.opcode == opcode_) {
        uint16_t addr;
        bool page_crossed = get_operand_address(inst.mode, addr);
        bool branch_taken = false;
        
        // 执行指令
        (this->*inst.operation)(addr);
        
        // 设置基本周期数
        cycles_ = inst.cycles;
        
        // 处理分支指令的额外周期
        if (inst.mode == REL) {
          // 分支成功时增加1个周期
          if (branch_taken) {
            cycles_++;
            // 跨页时再增加1个周期
            if (page_crossed) {
              cycles_++;
            }
          }
        }
        // 其他指令的跨页处理
        else if (page_crossed) {
          cycles_++;
        }
        
        return;
      }
    }

    // 未知指令
    cycles_ = 1;
  }


  const char* CPU::get_op_name()
  {
    if (opcode_ == 0x00)
    {
      return "None";
    }
    return instructions[opcode_].name;
  }
}
