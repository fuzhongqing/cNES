#include "cpu.h"
#include "bus.h"

namespace cnes {

  CPU::CPU()
  {
    // 初始化寄存器
    a_ = 0x00;
    x_ = 0x00;
    y_ = 0x00;
    sp_ = 0xFD;
    status_ = 0x00 | U;
    pc_ = 0x0000;

    // 初始化内部状态
    opcode_ = 0x00;
    cycles_ = 0;
    clock_count_ = 0;
  }

  void CPU::clock()
  {
    if (cycles_ == 0) {
      // 获取新指令
      opcode_ = read(pc_++);
      // 执行指令
      execute_instruction();
    }

    cycles_--;
    clock_count_++;
  }

  void CPU::reset()
  {
    // 初始化寄存器
    a_ = 0x00;
    x_ = 0x00;
    y_ = 0x00;
    sp_ = 0xFD;
    status_ = 0x00 | U;

    // 从复位向量获取程序计数器初始值
    uint16_t lo = read(RESET_VECTOR);
    uint16_t hi = read(RESET_VECTOR + 1);
    pc_ = (hi << 8) | lo;

    // 重置内部状态
    cycles_ = 8;
    clock_count_ = 0;
  }

  void CPU::irq()
  {
    if (!(status_ & I)) {
      // 保存当前状态
      write(STACK_BASE + sp_, (pc_ >> 8) & 0xFF);
      sp_--;
      write(STACK_BASE + sp_, pc_ & 0xFF);
      sp_--;

      status_ &= ~B;    // 清除B标志
      status_ |= U;     // 设置U标志
      write(STACK_BASE + sp_, status_);
      sp_--;

      status_ |= I;     // 设置中断禁止标志

      // 加载中断向量
      uint16_t lo = read(IRQ_VECTOR);
      uint16_t hi = read(IRQ_VECTOR + 1);
      pc_ = (hi << 8) | lo;

      cycles_ = 7;
    }
  }

  void CPU::nmi()
  {
    // 保存当前状态
    write(STACK_BASE + sp_, (pc_ >> 8) & 0xFF);
    sp_--;
    write(STACK_BASE + sp_, pc_ & 0xFF);
    sp_--;

    status_ &= ~B;    // 清除B标志
    status_ |= U;     // 设置U标志
    write(STACK_BASE + sp_, status_);
    sp_--;

    status_ |= I;     // 设置中断禁止标志

    // 加载NMI向量
    uint16_t lo = read(NMI_VECTOR);
    uint16_t hi = read(NMI_VECTOR + 1);
    pc_ = (hi << 8) | lo;

    cycles_ = 8;
  }

  void CPU::write(uint16_t addr, uint8_t data)
  {
    bus_->write(addr, data);
  }

  uint8_t CPU::read(uint16_t addr)
  {
    return bus_->read(addr);
  }
}
