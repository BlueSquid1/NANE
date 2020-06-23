#include "Cpu.h"

#include <memory> //std::unique_ptr
#include <iostream> //std::cout
#include <sstream> //std::stringstream
#include <iomanip> //std::setfill and std::setw
#include <exception> //throw exceptions

Cpu::Cpu(Dma& dma)
: dma(dma)
{
    //this->dma.GetNmiEventHandler() += this->HandleNmiEvent;
}

bool Cpu::PowerCycle(dword * overridePcAddress)
{
    this->GetRegs().name.P = 0x34;
    this->GetRegs().name.A = 0x0;
    this->GetRegs().name.X = 0x0;
    this->GetRegs().name.Y = 0x0;
    this->GetRegs().name.Y = 0xFD;
    this->dma.Write(0x4017, 0x0);
    this->dma.Write(0x4015, 0x0);
    for(int i = 0x4000; i < 0x400F; ++i)
    {
        this->dma.Write(i, 0x0);
    }
    for(int i = 0x4010; i < 0x4013; ++i)
    {
        this->dma.Write(i, 0x0);
    }

    if(overridePcAddress == NULL)
    {
        //the entry point is set at 0xFFFC in the ROM
        this->GetRegs().name.PC = BitUtil::GetDWord(&this->dma, 0xFFFC);
    }
    else
    {
        this->GetRegs().name.PC = *overridePcAddress;
    }

    this->SetTotalClockCycles(7);
    return true;
}

