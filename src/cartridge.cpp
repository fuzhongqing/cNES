#include "cartridge.h"
#include <fstream>

namespace cnes {

Cartridge::Cartridge() : mirror_mode_(HORIZONTAL) {}

bool Cartridge::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    // 读取文件到内存
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    return load_from_memory(data);
}

bool Cartridge::load_from_memory(std::vector<uint8_t> data) {
    if (data.size() < sizeof(Header)) return false;

    // 读取文件头
    Header header;
    std::copy(data.begin(), data.begin() + sizeof(Header),
              reinterpret_cast<uint8_t*>(&header));

    // 验证文件头
    if (header.name[0] != 'N' || header.name[1] != 'E' ||
        header.name[2] != 'S' || header.name[3] != 0x1A)
        return false;

    // 提取Mapper ID
    uint8_t mapper_id = ((header.flags7 & 0xF0) | (header.flags6 >> 4));

    // 设置镜像模式
    mirror_mode_ = (header.flags6 & 0x01) ? VERTICAL : HORIZONTAL;

    // 分配ROM/RAM空间
    size_t prg_rom_size = header.prg_rom_size * 16384;
    size_t chr_rom_size = header.chr_rom_size * 8192;

    prg_rom_.resize(prg_rom_size);
    chr_rom_.resize(chr_rom_size);

    // 复制ROM数据
    size_t offset = sizeof(Header);
    std::copy(data.begin() + offset,
              data.begin() + offset + prg_rom_size,
              prg_rom_.begin());
    offset += prg_rom_size;

    if (chr_rom_size > 0) {
        std::copy(data.begin() + offset,
                data.begin() + offset + chr_rom_size,
                chr_rom_.begin());
    }

    return true;
}

void Cartridge::reset() {
    if (mapper_) mapper_.reset();
}

bool Cartridge::cpu_read(uint16_t addr, uint8_t& data) {
    if (mapper_) {
        return mapper_->cpu_read(addr, data);
    }
    return false;
}

bool Cartridge::cpu_write(uint16_t addr, uint8_t data) {
    if (mapper_) {
        return mapper_->cpu_write(addr, data);
    }
    return false;
}

bool Cartridge::ppu_read(uint16_t addr, uint8_t& data) {
    if (mapper_) {
        return mapper_->ppu_read(addr, data);
    }
    return false;
}

bool Cartridge::ppu_write(uint16_t addr, uint8_t data) {
    if (mapper_) {
        return mapper_->ppu_write(addr, data);
    }
    return false;
}

} // namespace cnes
