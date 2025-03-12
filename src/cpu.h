#ifndef CNES_CPU_H
#define CNES_CPU_H

#include <cstdint>
#include <array>

namespace cnes {

class Bus;

// MOS Technology 6502 CPU
class CPU {
public:

    // 关键内存地址常量
    static constexpr uint16_t STACK_BASE = 0x0100;      // 栈基地址
    static constexpr uint16_t NMI_VECTOR = 0xFFFA;      // 不可屏蔽中断向量
    static constexpr uint16_t RESET_VECTOR = 0xFFFC;    // 复位向量
    static constexpr uint16_t IRQ_VECTOR = 0xFFFE;      // 可屏蔽中断向量
                                                        //
    enum ADDR_MODE {
        IMP,    // 隐含寻址
        IMM,    // 立即寻址
        ZP0,    // 零页寻址
        ZPX,    // 零页X变址
        ZPY,    // 零页Y变址
        REL,    // 相对寻址
        ABS,    // 绝对寻址
        ABX,    // 绝对X变址
        ABY,    // 绝对Y变址
        IND,    // 间接寻址
        IZX,    // 间接X变址
        IZY     // 间接Y变址
    };

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

    CPU();
    ~CPU() = default;

    // 指令操作函数
    void LDA(uint16_t addr);  // 加载累加器
    void LDX(uint16_t addr);  // 加载X寄存器
    void LDY(uint16_t addr);  // 加载Y寄存器
    void STA(uint16_t addr);  // 存储累加器
    void STX(uint16_t addr);  // 存储X寄存器
    void STY(uint16_t addr);  // 存储Y寄存器
    void BEQ(uint16_t addr);  // 相等时分支
    void BNE(uint16_t addr);  // 不相等时分支
    void BCS(uint16_t addr);  // 进位时分支
    void BCC(uint16_t addr);  // 无进位时分支

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


    // 内部变量
    uint8_t opcode_ = 0x00;           // 当前操作码
    uint8_t cycles_ = 0;              // 剩余周期数
    uint32_t clock_count_ = 0;        // 时钟计数

    // 总线指针
    Bus* bus_ = nullptr;

    // 指令执行
    void execute_instruction();
    uint8_t fetch();
    bool get_operand_address(ADDR_MODE mode, uint16_t& addr);

    // 状态标志位操作
    void set_flag(FLAGS flag, bool value);
    bool get_flag(FLAGS flag);

    // 分支指令状态
    bool branch_taken_ = false;
    bool page_crossed_ = false;

    // 内存访问
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr);
};

} // namespace cnes

#endif // CNES_CPU_H
