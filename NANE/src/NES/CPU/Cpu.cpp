#include "Cpu.h"

#include <memory> //std::unique_ptr
#include <iostream> //std::cout
#include <sstream> //std::stringstream
#include <iomanip> //std::setfill and std::setw

Cpu::Cpu(Dma& dma)
: dma(dma)
{
}

bool Cpu::PowerCycle(dword newPcAddress)
{
    this->GetRegs().name.P = 0x34;
    this->GetRegs().name.A = 0x0;
    this->GetRegs().name.X = 0x0;
    this->GetRegs().name.Y = 0x0;
    this->GetRegs().name.Y = 0xFD;
    this->dma.CpuWrite(0x4017, 0x0);
    this->dma.CpuWrite(0x4015, 0x0);
    for(int i = 0x4000; i < 0x400F; ++i)
    {
        this->dma.CpuWrite(i, 0x0);
    }
    for(int i = 0x4010; i < 0x4013; ++i)
    {
        this->dma.CpuWrite(i, 0x0);
    }
    this->GetRegs().name.PC = newPcAddress;
    return true;
}

int Cpu::Step(bool verbose)
{
    std::unique_ptr<Instructions::Instruction> decodedInst = this->DecodeInstruction(this->GetRegs().name.PC);
    if(decodedInst == NULL)
    {
        std::cerr << "failed to decode instruction at " << this->GetRegs().name.PC << std::endl;
        ++this->GetRegs().name.PC;
        return 0;
    }
    Instructions::Opcode opcode = decodedInst->opcode;
    byte inputVal = decodedInst->inputVal;
    dword argAddress = decodedInst->argAddress;
    int instLen = decodedInst->instLen;
    int cycleCount = decodedInst->cycleCount;
    bool outputToAccum = decodedInst->outputToAccum;
    std::string addressModeText = decodedInst->addressModeText;

    //print to display
    if(verbose == true)
    {
        std::stringstream instCodeSS;
        for(int i = 0; i < instLen; ++i)
        {
            instCodeSS << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << std::right << (int) this->dma.CpuRead(this->GetRegs().name.PC + i) << " ";
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
        std::cout << "CYC:" << std::dec << (int) this->totalClockCycles;
        std::cout << std::endl;
    }

    //increment PC
    this->GetRegs().name.PC += instLen;

    //execute instructions
    switch(opcode.instr)
    {
        case Instructions::Instr::ADC:
        {
            byte oldA = this->GetRegs().name.A;
            this->GetRegs().name.A = this->GetRegs().name.A + this->GetRegs().name.C + inputVal;
            this->UpdateRegsForOverflow(oldA, inputVal);
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::AND:
        {
            this->GetRegs().name.A = this->GetRegs().name.A & inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::ASL:
        {
            this->GetRegs().name.C = BitUtil::GetBits(inputVal,7);
            byte result = inputVal << 1;
            if(outputToAccum)
            {
                this->GetRegs().name.A = result;
            }
            else
            {
                this->dma.CpuWrite(argAddress, result);
            }
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
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
            std::cerr << "Haven't implemented break command" << std::endl;
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
            this->UpdateRegsForCompaire(this->GetRegs().name.A, inputVal);
            break;
        }
        case Instructions::Instr::CPX:
        {
            this->UpdateRegsForCompaire(this->GetRegs().name.X, inputVal);
            break;
        }
        case Instructions::Instr::CPY:
        {
            this->UpdateRegsForCompaire(this->GetRegs().name.Y, inputVal);
            break;
        }
        case Instructions::Instr::DEC:
        {
            byte result = inputVal - 1;
            this->dma.CpuWrite(argAddress, result);
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
            byte result = this->GetRegs().name.A ^ inputVal;
            this->GetRegs().name.A = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INC:
        {
            byte result = inputVal + 1;
            this->dma.CpuWrite(argAddress, result);
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
            this->GetRegs().name.A = inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::LDX:
        {
            this->GetRegs().name.X = inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.X);
            break;
        }
        case Instructions::Instr::LDY:
        {
            this->GetRegs().name.Y = inputVal;
            this->UpdateRegsForZeroAndNeg(this->GetRegs().name.Y);
            break;
        }
        case Instructions::Instr::LSR:
        {
            this->GetRegs().name.C = BitUtil::GetBits(inputVal,0);
            byte result = inputVal >> 1;
            if(outputToAccum)
            {
                this->GetRegs().name.A = result;
            }
            else
            {
                this->dma.CpuWrite(argAddress, result);
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
            //magic value from nestest
            byte setBreakHigh = this->GetRegs().name.P | 0x30;
            this->Push(setBreakHigh);
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
            //from nestest rom
            byte breakBitLow = (this->Pop() & 0xEF) | 0x20;
            this->GetRegs().name.P = breakBitLow;
           
            break;
        }
        case Instructions::Instr::ROL:
        {
            byte result = inputVal << 1;
            result = result | this->GetRegs().name.C;
            if(outputToAccum)
            {
                this->GetRegs().name.A = result;
            }
            else
            {
                this->dma.CpuWrite(argAddress, result);
            }
            this->GetRegs().name.C = BitUtil::GetBits(inputVal, 7);
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::ROR:
        {
            byte result = inputVal >> 1;
            result = result | (this->GetRegs().name.C << 7);
            if(outputToAccum)
            {
                this->GetRegs().name.A = result;
            }
            else
            {
                this->dma.CpuWrite(argAddress, result);
            }
            this->GetRegs().name.C = BitUtil::GetBits(inputVal, 0);
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::RTI:
        {
            this->GetRegs().name.P = this->Pop() | 0x20; //nestest
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
            this->dma.CpuWrite(argAddress, this->GetRegs().name.A);
            break;
        }
        case Instructions::Instr::STX:
        {
            this->dma.CpuWrite(argAddress, this->GetRegs().name.X);
            break;
        }
        case Instructions::Instr::STY:
        {
            this->dma.CpuWrite(argAddress, this->GetRegs().name.Y);
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
    this->dma.CpuWrite(0x0100 + this->GetRegs().name.S , value);

    //update stack to next free byte
    this->GetRegs().name.S -= 1;
}

byte Cpu::Pop()
{
    //go back to next byte on the stack
    this->GetRegs().name.S += 1;
    //read value from stack
    byte value = this->dma.CpuRead( 0x0100 + this->GetRegs().name.S);
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

std::unique_ptr<Instructions::Instruction> Cpu::DecodeInstruction(dword pcAddress)
{
    try
    {
        byte optCode = this->dma.CpuRead(pcAddress);
        Instructions::Opcode opcode = Instructions::Opcodes[optCode];
        if( opcode.addrm == Instructions::AddrM::INVALID )
        {
            throw std::logic_error("not a valid instruction");
        }

        byte inputVal = 0;
        dword argAddress = 0;
        std::stringstream ss;
        int instLen = 0;
        int cycleCount = opcode.cycles;
        bool outputToAccum = false;

        //work out input value and input size
        switch(opcode.addrm)
        {
            //1 byte instructions
            case Instructions::AddrM::impl:
            {
                instLen = 1;
                //DEX
                ss << opcode.instr_name;
                break;
            }
            case Instructions::AddrM::acc:
            {
                instLen = 1;
                inputVal = this->GetRegs().name.A;
                outputToAccum = true;
                //LSR A
                ss << opcode.instr_name << " A";
                break;
            }

            //2 byte instructions
            case Instructions::AddrM::imm:
            {
                instLen = 2;
                inputVal = this->dma.CpuRead(pcAddress + 1);
                //ORA #$AA
                ss << opcode.instr_name << std::hex << " #$" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)inputVal;
                break;
            }
            case Instructions::AddrM::zpg_:
            {
                instLen = 2;
                argAddress = this->dma.CpuRead(pcAddress + 1);
                inputVal = this->dma.CpuRead(argAddress);
                //ORA $78 = 00
                ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << argAddress << " = " << std::setw(2) << (int)inputVal;
                break;
            }
            case Instructions::AddrM::zpgX:
            {
                instLen = 2;
                dword lookupAddress = this->dma.CpuRead(pcAddress + 1);
                argAddress = lookupAddress + this->GetRegs().name.X;
                argAddress = argAddress % 0x100;
                inputVal = this->dma.CpuRead(argAddress);
                //ORA $00,X @ 78 = 00
                ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << lookupAddress << ",X @ " << std::setw(2) << argAddress << " = " << std::setw(2) << (int)inputVal;
                break;
            }
            case Instructions::AddrM::zpgY:
            {
                instLen = 2;
                dword lookupAddress = this->dma.CpuRead(pcAddress + 1);
                argAddress = lookupAddress + this->GetRegs().name.Y;
                argAddress = argAddress % 0x100;
                inputVal = this->dma.CpuRead(argAddress);
                //ORA $00,X @ 78 = 00
                ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << lookupAddress << ",Y @ " << std::setw(2) << argAddress << " = " << std::setw(2) << (int)inputVal;
                break;
            }
            case Instructions::AddrM::rel:
            {
                instLen = 2;
                sByte offset = this->dma.CpuRead(pcAddress + 1);
                argAddress = offset + pcAddress + 2;
                //BCS $F815
                ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << argAddress;
                break;
            }
            case Instructions::AddrM::Xind:
            {
                instLen = 2;
                byte indirrectAddress = this->dma.CpuRead( pcAddress + 1);
                byte indirrectAddressX = indirrectAddress + this->GetRegs().name.X;
                argAddress = BitUtil::GetDWord(&this->dma, indirrectAddressX, true);
                inputVal = this->dma.CpuRead( argAddress );
                //CMP ($80,X) @ 80 = 0200 = 40
                ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)indirrectAddress << ",X) @ " << std::setw(2) << (int)indirrectAddressX << " = " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
                break;
            }
            case Instructions::AddrM::indY:
            {
                instLen = 2;
                byte indirrectAddress = this->dma.CpuRead( pcAddress + 1);
                dword inDirAddress = BitUtil::GetDWord(&this->dma, indirrectAddress, true);
                argAddress = inDirAddress + this->GetRegs().name.Y;
                inputVal = this->dma.CpuRead( argAddress );
                if(opcode.instr != Instructions::Instr::STA)
                {
                    cycleCount += this->AdditionalCyclesForPageCross(inDirAddress, argAddress);
                }
                //LDA ($97),Y = FFFF @ 0033 = A3
                ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)indirrectAddress << "),Y = " << std::setw(4) << inDirAddress << " @ " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
                break;
            }

            //3 byte instructions
            case Instructions::AddrM::abs_:
            {
                instLen = 3;
                argAddress = BitUtil::GetDWord(&this->dma, pcAddress + 1);
                inputVal = this->dma.CpuRead(argAddress);
                //LDA $0400 = 87
                ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << argAddress;
                if(opcode.instr != Instructions::Instr::JMP && opcode.instr != Instructions::Instr::JSR)
                {
                    ss << " = " << std::hex << std::setw(2) << std::uppercase << (int) inputVal;
                }
                break;
            }
            case Instructions::AddrM::absX:
            {
                instLen = 3;
                dword lookupAddress = BitUtil::GetDWord(&this->dma, pcAddress + 1);
                argAddress = lookupAddress + this->GetRegs().name.X;
                inputVal = this->dma.CpuRead(argAddress);
                if(opcode.instr != Instructions::Instr::STA)
                {
                    cycleCount += this->AdditionalCyclesForPageCross(lookupAddress, argAddress);
                }
                //LDA $05FF,X @ 0689 = BB
                ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << lookupAddress << ",X @ " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
                break;
            }
            case Instructions::AddrM::absY:
            {
                instLen = 3;
                dword lookupAddress = BitUtil::GetDWord(&this->dma, pcAddress + 1);
                argAddress = lookupAddress + this->GetRegs().name.Y;
                inputVal = this->dma.CpuRead(argAddress);
                if(opcode.instr != Instructions::Instr::STA)
                {
                    cycleCount += this->AdditionalCyclesForPageCross(lookupAddress, argAddress);
                }
                //LDA $0300,Y @ 0300 = 89
                ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::uppercase << std::right << lookupAddress << ",Y @ " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
                break;
            }
            case Instructions::AddrM::ind_:
            {
                instLen = 3;
                dword indirrectAddress = BitUtil::GetDWord(&this->dma, pcAddress + 1);
                argAddress = BitUtil::GetDWord(&this->dma, indirrectAddress, true);
                //JMP ($0200) = DB7E
                ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << indirrectAddress << ") = " << std::setw(4) << argAddress;
                break;
            }
            default:
            {
                std::cerr << "invalid memory mode" << std::endl;
                //default to zero page addressing
                instLen = 2;
                argAddress = this->dma.CpuRead(pcAddress + 1);
                inputVal = this->dma.CpuRead(argAddress);
                //ORA $78 = 00
                ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << argAddress << " = " << std::setw(2) << (int)inputVal;
                break;
            }
        }
        std::string addressModeText = ss.str();

        std::unique_ptr<Instructions::Instruction> decodedInst = std::unique_ptr<Instructions::Instruction>( new Instructions::Instruction() );
        decodedInst->opcode = opcode;
        decodedInst->inputVal = inputVal;
        decodedInst->pcAddress = pcAddress;
        decodedInst->argAddress = argAddress;
        decodedInst->instLen = instLen;
        decodedInst->cycleCount = cycleCount;
        decodedInst->outputToAccum = outputToAccum;
        decodedInst->addressModeText = addressModeText;
        return decodedInst;
    }
    catch(const std::exception& e)
    {
        return NULL;
    }
}

std::string Cpu::GenerateCpuScreen(int instructionsBefore, int instructionsAfter)
{
    dword pc = this->GetRegs().name.PC;
    std::vector<Instructions::Instruction> decodedInstructions;
    int currentInstrIndex = -1;
    std::unique_ptr<MemoryRepeaterVec>& prgRom = this->dma.GetCartridge()->GetPrgRom();
    int i = prgRom->startAddress;
    while(i < prgRom->endAddress)
    {
        if(i == pc)
        {
            currentInstrIndex = decodedInstructions.size();
        }

        std::unique_ptr<Instructions::Instruction> decodedInstr = this->DecodeInstruction(i);
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