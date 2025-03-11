#ifndef CNES_MAPPER_H
#define CNES_MAPPER_H

#include <cstdint>

namespace cnes {

// 基础Mapper类
class Mapper {
public:
    Mapper() = default;
    virtual ~Mapper() = default;

    // 内存映射操作
    virtual bool cpu_read(uint16_t addr, uint8_t& data) = 0;
    virtual bool cpu_write(uint16_t addr, uint8_t data) = 0;
    virtual bool ppu_read(uint16_t addr, uint8_t& data) = 0;
    virtual bool ppu_write(uint16_t addr, uint8_t data) = 0;

    // 镜像模式操作
    virtual uint8_t mirror_mode() = 0;

    // 中断请求
    virtual void scanline() { }
    virtual bool irq_state() { return false; }
    virtual void irq_clear() { }

protected:
    // Mapper配置
    uint8_t prg_banks_ = 0;
    uint8_t chr_banks_ = 0;
};

} // namespace cnes

#endif // CNES_MAPPER_H