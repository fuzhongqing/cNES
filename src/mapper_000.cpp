#include "mapper_000.h"

namespace cnes {

Mapper000::Mapper000(const std::vector<uint8_t>& prg_rom, const std::vector<uint8_t>& chr_rom, uint8_t mirror_mode)
    : prg_rom_(prg_rom), chr_rom_(chr_rom), mirror_mode_(mirror_mode) {
    // 如果没有CHR-ROM，则分配8KB CHR-RAM
    if (chr_rom_.empty()) {
        chr_ram_.resize(0x2000);
    }
}

bool Mapper000::cpu_read(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        // 对于16KB PRG-ROM，镜像两次
        // 对于32KB PRG-ROM，直接映射
        size_t index = (addr - 0x8000) % prg_rom_.size();
        data = prg_rom_[index];
        return true;
    }
    return false;
}

bool Mapper000::cpu_write(uint16_t addr, uint8_t data) {
    // NROM不支持PRG-ROM写入
    return false;
}

bool Mapper000::ppu_read(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (!chr_rom_.empty()) {
            data = chr_rom_[addr];
        } else {
            data = chr_ram_[addr];
        }
        return true;
    }
    return false;
}

bool Mapper000::ppu_write(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (chr_rom_.empty()) {
            chr_ram_[addr] = data;
            return true;
        }
    }
    return false;
}

} // namespace cnes