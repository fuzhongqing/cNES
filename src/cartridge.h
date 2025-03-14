#ifndef CNES_CARTRIDGE_H
#define CNES_CARTRIDGE_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include "mapper.h"
#include "mapper_000.h"

namespace cnes {

// NES卡带
class Cartridge {
public:
    Cartridge();
    ~Cartridge() = default;

    // 卡带操作
    bool load(const std::string& filename);    // 加载ROM文件
    bool load_from_memory(std::vector<uint8_t> data);      // 从内存中加载
    void reset();                              // 重置卡带

    // 内存访问
    bool cpu_read(uint16_t addr, uint8_t& data);
    bool cpu_write(uint16_t addr, uint8_t data);
    bool ppu_read(uint16_t addr, uint8_t& data);
    bool ppu_write(uint16_t addr, uint8_t data);

    // 镜像模式
    enum MIRROR {
        HORIZONTAL,
        VERTICAL,
        ONESCREEN_LO,
        ONESCREEN_HI,
    };

private:
    // ROM数据
    std::vector<uint8_t> prg_rom_;    // 程序ROM
    std::vector<uint8_t> chr_rom_;    // 字符ROM

    // RAM数据
    std::vector<uint8_t> prg_ram_;    // 程序RAM
    std::vector<uint8_t> chr_ram_;    // 字符RAM

    // Mapper
    std::unique_ptr<Mapper> mapper_;

    // iNES文件头
    struct Header {
        char name[4];          // NES^Z
        uint8_t prg_rom_size;  // PRG ROM大小（16KB单位）
        uint8_t chr_rom_size;  // CHR ROM大小（8KB单位）
        uint8_t flags6;        // Mapper低位，镜像，电池等
        uint8_t flags7;        // Mapper高位，VS/Playchoice等
        uint8_t flags8;        // PRG RAM大小
        uint8_t flags9;        // TV系统
        uint8_t flags10;       // TV系统，PRG RAM存在
        uint8_t padding[5];    // 未使用
    };

    // 镜像模式
    MIRROR mirror_mode_;
};

} // namespace cnes

#endif // CNES_CARTRIDGE_H
