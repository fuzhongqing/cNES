#include "cpu.h"
#include "bus.h"

#include <ios>
#include <iostream>
#include <chrono>
#include <ostream>
#include <thread>
#include <iomanip>

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

    enable_debugging();
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

    if (enable_debugging_)
    {
      print_status();
    }
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

  void CPU::print_status()
  {
    // 清除控制台
    std::cout << "\033[2J\033[H";

    std::cout << "================================" << std::endl;
    std::cout << "MOS Technology 6502 CPU Debugger" << std::endl;
    std::cout << "================================" << std::endl;

    std::cout << "Clock:\t" << std::oct << clock_count_ << std::endl;
    std::cout << "Cycle:\t" << std::oct << static_cast<int>(cycles_) << std::endl; 

    std::cout << "A:\t0x" << std::hex << static_cast<int>(a_) << std::endl;
    std::cout << "X:\t0x" << std::hex << static_cast<int>(x_) << std::endl;
    std::cout << "Y:\t0x" << std::hex << static_cast<int>(y_) << std::endl;


    std::cout << "Status:\t0x" << std::hex << static_cast<int>(status_) << " [";
    std::cout << ((status_ & N) ? 'N' : '-') << " "
              << ((status_ & V) ? 'V' : '-') << " "
              << ((status_ & U) ? 'U' : '-') << " "
              << ((status_ & B) ? 'B' : '-') << " "
              << ((status_ & D) ? 'D' : '-') << " "
              << ((status_ & I) ? 'I' : '-') << " "
              << ((status_ & Z) ? 'Z' : '-') << " "
              << ((status_ & C) ? 'C' : '-') << "]" << std::endl;

    // 打印程序计数器（PC）
    std::cout << "PC:\t0x" << std::hex << std::setfill('0') << std::setw(4) << pc_ << std::endl;

    // 打印PC上下5个地址的内存内容
    std::cout << "--------------------------------" << std::endl;
    for (int i = 0; i <= 5; ++i) {
        uint16_t addr = pc_ + i;

        char p_char = ' ';
        char e_char = ' ';

        if (i == 0)
        {
          p_char = '[';
          e_char = ']';
        }

        std::cout << p_char << "0x" << std::hex << std::setfill('0') << std::setw(4) << addr << ": "
                  << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(read(addr)) << e_char << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;

    std::cout << get_op_name() << std::endl;

    std::cout << std::endl;
    
    std::cout << "SP:\t0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(sp_) << std::endl;

    std::cout << "Stack:" << std::endl;
    for (uint16_t addr = STACK_BASE + sp_ + 1; addr <= STACK_BASE + sp_ + 5 && addr <= STACK_BASE + 0xFF; ++addr) {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(4) << addr << ": "
                  << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(read(addr)) << std::endl;
    }

    std::cout << std::endl;


    std::chrono::milliseconds timespan(500);
    std::this_thread::sleep_for(timespan);
  }

  void CPU::enable_debugging()
  {
    enable_debugging_ = true;
  }
}
