#include <catch2/catch.hpp>
#include <iostream> //std::string
#include <fstream> //std::ofstream
#include <sstream>

#include "NES/CPU/Cpu.h"
#include "NES/Cartridge/CartridgeLoader.h"
#include "NES/Memory/Dma.h"

/**
 * running the nestest rom
 */
TEST_CASE("Run NesTest") {
    //read expect logs from nestest log file
    std::string logsPath = "NANE/test/resources/nettest_logs.txt";
    std::ifstream nesLog;
    nesLog.open(logsPath);

    //redirect cout to the string stream
    std::cout.flush();
    std::stringstream outSS;
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(outSS.rdbuf()); //redirect std::cout to outSS

    std::shared_ptr<Dma> dma = std::make_shared<Dma>();
    Cpu cpu(dma);
    dword nesTestEntry = 0xC000;
    REQUIRE(cpu.PowerCycle(&nesTestEntry) == true);

    CpuRegisters& registers = dma->GetCpuMemory().GetRegisters();
    registers.name.Y = 0;
    registers.name.P = 0x24;
    registers.name.S = 0xFD;

    CartridgeLoader cartridgeLoader;
    const std::string nestestPath = "NANE/test/resources/nestest.nes";
    std::shared_ptr<ICartridge> cartridge = cartridgeLoader.LoadCartridge(nestestPath);
    dma->SetCartridge(cartridge);

    int cpuCycles = 5003;
    for(int i = 0; i < cpuCycles; ++i)
    {
        cpu.Step(true);

        int logLineMaxSize = 10000;
        char coutLineC[logLineMaxSize];
        outSS.getline(coutLineC, logLineMaxSize);
        std::string coutLine(coutLineC);

        char nesTestC[logLineMaxSize];
        nesLog.getline(nesTestC, logLineMaxSize);
        std::string nesTestLine(nesTestC);

        //compaire cout with expect logs
        //CPU part
        REQUIRE(nesTestLine.substr(0, 73) == coutLine.substr(0, 73));

        //clock cycles part
        std::stringstream ssCycles;
        ssCycles << std::setw(9) << std::left << std::setfill(' ') << nesTestLine.substr(86,9);
        std::string cpuCycles = ssCycles.str();
        REQUIRE(cpuCycles == coutLine.substr(86,9));
    }

    //reset to standard output again
    std::cout.rdbuf(coutbuf);
}