int Cpu::Step(bool verbose)
{
    //handle DMA
    if(this->dma.GetDmaActive())
    {
        //put CPU to sleep for 85 cycles (or 85 * 3 = 256 ppu cycles)
        return 85;
    }

    //decode instruction
    std::unique_ptr<Instructions::Instruction> decodedInst = this->DecodeInstruction(this->GetRegs().name.PC, false, verbose);
    if(decodedInst == NULL)
    {
        std::cerr << "failed to decode instruction at " << this->GetRegs().name.PC << " opcode: " << (int)this->dma.Seek(this->GetRegs().name.PC) << std::endl;
        ++this->GetRegs().name.PC;
        return 0;
    }
    Instructions::Opcode opcode = decodedInst->opcode;
    dword argAddress = decodedInst->argAddress;
    int instLen = decodedInst->instLen;
    int cycleCount = decodedInst->cycleCount;
    bool outputToAccum = decodedInst->outputToAccum;

    //print to display
    if(verbose == true)
    {
        std::string addressModeText = decodedInst->addressModeText;

        std::stringstream instCodeSS;
        for(int i = 0; i < instLen; ++i)
        {
            instCodeSS << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << std::right << (int) this->dma.Seek(this->GetRegs().name.PC + i) << " ";
        }
        std::string instCodeText = instCodeSS.str();
        std::cout<< std::hex << std::setfill('0') << std::setw(4) << std::right << std::uppercase << this->GetRegs().name.PC << "  ";
        std::cout << std::setw (10) << std::left << std::setfill(' ') << instCodeText;
        std::cout << std::setw (32) << std::left << addressModeText;
        std::cout << "A:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.A << " ";
        std::cout << "X:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.X << " ";
        std::cout << "Y:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.Y << " ";
        std::cout << "P:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.P << " ";
        std::cout << "SP:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.S << " ";
        std::cout << "PPU:" << std::dec << std::setfill(' ') << std::setw(3) << std::right << (int) 0 << ",";
        std::cout << std::dec << std::setfill(' ') << std::setw(3) << std::right << (int) 0 << " ";
        std::cout << "CYC:" << std::dec << std::setfill(' ') << std::setw(4) << std::left << (int) this->totalClockCycles << " ";
        std::cout << "PPUADDR: " << std::hex << std::setfill('0') << std::setw(4) << std::right << (int) this->dma.GetPpuMemory().GetRegisters().bgr.vramPpuAddress.val << " ";
        std::cout << std::endl;
    }

    if(this->totalClockCycles == 116854)
    {
        int i = 0;
    }

    //increment PC
    this->GetRegs().name.PC += instLen;

    //execute instruction
    switch(opcode.instr)
    {
        case Instructions::Instr::ADC:
        {
            byte inputVal = this->dma.Read( argAddress );
            byte oldA = this->GetRegs().name.A;
            this->GetRegs().name.A = this->GetRegs().name.A + this->GetRegs().name.C + inputVal;
            this->UpdateRegsForOverflow(oldA, inputVal);
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::AND:
        {
            byte inputVal = this->dma.Read( argAddress );
            this->GetRegs().name.A = this->GetRegs().name.A & inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::ASL:
        {
            byte inputVal;
            if(outputToAccum)
            {
                inputVal = this->GetRegs().name.A;
            }
            else
            {
                inputVal = this->dma.Read( argAddress );
            }
            this->GetRegs().name.C = BitUtil::GetBits(inputVal,7);
            byte result = inputVal << 1;
            if(outputToAccum)
            {
                this->GetRegs().name.A = result;
            }
            else
            {
                this->dma.Write(argAddress, result);
            }
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::BCC:
        {
            if( this->GetRegs().name.C == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->GetRegs().name.PC);
                this->GetRegs().name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BCS:
        {
            if( this->GetRegs().name.C == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->GetRegs().name.PC);
                this->GetRegs().name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BEQ:
        {
            if( this->GetRegs().name.Z == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->GetRegs().name.PC);
                this->GetRegs().name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BIT:
        {
            byte inputVal = this->dma.Read( argAddress );
            byte result = this->GetRegs().name.A & inputVal;
            this->UpdateRegsForZeroAndNeg(result);
            this->GetRegs().name.V = BitUtil::GetBits(inputVal, 6);
            this->GetRegs().name.N = BitUtil::GetBits(inputVal, 7);
            break;
        }
        case Instructions::Instr::BMI:
        {
            if( this->GetRegs().name.N == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->GetRegs().name.PC);
                this->GetRegs().name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BNE:
        {
            if( this->GetRegs().name.Z == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->GetRegs().name.PC);
                this->GetRegs().name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BPL:
        {
            byte p = this->GetRegs().name.P;
            if( this->GetRegs().name.N == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->GetRegs().name.PC);
                this->GetRegs().name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BRK:
        {
            this->HandleInterrupt(InterruptType::breakCommand, verbose);
            break;
        }
        case Instructions::Instr::BVC:
        {
            if( this->GetRegs().name.V == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->GetRegs().name.PC);
                this->GetRegs().name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BVS:
        {
            if( this->GetRegs().name.V == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->GetRegs().name.PC);
                this->GetRegs().name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::CLC:
        {
            this->GetRegs().name.C = false;
            break;
        }
        case Instructions::Instr::CLD:
        {
            this->GetRegs().name.D = false;
            break;
        }
        case Instructions::Instr::CLI:
        {
            this->GetRegs().name.I = false;
            break;
        }
        case Instructions::Instr::CLV:
        {
            this->GetRegs().name.V = false;
            break;
        }
        case Instructions::Instr::CMP:
        {
            byte inputVal = this->dma.Read( argAddress );
            this->UpdateRegsForCompaire(this->GetRegs().name.A, inputVal);
            break;
        }
        case Instructions::Instr::CPX:
        {
            byte inputVal = this->dma.Read( argAddress );
            this->UpdateRegsForCompaire(this->GetRegs().name.X, inputVal);
            break;
        }
        case Instructions::Instr::CPY:
        {
            byte inputVal = this->dma.Read( argAddress );
            this->UpdateRegsForCompaire(this->GetRegs().name.Y, inputVal);
            break;
        }
        case Instructions::Instr::DEC:
        {
            byte inputVal = this->dma.Read( argAddress );
            byte result = inputVal - 1;
            this->dma.Write(argAddress, result);
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::DEX:
        {
            byte result = this->GetRegs().name.X - 1;
            this->GetRegs().name.X = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::DEY:
        {
            byte result = this->GetRegs().name.Y - 1;
            this->GetRegs().name.Y = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::EOR:
        {
            byte inputVal = this->dma.Read( argAddress );
            byte result = this->GetRegs().name.A ^ inputVal;
            this->GetRegs().name.A = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INC:
        {
            byte inputVal = this->dma.Read( argAddress );
            byte result = inputVal + 1;
            this->dma.Write(argAddress, result);
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INX:
        {
            byte result = this->GetRegs().name.X + 1;
            this->GetRegs().name.X = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INY:
        {
            byte result = this->GetRegs().name.Y + 1;
            this->GetRegs().name.Y = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::JMP:
        {
            this->GetRegs().name.PC = argAddress;
            break;
        }
        case Instructions::Instr::JSR:
        {
            //store next address on stack
            dword returnMinus1 = this->GetRegs().name.PC - 1;
            this->Push(returnMinus1);
            this->GetRegs().name.PC = argAddress;
            break;
        }
        case Instructions::Instr::LDA:
        {
            byte inputVal = this->dma.Read( argAddress );
            this->GetRegs().name.A = inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::LDX:
        {
            byte inputVal = this->dma.Read( argAddress );
            this->GetRegs().name.X = inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.X);
            break;
        }
        case Instructions::Instr::LDY:
        {
            byte inputVal = this->dma.Read( argAddress );
            this->GetRegs().name.Y = inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.Y);
            break;
        }
        case Instructions::Instr::LSR:
        {
            byte inputVal;
            if(outputToAccum)
            {
                inputVal = this->GetRegs().name.A;
            }
            else
            {
                inputVal = this->dma.Read( argAddress );
            }
            this->GetRegs().name.C = BitUtil::GetBits(inputVal,0);
            byte result = inputVal >> 1;
            if(outputToAccum)
            {
                this->GetRegs().name.A = result;
            }
            else
            {
                this->dma.Write(argAddress, result);
            }
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::NOP:
        {
            break;
        }
        case Instructions::Instr::ORA:
        {
            byte inputVal = this->dma.Read( argAddress );
            this->GetRegs().name.A = this->GetRegs().name.A | inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::PHA:
        {
            this->Push(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::PHP:
        {
            // As stated here bit4 and bit5 are always set high for the PHP instruction:
            // https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
            byte statusTemp = this->GetRegs().name.P;
            statusTemp |= BitUtil::bit4;
            statusTemp |= BitUtil::bit5;
            this->Push(statusTemp);
            break;
        }
        case Instructions::Instr::PLA:
        {
            this->GetRegs().name.A = this->Pop();
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::PLP:
        {
            // TODO revisit this. should be ignoring bit 4 and bit 5 according to:
            // https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
            byte breakBitLow = (this->Pop() & 0xEF) | 0x20;
            this->GetRegs().name.P = breakBitLow;
           
            break;
        }
        case Instructions::Instr::ROL:
        {
            byte inputVal;
            if(outputToAccum)
            {
                inputVal = this->GetRegs().name.A;
            }
            else
            {
                inputVal = this->dma.Read( argAddress );
            }
            byte result = inputVal << 1;
            result = result | this->GetRegs().name.C;
            if(outputToAccum)
            {
                this->GetRegs().name.A = result;
            }
            else
            {
                this->dma.Write(argAddress, result);
            }
            this->GetRegs().name.C = BitUtil::GetBits(inputVal, 7);
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::ROR:
        {
            byte inputVal;
            if(outputToAccum)
            {
                inputVal = this->GetRegs().name.A;
            }
            else
            {
                inputVal = this->dma.Read( argAddress );
            }
            byte result = inputVal >> 1;
            result = result | (this->GetRegs().name.C << 7);
            if(outputToAccum)
            {
                this->GetRegs().name.A = result;
            }
            else
            {
                this->dma.Write(argAddress, result);
            }
            this->GetRegs().name.C = BitUtil::GetBits(inputVal, 0);
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::RTI:
        {
            // TODO: should be ignore bits 4 and 5 according to:
            // https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag

            this->GetRegs().name.P = this->Pop() | 0x20;
            this->GetRegs().name.PCL = this->Pop();
            this->GetRegs().name.PCH = this->Pop();
            break;
        }
        case Instructions::Instr::RTS:
        {
            this->GetRegs().name.PCL = this->Pop();
            this->GetRegs().name.PCH = this->Pop();
            this->GetRegs().name.PC += 1;
            break;
        }
        case Instructions::Instr::SBC:
        {
            byte inputVal = this->dma.Read( argAddress );
            byte oldAVal = this->GetRegs().name.A;
            this->GetRegs().name.A = this->GetRegs().name.A - (inputVal + (1 - this->GetRegs().name.C));
            this->UpdateRegsForOverflowNeg(oldAVal, inputVal);
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::SEC:
        {
            this->GetRegs().name.C = true;
            break;
        }
        case Instructions::Instr::SED:
        {
            this->GetRegs().name.D = true;
            break;
        }
        case Instructions::Instr::SEI:
        {
            this->GetRegs().name.I = true;
            break;
        }
        case Instructions::Instr::STA:
        {
            this->dma.Write(argAddress, this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::STX:
        {
            this->dma.Write(argAddress, this->GetRegs().name.X);
            break;
        }
        case Instructions::Instr::STY:
        {
            this->dma.Write(argAddress, this->GetRegs().name.Y);
            break;
        }
        case Instructions::Instr::TAX:
        {
            this->GetRegs().name.X = this->GetRegs().name.A;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.X);
            break;
        }
        case Instructions::Instr::TAY:
        {
            this->GetRegs().name.Y = this->GetRegs().name.A;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.Y);
            break;
        }
        case Instructions::Instr::TSX:
        {
            this->GetRegs().name.X = this->GetRegs().name.S;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.X);
            break;
        }
        case Instructions::Instr::TXA:
        {
            this->GetRegs().name.A = this->GetRegs().name.X;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::TXS:
        {
            this->GetRegs().name.S = this->GetRegs().name.X;
            break;
        }
        case Instructions::Instr::TYA:
        {
            this->GetRegs().name.A = this->GetRegs().name.Y;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        default:
        {
            std::cerr << "Unknown instruction at pc: " << std::hex << this->GetRegs().name.PC;
            break;
        }
    }

    //update cycle count
    this->totalClockCycles += cycleCount;
    return cycleCount;
}

int Cpu::HandleIrqEvent(bool verbose)
{
    if(this->GetRegs().name.I == false)
    {
        this->HandleInterrupt(InterruptType::irqCommand, verbose);
        int cyclesTaken = 7;
        return cyclesTaken;
    }
    return 0;
}

int Cpu::HandleNmiEvent(bool verbose)
{
    this->HandleInterrupt(InterruptType::nmiCommand, verbose);
    int cyclesTaken = 8;
    return cyclesTaken;
}

void Cpu::HandleInterrupt(InterruptType interruptType, bool verbose)
{
    dword interruptAddress;
    byte StateTemp = this->GetRegs().name.P;
    switch(interruptType)
    {
        case InterruptType::breakCommand:
        {
            interruptAddress = 0xFFFE;
            StateTemp |= BitUtil::bit4;
            break;
        }
        case InterruptType::irqCommand:
        {
            interruptAddress = 0xFFFC;
            break;
        }
        case InterruptType::nmiCommand:
        {
            interruptAddress = 0xFFFA;
            break;
        }
        default:
        {
            throw std::invalid_argument("unknown interrupt type");
            break;
        }
    }
    StateTemp |= BitUtil::bit5;

    if(verbose)
    {
        std::cout << "interrupt called, jumping to: " << interruptAddress << std::endl;
    }

    this->Push(this->GetRegs().name.PC);
    this->GetRegs().name.I = 1;
    this->Push(StateTemp);
    this->GetRegs().name.PC = interruptAddress;
}

void Cpu::Push(dword value)
{
    dword_p temp;
    temp.val = value;
    this->Push(temp.upper);
    this->Push(temp.lower);
}

void Cpu::Push(byte value)
{
    //write value to stack
    this->dma.Write(0x0100 + this->GetRegs().name.S , value);

    //update stack to next free byte
    this->GetRegs().name.S -= 1;
}

byte Cpu::Pop()
{
    //go back to next byte on the stack
    this->GetRegs().name.S += 1;
    //read value from stack
    byte value = this->dma.Read( 0x0100 + this->GetRegs().name.S);
    return value;
}


void Cpu::UpdateRegsForOverflow(byte oldAValue, byte inputVal)
{
    //convert to dwords so can detect overflow
    dword dInputVal = inputVal;
    dword dA = oldAValue;
    dword dC = this->GetRegs().name.C;
    dword dAValue = dInputVal + dA + dC;
    this->GetRegs().name.C = (dAValue > 255);

    int sInputVal = (sByte)inputVal;
    int sA = (sByte)oldAValue;
    int sAValue = sInputVal + sA + dC;
    this->GetRegs().name.V = (sAValue < -128) || (sAValue > 127);
}

void Cpu::UpdateRegsForOverflowNeg(byte oldAValue, byte inputVal)
{
    //convert to dwords so can detect overflow
    dword dInputVal = inputVal;
    dword dA = oldAValue;
    dword dC = this->GetRegs().name.C;
    dword dAValue = dA + ~dInputVal + dC;
    this->GetRegs().name.C = !(dAValue > 255);


    int sInputVal = (sByte)inputVal;
    int sA = (sByte)oldAValue;
    int sAValue = sA - sInputVal - ( 1 - dC );
    this->GetRegs().name.V = (sAValue < -128) || (sAValue > 127);
}



void Cpu::UpdateRegsForZeroAndNeg(byte inputVal)
{
    this->GetRegs().name.Z = (inputVal == 0);
    
    this->GetRegs().name.N = BitUtil::GetBits(inputVal, 7);
}

void Cpu::UpdateRegsForCompaire(byte value, byte inputVal)
{
    byte diff = value - inputVal;
    this->UpdateRegsForZeroAndNeg(diff);
    this->GetRegs().name.C = (value >= inputVal);
}

int Cpu::AdditionalCyclesForPageCross(dword address1, dword address2)
{
    if( (address1 & 0xFF00) != (address2 & 0xFF00) )
    {
        return 1;
    }
    return 0;
}

CpuRegisters& Cpu::GetRegs()
{
    return this->dma.GetCpuMemory().GetRegisters();
}

byte Cpu::SeekOrRead(dword address, bool seekOnly)
{
    if(seekOnly)
    {
        return this->dma.Seek(address);
    }
    else
    {
        return this->dma.Read(address);
    }
}

std::unique_ptr<Instructions::Instruction> Cpu::DecodeInstruction(dword pcAddress, bool seekOnly, bool verbose)
{
    try
    {
        byte optCode = this->dma.Seek(pcAddress);
        Instructions::Opcode opcode = Instructions::Opcodes[optCode];
        if( opcode.addrm == Instructions::AddrM::INVALID )
        {
            throw std::logic_error("not a valid instruction");
        }

        dword argAddress = 0;
        int instLen = 0;
        int cycleCount = opcode.cycles;
        bool outputToAccum = false;
        std::stringstream ss;

        //work out input value and input size
        switch(opcode.addrm)
        {
            //1 byte instructions
            case Instructions::AddrM::impl: //DEX
            {
                instLen = 1;
                if(verbose)
                {
                    ss << opcode.instr_name;
                }
                break;
            }
            case Instructions::AddrM::acc: //LSR A
            {
                instLen = 1;
                outputToAccum = true;
                if(verbose)
                {
                    ss << opcode.instr_name << " A";
                }
                break;
            }

            //2 byte instructions
            case Instructions::AddrM::imm: //ORA #$AA
            {
                instLen = 2;
                argAddress = pcAddress + 1;
                if(verbose)
                {
                    byte inputVal = this->dma.Seek(argAddress);
                    ss << opcode.instr_name << std::hex << " #$" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)inputVal;
                }
                break;
            }
            case Instructions::AddrM::zpg_: //ORA $78 = 00
            {
                instLen = 2;
                argAddress = this->SeekOrRead(pcAddress + 1, seekOnly);
                if(verbose)
                {
                    byte inputVal = this->dma.Seek(argAddress);
                    ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << argAddress << " = " << std::setw(2) << (int)inputVal;
                }
                break;
            }
            case Instructions::AddrM::zpgX: //ORA $00,X @ 78 = 00
            {
                instLen = 2;
                dword lookupAddress = this->SeekOrRead(pcAddress + 1, seekOnly);
                argAddress = lookupAddress + this->GetRegs().name.X;
                argAddress = argAddress % 0x100;
                if(verbose)
                {
                    byte inputVal = this->dma.Seek(argAddress);
                    ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << lookupAddress << ",X @ " << std::setw(2) << argAddress << " = " << std::setw(2) << (int)inputVal;
                }
                break;
            }
            case Instructions::AddrM::zpgY: //ORA $00,X @ 78 = 00
            {
                instLen = 2;
                dword lookupAddress = this->SeekOrRead(pcAddress + 1, seekOnly);
                argAddress = lookupAddress + this->GetRegs().name.Y;
                argAddress = argAddress % 0x100;
                if(verbose)
                {
                    byte inputVal = this->dma.Seek(argAddress);
                    ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << lookupAddress << ",Y @ " << std::setw(2) << argAddress << " = " << std::setw(2) << (int)inputVal;
                }
                break;
            }
            case Instructions::AddrM::rel: //BCS $F815
            {
                instLen = 2;
                sByte offset = this->SeekOrRead(pcAddress + 1, seekOnly);
                argAddress = offset + pcAddress + 2;
                if(verbose)
                {
                    ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << argAddress;
                }
                break;
            }
            case Instructions::AddrM::Xind: //CMP ($80,X) @ 80 = 0200 = 40
            {
                instLen = 2;
                byte indirrectAddress = this->SeekOrRead( pcAddress + 1, seekOnly);
                byte indirrectAddressX = indirrectAddress + this->GetRegs().name.X;
                argAddress = BitUtil::GetDWord(&this->dma, indirrectAddressX, true, !seekOnly);
                if(verbose)
                {
                    byte inputVal = this->dma.Seek( argAddress ); 
                    ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)indirrectAddress << ",X) @ " << std::setw(2) << (int)indirrectAddressX << " = " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
                }
                break;
            }
            case Instructions::AddrM::indY: //LDA ($97),Y = FFFF @ 0033 = A3
            {
                instLen = 2;
                byte indirrectAddress = this->SeekOrRead( pcAddress + 1, seekOnly);
                dword inDirAddress = BitUtil::GetDWord(&this->dma, indirrectAddress, true, !seekOnly);
                argAddress = inDirAddress + this->GetRegs().name.Y;
                if(opcode.instr != Instructions::Instr::STA)
                {
                    cycleCount += this->AdditionalCyclesForPageCross(inDirAddress, argAddress);
                }
                if(verbose)
                {
                    byte inputVal = this->dma.Seek(argAddress); 
                    ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)indirrectAddress << "),Y = " << std::setw(4) << inDirAddress << " @ " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
                }
                break;
            }

            //3 byte instructions
            case Instructions::AddrM::abs_: //LDA $0400 = 87
            {
                instLen = 3;
                argAddress = BitUtil::GetDWord(&this->dma, pcAddress + 1, false, !seekOnly);
                if(verbose)
                {
                    byte inputVal = this->dma.Seek(argAddress);
                    ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << argAddress;
                    if(opcode.instr != Instructions::Instr::JMP && opcode.instr != Instructions::Instr::JSR)
                    {
                        ss << " = " << std::hex << std::setw(2) << std::uppercase << (int) inputVal;
                    }
                }
                break;
            }
            case Instructions::AddrM::absX: //LDA $05FF,X @ 0689 = BB
            {
                instLen = 3;
                dword lookupAddress = BitUtil::GetDWord(&this->dma, pcAddress + 1, false, !seekOnly);
                argAddress = lookupAddress + this->GetRegs().name.X;
                if(opcode.instr != Instructions::Instr::STA)
                {
                    cycleCount += this->AdditionalCyclesForPageCross(lookupAddress, argAddress);
                }
                if(verbose)
                {
                    byte inputVal = this->dma.Seek(argAddress); 
                    ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << lookupAddress << ",X @ " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
                }
                break;
            }
            case Instructions::AddrM::absY: //LDA $0300,Y @ 0300 = 89
            {
                instLen = 3;
                dword lookupAddress = BitUtil::GetDWord(&this->dma, pcAddress + 1, false, !seekOnly);
                argAddress = lookupAddress + this->GetRegs().name.Y;
                if(opcode.instr != Instructions::Instr::STA)
                {
                    cycleCount += this->AdditionalCyclesForPageCross(lookupAddress, argAddress);
                }
                if(verbose)
                {
                    byte inputVal = this->dma.Seek(argAddress); 
                    ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::uppercase << std::right << lookupAddress << ",Y @ " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
                }
                break;
            }
            case Instructions::AddrM::ind_: //JMP ($0200) = DB7E
            {
                instLen = 3;
                dword indirrectAddress = BitUtil::GetDWord(&this->dma, pcAddress + 1, false, !seekOnly);
                argAddress = BitUtil::GetDWord(&this->dma, indirrectAddress, true, !seekOnly);
                if(verbose)
                {
                    ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << indirrectAddress << ") = " << std::setw(4) << argAddress;
                }
                break;
            }
            default:
            {
                std::cerr << "invalid memory mode" << std::endl;
                break;
            }
        }

        std::unique_ptr<Instructions::Instruction> decodedInst = std::unique_ptr<Instructions::Instruction>( new Instructions::Instruction() );
        decodedInst->opcode = opcode;
        decodedInst->pcAddress = pcAddress;
        decodedInst->argAddress = argAddress;
        decodedInst->instLen = instLen;
        decodedInst->cycleCount = cycleCount;
        decodedInst->outputToAccum = outputToAccum;
        decodedInst->addressModeText = ss.str();
        return decodedInst;
    }
    catch(const std::exception& e)
    {
        //if failed to decode instruction then return NULL
        return NULL;
    }
}

std::string Cpu::GenerateCpuScreen(int instructionsBefore, int instructionsAfter)
{
    dword pc = this->GetRegs().name.PC;
    std::vector<Instructions::Instruction> decodedInstructions;
    int currentInstrIndex = -1;
    std::shared_ptr<MemoryRepeaterVec> prgRom = this->dma.GetCartridge()->GetPrgRom();
    int i = prgRom->startAddress;
    while(i < prgRom->endAddress)
    {
        if(i == pc)
        {
            currentInstrIndex = decodedInstructions.size();
        }

        std::unique_ptr<Instructions::Instruction> decodedInstr = this->DecodeInstruction(i, true, true);
        if(decodedInstr == NULL)
        {
            decodedInstr = std::unique_ptr<Instructions::Instruction>(new Instructions::Instruction());
            decodedInstr->pcAddress = i;
            decodedInstr->opcode.instr_name = "NOP*";
            decodedInstr->instLen = 1;
        }
        decodedInstructions.push_back(*decodedInstr);
        i += decodedInstr->instLen;
    }
    int lastInstructions = std::max(currentInstrIndex - instructionsBefore, 0);
    int nextInstructions = std::min(currentInstrIndex + instructionsAfter, (int)decodedInstructions.size());
    
    std::stringstream ss;
    for(int i = lastInstructions; i < nextInstructions; ++i)
    {
        const Instructions::Instruction& curInstr = decodedInstructions.at(i);
        if(i == currentInstrIndex)
        {
            ss << "->";
        }
        else
        {
            ss << "  ";
        }
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << std::right << "0x" << curInstr.pcAddress << " " << curInstr.addressModeText << std::endl;
    }
    ss << "CYC:" << std::dec << (int) this->totalClockCycles << " ";
    ss << "A:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.A << " ";
    ss << "X:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.X << " ";
    ss << "Y:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.Y << " ";
    ss << "P:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.P << " ";
    ss << "SP:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->GetRegs().name.S << " ";
    return ss.str();
}

void Cpu::SetTotalClockCycles(int totalCycles)
{
    this->totalClockCycles = totalCycles;
}