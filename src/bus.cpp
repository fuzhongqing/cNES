#include "bus.h"
#include "cartridge.h"

namespace cnes {

Bus::Bus() {
}

void Bus::write(uint16_t addr, uint8_t data) {
 
  if (cartridge_ && cartridge_->cpu_write(addr, data)) {
  }
  else if (addr >= 0x0000 && addr <= 0x1FFF) {
      // 系统RAM，每2KB镜像
      ram_[addr & 0x07FF] = data;
  }
  else if (addr >= 0x2000 && addr <= 0x3FFF) {
      // PPU寄存器，每8字节镜像
      ppu_->write_register(0x2000 + (addr & 0x7), data);
  }
  else if (addr >= 0x4000 && addr <= 0x4013) {
      // APU寄存器
      apu_->write_register(addr, data);
  }
  else if (addr == 0x4014) {
      // PPU DMA
      dma_page_ = data;
      dma_transfer_ = true;
  }
  else if (addr == 0x4015) {
      // APU状态
      apu_->write_register(addr, data);
  }
}

uint8_t Bus::read(uint16_t addr) {
    uint8_t data = 0x00;

    if (cartridge_ && cartridge_->cpu_read(addr, data)) {
        // 从卡带读取
    }
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // 系统RAM
        data = ram_[addr & 0x07FF];
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        // PPU寄存器
        data = ppu_->read_register(0x2000 + (addr & 0x7));
    }
    else if (addr >= 0x4000 && addr <= 0x4013) {
        // APU寄存器
        data = apu_->read_register(addr);
    }
    else if (addr == 0x4015) {
        // APU状态
        data = apu_->read_register(addr);
    }

    return data;
}

void Bus::clock() {
    ppu_->clock();
    
    if (system_clock_counter_ % 3 == 0) {
        if (dma_transfer_) {
            dma_execute();
        }
        else {
            cpu_->clock();
        }
    }

    if (system_clock_counter_ % 2 == 0) {
        apu_->clock();
    }

    system_clock_counter_++;
}

void Bus::reset() {
    system_clock_counter_ = 0;
    dma_transfer_ = false;
    cpu_->reset();
    ppu_->reset();
    apu_->reset();
}

void Bus::dma_write(uint8_t data) {
    dma_data_ = data;
}

void Bus::dma_execute() {
    if (system_clock_counter_ % 2 == 0) {
        dma_data_ = read(dma_page_ << 8 | dma_addr_);
    }
    else {
        ppu_->write_register(0x2004, dma_data_);
        
        dma_addr_++;
        if (dma_addr_ == 0x00) {
            dma_transfer_ = false;
        }
    }
}

} // namespace cnes
