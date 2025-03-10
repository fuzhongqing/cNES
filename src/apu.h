#ifndef CNES_APU_H
#define CNES_APU_H

#include <cstdint>
#include <array>

namespace cnes {

class Bus;

// Audio Processing Unit
class APU {
public:
    APU();
    ~APU() = default;

    // APU与总线连接
    void connect_bus(Bus* bus) { bus_ = bus; }

    // APU操作
    void clock();    // 时钟周期
    void reset();    // 重置APU

    // APU寄存器接口（CPU访问）
    uint8_t read_register(uint16_t addr);
    void write_register(uint16_t addr, uint8_t data);

    // 获取音频样本
    float get_audio_sample();

private:
    // 方波通道1
    struct {
        uint8_t duty = 0;           // 占空比
        uint8_t volume = 0;          // 音量
        uint16_t frequency = 0;      // 频率
        bool length_enabled = false;  // 长度计数器启用
        uint8_t length_counter = 0;  // 长度计数器
    } pulse1_{};

    // 方波通道2
    struct {
        uint8_t duty = 0;
        uint8_t volume = 0;
        uint16_t frequency = 0;
        bool length_enabled = false;
        uint8_t length_counter = 0;
    } pulse2_{};

    // 三角波通道
    struct {
        uint16_t frequency = 0;
        bool length_enabled = false;
        uint8_t length_counter = 0;
    } triangle_{};

    // 噪声通道
    struct {
        uint8_t volume = 0;
        uint16_t frequency = 0;
        bool length_enabled = false;
        uint8_t length_counter = 0;
    } noise_{};

    // DMC通道
    struct {
        uint16_t frequency = 0;
        uint16_t sample_address = 0;
        uint16_t sample_length = 0;
        bool loop = false;
    } dmc_{};

    // 帧计数器
    uint8_t frame_counter_ = 0;
    bool frame_interrupt_ = false;

    // 总线指针
    Bus* bus_ = nullptr;

    // 内存访问
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
};

} // namespace cnes

#endif // CNES_APU_H