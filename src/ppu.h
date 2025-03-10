#ifndef CNES_PPU_H
#define CNES_PPU_H

#include <cstdint>
#include <array>

namespace cnes {

class Bus;

// Picture Processing Unit (2C02)
class PPU {
public:
    PPU();
    ~PPU() = default;

    // PPU与总线连接
    void connect_bus(Bus* bus) { bus_ = bus; }

    // PPU操作
    void clock();        // 时钟周期
    void reset();        // 重置PPU

    // PPU寄存器接口（CPU访问）
    uint8_t read_register(uint16_t addr);
    void write_register(uint16_t addr, uint8_t data);

    // 帧完成信号
    bool frame_complete();
    void clear_frame_complete();

    // 屏幕数据
    uint8_t* get_screen() { return screen_.data(); }

private:
    // PPU内存组件
    std::array<uint8_t, 2048> pattern_tables_{};    // 图案表
    std::array<uint8_t, 1024> name_tables_{};       // 名称表
    std::array<uint8_t, 32> palette_{};             // 调色板
    std::array<uint8_t, 256> oam_{};                // 对象属性内存

    // 屏幕缓冲
    std::array<uint8_t, 256 * 240> screen_{};

    // PPU寄存器
    uint8_t control_{};      // 控制寄存器
    uint8_t mask_{};         // 掩码寄存器
    uint8_t status_{};       // 状态寄存器
    uint8_t oam_addr_{};     // OAM地址寄存器

    // 内部变量
    int16_t scanline_{};     // 当前扫描线
    int16_t cycle_{};        // 当前周期
    bool frame_complete_{};   // 帧完成标志

    // 总线指针
    Bus* bus_ = nullptr;

    // 内存访问
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
};

} // namespace cnes

#endif // CNES_PPU_H