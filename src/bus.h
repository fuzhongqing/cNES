#ifndef CNES_BUS_H
#define CNES_BUS_H

#include <cstdint>
#include <array>
#include "cpu.h"
#include "ppu.h"
#include "apu.h"

namespace cnes {

class Cartridge;

// 系统总线
class Bus {
public:
    Bus();
    ~Bus() = default;

    // 组件连接
    void connect_cartridge(Cartridge* cartridge) { 
      cartridge_ = cartridge; 
    }

    void connect_cpu(CPU* cpu) { 
      cpu_ = cpu; 
      cpu_->connect_bus(this);  
    }

    void connect_ppu(PPU* ppu) { 
      ppu_ = ppu; 
      ppu_->connect_bus(this);
    }

    void connect_apu(APU* apu) { 
      apu_ = apu; 
      apu_->connect_bus(this);
    }

    // 总线操作
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr);

    // 系统操作
    void clock();    // 系统时钟
    void reset();    // 系统重置

    // DMA传输
    void dma_write(uint8_t data);
    void dma_execute();

private:
    // 系统组件
    CPU* cpu_ = nullptr;
    PPU* ppu_ = nullptr;
    APU* apu_ = nullptr;
    Cartridge* cartridge_ = nullptr;

    // 系统RAM
    std::array<uint8_t, 2048> ram_{};

    // DMA状态
    bool dma_transfer_ = false;
    uint8_t dma_page_ = 0x00;
    uint8_t dma_addr_ = 0x00;
    uint8_t dma_data_ = 0x00;

    // 系统时钟计数
    uint32_t system_clock_counter_ = 0;
};

} // namespace cnes

#endif // CNES_BUS_H
