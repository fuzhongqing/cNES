#ifndef CNES_TEST_ROM_H
#define CNES_TEST_ROM_H

#include <vector>
#include <cstdint>

namespace cnes {

class TestROM {
public:
    // 获取测试ROM数据
    static std::vector<uint8_t> get_test_rom_data() {
        // iNES文件头 (16字节)
        std::vector<uint8_t> rom_data = {
            0x4E, 0x45, 0x53, 0x1A,  // NES^Z 标识
            0x01,                     // 16KB PRG-ROM
            0x01,                     // 8KB CHR-ROM
            0x00,                     // Mapper 0, 水平镜像
            0x00,                     // Mapper 0
            0x00,                     // PRG-RAM大小
            0x00,                     // NTSC格式
            0x00,                     // 未使用
            0x00, 0x00, 0x00, 0x00   // 未使用
        };

        // PRG-ROM (16KB)
        // 一个简单的测试程序，在屏幕上显示一些图案
        std::vector<uint8_t> prg_rom(16384, 0xEA);  // 填充NOP指令
        // 重置向量指向程序起始位置
        prg_rom[0x3FFC] = 0x00;
        prg_rom[0x3FFD] = 0x80;

        // CHR-ROM (8KB)
        // 一些基本的图形数据
        std::vector<uint8_t> chr_rom(8192, 0x00);
        // 设置一些简单的图案
        for (int i = 0; i < 16; i++) {
            chr_rom[i] = 0xFF;  // 实心方块图案
        }

        // 合并所有数据
        rom_data.insert(rom_data.end(), prg_rom.begin(), prg_rom.end());
        rom_data.insert(rom_data.end(), chr_rom.begin(), chr_rom.end());

        return rom_data;
    }
};

} // namespace cnes

#endif // CNES_TEST_ROM_H