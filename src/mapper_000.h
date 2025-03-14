#ifndef CNES_MAPPER_000_H
#define CNES_MAPPER_000_H

#include "mapper.h"
#include <vector>

namespace cnes {

// NROM (Mapper 000)
class Mapper000 : public Mapper {
public:
    Mapper000(const std::vector<uint8_t>& prg_rom, const std::vector<uint8_t>& chr_rom, uint8_t mirror_mode);
    ~Mapper000() = default;

    bool cpu_read(uint16_t addr, uint8_t& data) override;
    bool cpu_write(uint16_t addr, uint8_t data) override;
    bool ppu_read(uint16_t addr, uint8_t& data) override;
    bool ppu_write(uint16_t addr, uint8_t data) override;
    uint8_t mirror_mode() override { return mirror_mode_; }

private:
    std::vector<uint8_t> prg_rom_;
    std::vector<uint8_t> chr_rom_;
    std::vector<uint8_t> chr_ram_;
    uint8_t mirror_mode_;
};

} // namespace cnes

#endif // CNES_MAPPER_000_H