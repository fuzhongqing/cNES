#ifndef CNES_CPU_H
#define CNES_CPU_H

#include <cstdint>
#include <array>

namespace cnes {

class Bus;

// MOS Technology 6502 CPU
class CPU {
public:
    CPU();
    ~CPU() = default;

    // CPU与总线连接
    void connect_bus(Bus* bus) { bus_ = bus; }

    // CPU操作
    void reset();    // 重置CPU
    void clock();    // 时钟周期
    void irq();      // 可屏蔽中断
    void nmi();      // 不可屏蔽中断

private:
    // CPU寄存器
    uint8_t a_ = 0x00;       // 累加器
    uint8_t x_ = 0x00;       // X变址寄存器
    uint8_t y_ = 0x00;       // Y变址寄存器
    uint8_t sp_ = 0xFD;      // 栈指针
    uint16_t pc_ = 0x0000;   // 程序计数器
    uint8_t status_ = 0x00;   // 状态寄存器

    // 状态标志位
    enum FLAGS {
        C = (1 << 0),    // 进位标志
        Z = (1 << 1),    // 零标志
        I = (1 << 2),    // 中断禁止
        D = (1 << 3),    // 十进制模式（未使用）
        B = (1 << 4),    // 中断类型标志
        U = (1 << 5),    // 未使用
        V = (1 << 6),    // 溢出标志
        N = (1 << 7),    // 负数标志
    };

    // 内部变量
    uint8_t opcode_ = 0x00;           // 当前操作码
    uint8_t cycles_ = 0;              // 剩余周期数
    uint32_t clock_count_ = 0;        // 时钟计数

    // 总线指针
    Bus* bus_ = nullptr;

    // 内存访问
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr);
};

} // namespace cnes

#endif // CNES_CPU_H
