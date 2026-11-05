#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include "Common/FileUtil.h"
#include "Core/DSP/DSPAssembler.h"
#include "Core/DSP/DSPDisassembler.h"
#include "Core/DSP/DSPTables.h"

int main(int argc, char** argv) {
    if (argc != 4) {
        std::fprintf(stderr, "dsp-inspect disassemble|list|assemble INPUT OUTPUT\n");
        return 2;
    }
    std::string input;
    if (!File::ReadFileToString(argv[2], input)) return 2;
    DSP::InitInstructionTable();
    DSP::AssemblerSettings settings;
    settings.decode_names = false;
    const std::string mode(argv[1]);
    std::string output;
    if (mode == "assemble") {
        DSP::DSPAssembler assembler(settings);
        std::vector<u16> code;
        if (!assembler.Assemble(input, code)) return 1;
        output.reserve(code.size() * 2);
        for (u16 word : code) {
            output.push_back(static_cast<char>(word >> 8));
            output.push_back(static_cast<char>(word));
        }
    } else if (mode == "disassemble" || mode == "list") {
        if (input.size() % 2) return 2;
        settings.show_pc = mode == "list";
        settings.show_hex = mode == "list";
        DSP::DSPDisassembler disassembler(settings);
        std::vector<u16> code;
        code.reserve(input.size() / 2);
        for (size_t index = 0; index < input.size(); index += 2) {
            code.push_back((static_cast<unsigned char>(input[index]) << 8) | static_cast<unsigned char>(input[index + 1]));
        }
        if (!disassembler.Disassemble(code, output)) return 1;
    } else {
        return 2;
    }
    std::ofstream destination(argv[3], std::ios::binary);
    destination.write(output.data(), output.size());
    destination.flush();
    destination.close();
    return destination ? 0 : 2;
}
