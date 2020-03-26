#include "Cpu.h"

Cpu::Cpu(std::unique_ptr<CpuRegisters> cpuRegisters, std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
{
    this->totalClockCycles = 0;
    this->registers = std::move(cpuRegisters);
    this->memory = std::unique_ptr<CpuMemoryMap>( new CpuMemoryMap(ppuRegisters, apuRegisters) );
}

bool Cpu::PowerCycle()
{
    this->registers->name.P = 0x34;
    this->registers->name.A = 0x0;
    this->registers->name.X = 0x0;
    this->registers->name.Y = 0x0;
    this->registers->name.Y = 0xFD;
    this->memory->Write(0x4017, 0x0);
    this->memory->Write(0x4015, 0x0);
    for(int i = 0x4000; i < 0x400F; ++i)
    {
        this->memory->Write(i, 0x0);
    }
    for(int i = 0x4010; i < 0x4013; ++i)
    {
        this->memory->Write(i, 0x0);
    }

    this->registers->name.PC = 0xC000;
    return true;
}

int Cpu::Step()
{
    //get optCode from memory
    byte optCode = this->memory->Read(this->registers->name.PC);

    Instructions::Opcode opcode = Instructions::Opcodes[optCode];

    if( opcode.addrm == Instructions::AddrM::INVALID )
    {
        std::cerr << "failed to read instruction" << std::endl;
    }

    byte inputVal = 0;
    dword address = 0;
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
            inputVal = this->registers->name.A;
            outputToAccum = true;
            //LSR A
            ss << opcode.instr_name << " A";
            break;
        }

        //2 byte instructions
        case Instructions::AddrM::imm:
        {
            instLen = 2;
            inputVal = this->memory->Read(this->registers->name.PC + 1);
            //ORA #$AA
            ss << opcode.instr_name << std::hex << " #$" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)inputVal;
            break;
        }
        case Instructions::AddrM::zpg_:
        {
            instLen = 2;
            address = this->memory->Read(this->registers->name.PC + 1);
            inputVal = this->memory->Read(address);
            //ORA $78 = 00
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << address << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::zpgX:
        {
            instLen = 2;
            dword argAddress = this->memory->Read(this->registers->name.PC + 1);
            address = argAddress + this->registers->name.X;
            address = address % 0x100;
            inputVal = this->memory->Read(address);
            //ORA $00,X @ 78 = 00
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << argAddress << ",X @ " << std::setw(2) << address << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::zpgY:
        {
            instLen = 2;
            dword argAddress = this->memory->Read(this->registers->name.PC + 1);
            address = argAddress + this->registers->name.Y;
            address = address % 0x100;
            inputVal = this->memory->Read(address);
            //ORA $00,X @ 78 = 00
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << argAddress << ",Y @ " << std::setw(2) << address << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::rel:
        {
            instLen = 2;
            sByte offset = this->memory->Read(this->registers->name.PC + 1);
            address = offset + this->registers->name.PC + 2;
            //BCS $F815
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << address;
            break;
        }
        case Instructions::AddrM::Xind:
        {
            instLen = 2;
            byte indirrectAddress = this->memory->Read( this->registers->name.PC + 1);
            byte indirrectAddressX = indirrectAddress + this->registers->name.X;
            address = BitUtil::GetDWord(this->memory.get(), indirrectAddressX, true);
            inputVal = this->memory->Read( address );
            //CMP ($80,X) @ 80 = 0200 = 40
            ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)indirrectAddress << ",X) @ " << std::setw(2) << (int)indirrectAddressX << " = " << std::setw(4) << address << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::indY:
        {
            instLen = 2;
            byte indirrectAddress = this->memory->Read( this->registers->name.PC + 1);
            dword inDirAddress = BitUtil::GetDWord(this->memory.get(), indirrectAddress, true);
            address = inDirAddress + this->registers->name.Y;
            inputVal = this->memory->Read( address );
            if(opcode.instr != Instructions::Instr::STA)
            {
                cycleCount += this->AdditionalCyclesForPageCross(inDirAddress, address);
            }
            //LDA ($97),Y = FFFF @ 0033 = A3
            ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)indirrectAddress << "),Y = " << std::setw(4) << inDirAddress << " @ " << std::setw(4) << address << " = " << std::setw(2) << (int)inputVal;
            break;
        }

        //3 byte instructions
        case Instructions::AddrM::abs_:
        {
            instLen = 3;
            address = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
            inputVal = this->memory->Read(address);
            //LDA $0400 = 87
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << address;
            if(opcode.instr != Instructions::Instr::JMP && opcode.instr != Instructions::Instr::JSR)
            {
                ss << " = " << std::hex << std::setw(2) << std::uppercase << (int) inputVal;
            }
            break;
        }
        case Instructions::AddrM::absX:
        {
            instLen = 3;
            dword argAddress = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
            address = argAddress + this->registers->name.X;
            inputVal = this->memory->Read(address);
            if(opcode.instr != Instructions::Instr::STA)
            {
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, address);
            }
            //LDA $05FF,X @ 0689 = BB
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << argAddress << ",X @ " << std::setw(4) << address << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::absY:
        {
            instLen = 3;
            dword argAddress = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
            address = argAddress + this->registers->name.Y;
            inputVal = this->memory->Read(address);
            if(opcode.instr != Instructions::Instr::STA)
            {
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, address);
            }
            //LDA $0300,Y @ 0300 = 89
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::uppercase << std::right << argAddress << ",Y @ " << std::setw(4) << address << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::ind_:
        {
            instLen = 3;
            dword indirrectAddress = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
            address = BitUtil::GetDWord(this->memory.get(), indirrectAddress, true);
            //JMP ($0200) = DB7E
            ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << indirrectAddress << ") = " << std::setw(4) << address;
            break;
        }
        default:
        {
            std::cerr << "invalid memory mode" << std::endl;
            //default to zero page addressing
            instLen = 2;
            address = this->memory->Read(this->registers->name.PC + 1);
            inputVal = this->memory->Read(address);
            //ORA $78 = 00
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << address << " = " << std::setw(2) << (int)inputVal;
            break;
        }
    }

    //print to display
    std::stringstream instCodeSS;
    for(int i = 0; i < instLen; ++i)
    {
        instCodeSS << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << std::right << (int) this->memory->Read(this->registers->name.PC + i) << " ";
    }
    std::string instCodeText = instCodeSS.str();
    std::string addressModeText = ss.str();
    std::cout<< std::hex << std::setfill('0') << std::setw(4) << std::right << std::uppercase << this->registers->name.PC << "  ";
    std::cout << std::setw (10) << std::left << std::setfill(' ') << instCodeText;
    std::cout << std::setw (32) << std::left << addressModeText;
    std::cout << "A:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->registers->name.A << " ";
    std::cout << "X:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->registers->name.X << " ";
    std::cout << "Y:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->registers->name.Y << " ";
    std::cout << "P:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->registers->name.P << " ";
    std::cout << "SP:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->registers->name.S << " ";
    std::cout << "PPU:" << std::dec << std::setfill(' ') << std::setw(3) << std::right << (int) 0 << ",";
    std::cout << std::dec << std::setfill(' ') << std::setw(3) << std::right << (int) 0 << " ";
    std::cout << "CYC:" << std::dec << (int) this->totalClockCycles;
    std::cout << std::endl;

    //increment PC
    this->registers->name.PC += instLen;

    //execute instructions
    switch(opcode.instr)
    {
        case Instructions::Instr::ADC:
        {
            byte oldA = this->registers->name.A;
            this->registers->name.A = this->registers->name.A + this->registers->name.C + inputVal;
            this->UpdateRegsForOverflow(oldA, inputVal);
            this->UpdateRegsForZeroAndNeg(this->registers->name.A);
            break;
        }
        case Instructions::Instr::AND:
        {
            this->registers->name.A = this->registers->name.A & inputVal;
            this->UpdateRegsForZeroAndNeg(this->registers->name.A);
            break;
        }
        case Instructions::Instr::ASL:
        {
            this->registers->name.C = BitUtil::GetBits(inputVal,7);
            byte result = inputVal << 1;
            if(outputToAccum)
            {
                this->registers->name.A = result;
            }
            else
            {
                this->memory->Write(address, result);
            }
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::BCC:
        {
            if( this->registers->name.C == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(address, this->registers->name.PC);
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BCS:
        {
            if( this->registers->name.C == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(address, this->registers->name.PC);
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BEQ:
        {
            if( this->registers->name.Z == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(address, this->registers->name.PC);
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BIT:
        {
            byte result = this->registers->name.A & inputVal;
            this->UpdateRegsForZeroAndNeg(result);
            this->registers->name.V = BitUtil::GetBits(inputVal, 6);
            this->registers->name.N = BitUtil::GetBits(inputVal, 7);
            break;
        }
        case Instructions::Instr::BMI:
        {
            if( this->registers->name.N == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(address, this->registers->name.PC);
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BNE:
        {
            if( this->registers->name.Z == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(address, this->registers->name.PC);
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BPL:
        {
            byte p = this->registers->name.P;
            if( this->registers->name.N == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(address, this->registers->name.PC);
                this->registers->name.PC = address;
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
            if( this->registers->name.V == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(address, this->registers->name.PC);
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BVS:
        {
            if( this->registers->name.V == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(address, this->registers->name.PC);
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::CLC:
        {
            this->registers->name.C = false;
            break;
        }
        case Instructions::Instr::CLD:
        {
            this->registers->name.D = false;
            break;
        }
        case Instructions::Instr::CLI:
        {
            this->registers->name.I = false;
            break;
        }
        case Instructions::Instr::CLV:
        {
            this->registers->name.V = false;
            break;
        }
        case Instructions::Instr::CMP:
        {
            this->UpdateRegsForCompaire(this->registers->name.A, inputVal);
            break;
        }
        case Instructions::Instr::CPX:
        {
            this->UpdateRegsForCompaire(this->registers->name.X, inputVal);
            break;
        }
        case Instructions::Instr::CPY:
        {
            this->UpdateRegsForCompaire(this->registers->name.Y, inputVal);
            break;
        }
        case Instructions::Instr::DEC:
        {
            byte result = inputVal - 1;
            this->memory->Write(address, result);
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::DEX:
        {
            byte result = this->registers->name.X - 1;
            this->registers->name.X = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::DEY:
        {
            byte result = this->registers->name.Y - 1;
            this->registers->name.Y = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::EOR:
        {
            byte result = this->registers->name.A ^ inputVal;
            this->registers->name.A = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INC:
        {
            byte result = inputVal + 1;
            this->memory->Write(address, result);
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INX:
        {
            byte result = this->registers->name.X + 1;
            this->registers->name.X = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INY:
        {
            byte result = this->registers->name.Y + 1;
            this->registers->name.Y = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::JMP:
        {
            this->registers->name.PC = address;
            break;
        }
        case Instructions::Instr::JSR:
        {
            //store next address on stack
            dword returnMinus1 = this->registers->name.PC - 1;
            this->Push(returnMinus1);
            this->registers->name.PC = address;
            break;
        }
        case Instructions::Instr::LDA:
        {
            this->registers->name.A = inputVal;
            this->UpdateRegsForZeroAndNeg(this->registers->name.A);
            break;
        }
        case Instructions::Instr::LDX:
        {
            this->registers->name.X = inputVal;
            this->UpdateRegsForZeroAndNeg(this->registers->name.X);
            break;
        }
        case Instructions::Instr::LDY:
        {
            this->registers->name.Y = inputVal;
            this->UpdateRegsForZeroAndNeg(this->registers->name.Y);
            break;
        }
        case Instructions::Instr::LSR:
        {
            this->registers->name.C = BitUtil::GetBits(inputVal,0);
            byte result = inputVal >> 1;
            if(outputToAccum)
            {
                this->registers->name.A = result;
            }
            else
            {
                this->memory->Write(address, result);
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
            this->registers->name.A = this->registers->name.A | inputVal;
            this->UpdateRegsForZeroAndNeg(this->registers->name.A);
            break;
        }
        case Instructions::Instr::PHA:
        {
            this->Push(this->registers->name.A);
            break;
        }
        case Instructions::Instr::PHP:
        {
            //magic value from nestest
            byte setBreakHigh = this->registers->name.P | 0x30;
            this->Push(setBreakHigh);
            break;
        }
        case Instructions::Instr::PLA:
        {
            this->registers->name.A = this->Pop();
            this->UpdateRegsForZeroAndNeg(this->registers->name.A);
            break;
        }
        case Instructions::Instr::PLP:
        {
            //from nestest rom
            byte breakBitLow = (this->Pop() & 0xEF) | 0x20;
            this->registers->name.P = breakBitLow;
           
            break;
        }
        case Instructions::Instr::ROL:
        {
            byte result = inputVal << 1;
            result = result | this->registers->name.C;
            if(outputToAccum)
            {
                this->registers->name.A = result;
            }
            else
            {
                this->memory->Write(address, result);
            }
            this->registers->name.C = BitUtil::GetBits(inputVal, 7);
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::ROR:
        {
            byte result = inputVal >> 1;
            result = result | (this->registers->name.C << 7);
            if(outputToAccum)
            {
                this->registers->name.A = result;
            }
            else
            {
                this->memory->Write(address, result);
            }
            this->registers->name.C = BitUtil::GetBits(inputVal, 0);
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::RTI:
        {
            this->registers->name.P = this->Pop() | 0x20; //nestest
            this->registers->name.PCL = this->Pop();
            this->registers->name.PCH = this->Pop();
            break;
        }
        case Instructions::Instr::RTS:
        {
            this->registers->name.PCL = this->Pop();
            this->registers->name.PCH = this->Pop();
            this->registers->name.PC += 1;
            break;
        }
        case Instructions::Instr::SBC:
        {
            byte oldAVal = this->registers->name.A;
            this->registers->name.A = this->registers->name.A - (inputVal + (1 - this->registers->name.C));
            this->UpdateRegsForOverflowNeg(oldAVal, inputVal);
            this->UpdateRegsForZeroAndNeg(this->registers->name.A);
            break;
        }
        case Instructions::Instr::SEC:
        {
            this->registers->name.C = true;
            break;
        }
        case Instructions::Instr::SED:
        {
            this->registers->name.D = true;
            break;
        }
        case Instructions::Instr::SEI:
        {
            this->registers->name.I = true;
            break;
        }
        case Instructions::Instr::STA:
        {
            this->memory->Write(address, this->registers->name.A);
            break;
        }
        case Instructions::Instr::STX:
        {
            this->memory->Write(address, this->registers->name.X);
            break;
        }
        case Instructions::Instr::STY:
        {
            this->memory->Write(address, this->registers->name.Y);
            break;
        }
        case Instructions::Instr::TAX:
        {
            this->registers->name.X = this->registers->name.A;
            this->UpdateRegsForZeroAndNeg(this->registers->name.X);
            break;
        }
        case Instructions::Instr::TAY:
        {
            this->registers->name.Y = this->registers->name.A;
            this->UpdateRegsForZeroAndNeg(this->registers->name.Y);
            break;
        }
        case Instructions::Instr::TSX:
        {
            this->registers->name.X = this->registers->name.S;
            this->UpdateRegsForZeroAndNeg(this->registers->name.X);
            break;
        }
        case Instructions::Instr::TXA:
        {
            this->registers->name.A = this->registers->name.X;
            this->UpdateRegsForZeroAndNeg(this->registers->name.A);
            break;
        }
        case Instructions::Instr::TXS:
        {
            this->registers->name.S = this->registers->name.X;
            break;
        }
        case Instructions::Instr::TYA:
        {
            this->registers->name.A = this->registers->name.Y;
            this->UpdateRegsForZeroAndNeg(this->registers->name.A);
            break;
        }
        default:
        {
            std::cerr << "Unknown instruction at pc: " << std::hex << this->registers->name.PC;
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
    this->memory->Write( 0x0100 + this->registers->name.S , value);

    //update stack to next free byte
    this->registers->name.S -= 1;
}

byte Cpu::Pop()
{
    //go back to next byte on the stack
    this->registers->name.S += 1;
    //read value from stack
    byte value = this->memory->Read( 0x0100 + this->registers->name.S);
    return value;
}


void Cpu::UpdateRegsForOverflow(byte oldAValue, byte inputVal)
{
    //convert to dwords so can detect overflow
    dword dInputVal = inputVal;
    dword dA = oldAValue;
    dword dC = this->registers->name.C;
    dword dAValue = dInputVal + dA + dC;
    this->registers->name.C = (dAValue > 255);

    int sInputVal = (sByte)inputVal;
    int sA = (sByte)oldAValue;
    int sAValue = sInputVal + sA + dC;
    this->registers->name.V = (sAValue < -128) || (sAValue > 127);
}

void Cpu::UpdateRegsForOverflowNeg(byte oldAValue, byte inputVal)
{
    //convert to dwords so can detect overflow
    dword dInputVal = inputVal;
    dword dA = oldAValue;
    dword dC = this->registers->name.C;
    dword dAValue = dA + ~dInputVal + dC;
    this->registers->name.C = !(dAValue > 255);


    int sInputVal = (sByte)inputVal;
    int sA = (sByte)oldAValue;
    int sAValue = sA - sInputVal - ( 1 - dC );
    this->registers->name.V = (sAValue < -128) || (sAValue > 127);
}



void Cpu::UpdateRegsForZeroAndNeg(byte inputVal)
{
    this->registers->name.Z = (inputVal == 0);
    
    this->registers->name.N = BitUtil::GetBits(inputVal, 7);
}

void Cpu::UpdateRegsForCompaire(byte value, byte inputVal)
{
    byte diff = value - inputVal;
    this->UpdateRegsForZeroAndNeg(diff);
    this->registers->name.C = (value >= inputVal);
}

int Cpu::AdditionalCyclesForPageCross(dword address1, dword address2)
{
    if( (address1 & 0xFF00) != (address2 & 0xFF00) )
    {
        return 1;
    }
    return 0;
}


bool Cpu::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->memory->SetCartridge(cartridge);
    return true;
}

CpuRegisters * Cpu::GetRegisters()
{
    return this->registers.get();
}

void Cpu::SetTotalClockCycles(int totalCycles)
{
    this->totalClockCycles = totalCycles;
}