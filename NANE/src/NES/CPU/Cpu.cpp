#include "Cpu.h"

Cpu::Cpu(std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
{
    this->totalClockCycles = 0;
    this->cpuMemory = std::unique_ptr<CpuMemoryMap>( new CpuMemoryMap(ppuRegisters, apuRegisters) );
}

bool Cpu::PowerCycle(dword newPcAddress)
{
    this->cpuMemory->GetRegisters()->name.P = 0x34;
    this->cpuMemory->GetRegisters()->name.A = 0x0;
    this->cpuMemory->GetRegisters()->name.X = 0x0;
    this->cpuMemory->GetRegisters()->name.Y = 0x0;
    this->cpuMemory->GetRegisters()->name.Y = 0xFD;
    this->cpuMemory->Write(0x4017, 0x0);
    this->cpuMemory->Write(0x4015, 0x0);
    for(int i = 0x4000; i < 0x400F; ++i)
    {
        this->cpuMemory->Write(i, 0x0);
    }
    for(int i = 0x4010; i < 0x4013; ++i)
    {
        this->cpuMemory->Write(i, 0x0);
    }
    this->cpuMemory->GetRegisters()->name.PC = newPcAddress;
    return true;
}

int Cpu::Step()
{
    std::unique_ptr<Instructions::Instruction> decodedInst = this->DecodeInstruction(this->cpuMemory->GetRegisters()->name.PC);
    Instructions::Opcode opcode = decodedInst->opcode;
    byte inputVal = decodedInst->inputVal;
    dword argAddress = decodedInst->argAddress;
    int instLen = decodedInst->instLen;
    int cycleCount = decodedInst->cycleCount;
    bool outputToAccum = decodedInst->outputToAccum;
    std::string addressModeText = decodedInst->addressModeText;

    //print to display
    std::stringstream instCodeSS;
    for(int i = 0; i < instLen; ++i)
    {
        instCodeSS << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << std::right << (int) this->cpuMemory->Read(this->cpuMemory->GetRegisters()->name.PC + i) << " ";
    }
    std::string instCodeText = instCodeSS.str();
    std::cout<< std::hex << std::setfill('0') << std::setw(4) << std::right << std::uppercase << this->cpuMemory->GetRegisters()->name.PC << "  ";
    std::cout << std::setw (10) << std::left << std::setfill(' ') << instCodeText;
    std::cout << std::setw (32) << std::left << addressModeText;
    std::cout << "A:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->cpuMemory->GetRegisters()->name.A << " ";
    std::cout << "X:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->cpuMemory->GetRegisters()->name.X << " ";
    std::cout << "Y:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->cpuMemory->GetRegisters()->name.Y << " ";
    std::cout << "P:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->cpuMemory->GetRegisters()->name.P << " ";
    std::cout << "SP:" << std::hex << std::setfill('0') << std::setw(2) << std::right << (int) this->cpuMemory->GetRegisters()->name.S << " ";
    std::cout << "PPU:" << std::dec << std::setfill(' ') << std::setw(3) << std::right << (int) 0 << ",";
    std::cout << std::dec << std::setfill(' ') << std::setw(3) << std::right << (int) 0 << " ";
    std::cout << "CYC:" << std::dec << (int) this->totalClockCycles;
    std::cout << std::endl;

    //increment PC
    this->cpuMemory->GetRegisters()->name.PC += instLen;

    //execute instructions
    switch(opcode.instr)
    {
        case Instructions::Instr::ADC:
        {
            byte oldA = this->cpuMemory->GetRegisters()->name.A;
            this->cpuMemory->GetRegisters()->name.A = this->cpuMemory->GetRegisters()->name.A + this->cpuMemory->GetRegisters()->name.C + inputVal;
            this->UpdateRegsForOverflow(oldA, inputVal);
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::AND:
        {
            this->cpuMemory->GetRegisters()->name.A = this->cpuMemory->GetRegisters()->name.A & inputVal;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::ASL:
        {
            this->cpuMemory->GetRegisters()->name.C = BitUtil::GetBits(inputVal,7);
            byte result = inputVal << 1;
            if(outputToAccum)
            {
                this->cpuMemory->GetRegisters()->name.A = result;
            }
            else
            {
                this->cpuMemory->Write(argAddress, result);
            }
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::BCC:
        {
            if( this->cpuMemory->GetRegisters()->name.C == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->cpuMemory->GetRegisters()->name.PC);
                this->cpuMemory->GetRegisters()->name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BCS:
        {
            if( this->cpuMemory->GetRegisters()->name.C == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->cpuMemory->GetRegisters()->name.PC);
                this->cpuMemory->GetRegisters()->name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BEQ:
        {
            if( this->cpuMemory->GetRegisters()->name.Z == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->cpuMemory->GetRegisters()->name.PC);
                this->cpuMemory->GetRegisters()->name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BIT:
        {
            byte result = this->cpuMemory->GetRegisters()->name.A & inputVal;
            this->UpdateRegsForZeroAndNeg(result);
            this->cpuMemory->GetRegisters()->name.V = BitUtil::GetBits(inputVal, 6);
            this->cpuMemory->GetRegisters()->name.N = BitUtil::GetBits(inputVal, 7);
            break;
        }
        case Instructions::Instr::BMI:
        {
            if( this->cpuMemory->GetRegisters()->name.N == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->cpuMemory->GetRegisters()->name.PC);
                this->cpuMemory->GetRegisters()->name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BNE:
        {
            if( this->cpuMemory->GetRegisters()->name.Z == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->cpuMemory->GetRegisters()->name.PC);
                this->cpuMemory->GetRegisters()->name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BPL:
        {
            byte p = this->cpuMemory->GetRegisters()->name.P;
            if( this->cpuMemory->GetRegisters()->name.N == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->cpuMemory->GetRegisters()->name.PC);
                this->cpuMemory->GetRegisters()->name.PC = argAddress;
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
            if( this->cpuMemory->GetRegisters()->name.V == false )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->cpuMemory->GetRegisters()->name.PC);
                this->cpuMemory->GetRegisters()->name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::BVS:
        {
            if( this->cpuMemory->GetRegisters()->name.V == true )
            {
                cycleCount += 1;
                cycleCount += this->AdditionalCyclesForPageCross(argAddress, this->cpuMemory->GetRegisters()->name.PC);
                this->cpuMemory->GetRegisters()->name.PC = argAddress;
            }
            break;
        }
        case Instructions::Instr::CLC:
        {
            this->cpuMemory->GetRegisters()->name.C = false;
            break;
        }
        case Instructions::Instr::CLD:
        {
            this->cpuMemory->GetRegisters()->name.D = false;
            break;
        }
        case Instructions::Instr::CLI:
        {
            this->cpuMemory->GetRegisters()->name.I = false;
            break;
        }
        case Instructions::Instr::CLV:
        {
            this->cpuMemory->GetRegisters()->name.V = false;
            break;
        }
        case Instructions::Instr::CMP:
        {
            this->UpdateRegsForCompaire(this->cpuMemory->GetRegisters()->name.A, inputVal);
            break;
        }
        case Instructions::Instr::CPX:
        {
            this->UpdateRegsForCompaire(this->cpuMemory->GetRegisters()->name.X, inputVal);
            break;
        }
        case Instructions::Instr::CPY:
        {
            this->UpdateRegsForCompaire(this->cpuMemory->GetRegisters()->name.Y, inputVal);
            break;
        }
        case Instructions::Instr::DEC:
        {
            byte result = inputVal - 1;
            this->cpuMemory->Write(argAddress, result);
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::DEX:
        {
            byte result = this->cpuMemory->GetRegisters()->name.X - 1;
            this->cpuMemory->GetRegisters()->name.X = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::DEY:
        {
            byte result = this->cpuMemory->GetRegisters()->name.Y - 1;
            this->cpuMemory->GetRegisters()->name.Y = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::EOR:
        {
            byte result = this->cpuMemory->GetRegisters()->name.A ^ inputVal;
            this->cpuMemory->GetRegisters()->name.A = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INC:
        {
            byte result = inputVal + 1;
            this->cpuMemory->Write(argAddress, result);
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INX:
        {
            byte result = this->cpuMemory->GetRegisters()->name.X + 1;
            this->cpuMemory->GetRegisters()->name.X = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::INY:
        {
            byte result = this->cpuMemory->GetRegisters()->name.Y + 1;
            this->cpuMemory->GetRegisters()->name.Y = result;
            this->UpdateRegsForZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::JMP:
        {
            this->cpuMemory->GetRegisters()->name.PC = argAddress;
            break;
        }
        case Instructions::Instr::JSR:
        {
            //store next address on stack
            dword returnMinus1 = this->cpuMemory->GetRegisters()->name.PC - 1;
            this->Push(returnMinus1);
            this->cpuMemory->GetRegisters()->name.PC = argAddress;
            break;
        }
        case Instructions::Instr::LDA:
        {
            this->cpuMemory->GetRegisters()->name.A = inputVal;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::LDX:
        {
            this->cpuMemory->GetRegisters()->name.X = inputVal;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.X);
            break;
        }
        case Instructions::Instr::LDY:
        {
            this->cpuMemory->GetRegisters()->name.Y = inputVal;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.Y);
            break;
        }
        case Instructions::Instr::LSR:
        {
            this->cpuMemory->GetRegisters()->name.C = BitUtil::GetBits(inputVal,0);
            byte result = inputVal >> 1;
            if(outputToAccum)
            {
                this->cpuMemory->GetRegisters()->name.A = result;
            }
            else
            {
                this->cpuMemory->Write(argAddress, result);
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
            this->cpuMemory->GetRegisters()->name.A = this->cpuMemory->GetRegisters()->name.A | inputVal;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::PHA:
        {
            this->Push(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::PHP:
        {
            //magic value from nestest
            byte setBreakHigh = this->cpuMemory->GetRegisters()->name.P | 0x30;
            this->Push(setBreakHigh);
            break;
        }
        case Instructions::Instr::PLA:
        {
            this->cpuMemory->GetRegisters()->name.A = this->Pop();
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::PLP:
        {
            //from nestest rom
            byte breakBitLow = (this->Pop() & 0xEF) | 0x20;
            this->cpuMemory->GetRegisters()->name.P = breakBitLow;
           
            break;
        }
        case Instructions::Instr::ROL:
        {
            byte result = inputVal << 1;
            result = result | this->cpuMemory->GetRegisters()->name.C;
            if(outputToAccum)
            {
                this->cpuMemory->GetRegisters()->name.A = result;
            }
            else
            {
                this->cpuMemory->Write(argAddress, result);
            }
            this->cpuMemory->GetRegisters()->name.C = BitUtil::GetBits(inputVal, 7);
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::ROR:
        {
            byte result = inputVal >> 1;
            result = result | (this->cpuMemory->GetRegisters()->name.C << 7);
            if(outputToAccum)
            {
                this->cpuMemory->GetRegisters()->name.A = result;
            }
            else
            {
                this->cpuMemory->Write(argAddress, result);
            }
            this->cpuMemory->GetRegisters()->name.C = BitUtil::GetBits(inputVal, 0);
            this->UpdateRegsForZeroAndNeg(result);
            //this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::RTI:
        {
            this->cpuMemory->GetRegisters()->name.P = this->Pop() | 0x20; //nestest
            this->cpuMemory->GetRegisters()->name.PCL = this->Pop();
            this->cpuMemory->GetRegisters()->name.PCH = this->Pop();
            break;
        }
        case Instructions::Instr::RTS:
        {
            this->cpuMemory->GetRegisters()->name.PCL = this->Pop();
            this->cpuMemory->GetRegisters()->name.PCH = this->Pop();
            this->cpuMemory->GetRegisters()->name.PC += 1;
            break;
        }
        case Instructions::Instr::SBC:
        {
            byte oldAVal = this->cpuMemory->GetRegisters()->name.A;
            this->cpuMemory->GetRegisters()->name.A = this->cpuMemory->GetRegisters()->name.A - (inputVal + (1 - this->cpuMemory->GetRegisters()->name.C));
            this->UpdateRegsForOverflowNeg(oldAVal, inputVal);
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::SEC:
        {
            this->cpuMemory->GetRegisters()->name.C = true;
            break;
        }
        case Instructions::Instr::SED:
        {
            this->cpuMemory->GetRegisters()->name.D = true;
            break;
        }
        case Instructions::Instr::SEI:
        {
            this->cpuMemory->GetRegisters()->name.I = true;
            break;
        }
        case Instructions::Instr::STA:
        {
            this->cpuMemory->Write(argAddress, this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::STX:
        {
            this->cpuMemory->Write(argAddress, this->cpuMemory->GetRegisters()->name.X);
            break;
        }
        case Instructions::Instr::STY:
        {
            this->cpuMemory->Write(argAddress, this->cpuMemory->GetRegisters()->name.Y);
            break;
        }
        case Instructions::Instr::TAX:
        {
            this->cpuMemory->GetRegisters()->name.X = this->cpuMemory->GetRegisters()->name.A;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.X);
            break;
        }
        case Instructions::Instr::TAY:
        {
            this->cpuMemory->GetRegisters()->name.Y = this->cpuMemory->GetRegisters()->name.A;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.Y);
            break;
        }
        case Instructions::Instr::TSX:
        {
            this->cpuMemory->GetRegisters()->name.X = this->cpuMemory->GetRegisters()->name.S;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.X);
            break;
        }
        case Instructions::Instr::TXA:
        {
            this->cpuMemory->GetRegisters()->name.A = this->cpuMemory->GetRegisters()->name.X;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        case Instructions::Instr::TXS:
        {
            this->cpuMemory->GetRegisters()->name.S = this->cpuMemory->GetRegisters()->name.X;
            break;
        }
        case Instructions::Instr::TYA:
        {
            this->cpuMemory->GetRegisters()->name.A = this->cpuMemory->GetRegisters()->name.Y;
            this->UpdateRegsForZeroAndNeg(this->cpuMemory->GetRegisters()->name.A);
            break;
        }
        default:
        {
            std::cerr << "Unknown instruction at pc: " << std::hex << this->cpuMemory->GetRegisters()->name.PC;
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
    this->cpuMemory->Write( 0x0100 + this->cpuMemory->GetRegisters()->name.S , value);

    //update stack to next free byte
    this->cpuMemory->GetRegisters()->name.S -= 1;
}

byte Cpu::Pop()
{
    //go back to next byte on the stack
    this->cpuMemory->GetRegisters()->name.S += 1;
    //read value from stack
    byte value = this->cpuMemory->Read( 0x0100 + this->cpuMemory->GetRegisters()->name.S);
    return value;
}


void Cpu::UpdateRegsForOverflow(byte oldAValue, byte inputVal)
{
    //convert to dwords so can detect overflow
    dword dInputVal = inputVal;
    dword dA = oldAValue;
    dword dC = this->cpuMemory->GetRegisters()->name.C;
    dword dAValue = dInputVal + dA + dC;
    this->cpuMemory->GetRegisters()->name.C = (dAValue > 255);

    int sInputVal = (sByte)inputVal;
    int sA = (sByte)oldAValue;
    int sAValue = sInputVal + sA + dC;
    this->cpuMemory->GetRegisters()->name.V = (sAValue < -128) || (sAValue > 127);
}

void Cpu::UpdateRegsForOverflowNeg(byte oldAValue, byte inputVal)
{
    //convert to dwords so can detect overflow
    dword dInputVal = inputVal;
    dword dA = oldAValue;
    dword dC = this->cpuMemory->GetRegisters()->name.C;
    dword dAValue = dA + ~dInputVal + dC;
    this->cpuMemory->GetRegisters()->name.C = !(dAValue > 255);


    int sInputVal = (sByte)inputVal;
    int sA = (sByte)oldAValue;
    int sAValue = sA - sInputVal - ( 1 - dC );
    this->cpuMemory->GetRegisters()->name.V = (sAValue < -128) || (sAValue > 127);
}



void Cpu::UpdateRegsForZeroAndNeg(byte inputVal)
{
    this->cpuMemory->GetRegisters()->name.Z = (inputVal == 0);
    
    this->cpuMemory->GetRegisters()->name.N = BitUtil::GetBits(inputVal, 7);
}

void Cpu::UpdateRegsForCompaire(byte value, byte inputVal)
{
    byte diff = value - inputVal;
    this->UpdateRegsForZeroAndNeg(diff);
    this->cpuMemory->GetRegisters()->name.C = (value >= inputVal);
}

int Cpu::AdditionalCyclesForPageCross(dword address1, dword address2)
{
    if( (address1 & 0xFF00) != (address2 & 0xFF00) )
    {
        return 1;
    }
    return 0;
}

std::unique_ptr<Instructions::Instruction> Cpu::DecodeInstruction(dword pcAddress)
{
    byte optCode = this->cpuMemory->Read(pcAddress);
    Instructions::Opcode opcode = Instructions::Opcodes[optCode];
    if( opcode.addrm == Instructions::AddrM::INVALID )
    {
        std::cerr << "failed to read instruction" << std::endl;
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
            inputVal = this->cpuMemory->GetRegisters()->name.A;
            outputToAccum = true;
            //LSR A
            ss << opcode.instr_name << " A";
            break;
        }

        //2 byte instructions
        case Instructions::AddrM::imm:
        {
            instLen = 2;
            inputVal = this->cpuMemory->Read(this->cpuMemory->GetRegisters()->name.PC + 1);
            //ORA #$AA
            ss << opcode.instr_name << std::hex << " #$" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)inputVal;
            break;
        }
        case Instructions::AddrM::zpg_:
        {
            instLen = 2;
            argAddress = this->cpuMemory->Read(this->cpuMemory->GetRegisters()->name.PC + 1);
            inputVal = this->cpuMemory->Read(argAddress);
            //ORA $78 = 00
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << argAddress << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::zpgX:
        {
            instLen = 2;
            dword lookupAddress = this->cpuMemory->Read(this->cpuMemory->GetRegisters()->name.PC + 1);
            argAddress = lookupAddress + this->cpuMemory->GetRegisters()->name.X;
            argAddress = argAddress % 0x100;
            inputVal = this->cpuMemory->Read(argAddress);
            //ORA $00,X @ 78 = 00
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << lookupAddress << ",X @ " << std::setw(2) << argAddress << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::zpgY:
        {
            instLen = 2;
            dword lookupAddress = this->cpuMemory->Read(this->cpuMemory->GetRegisters()->name.PC + 1);
            argAddress = lookupAddress + this->cpuMemory->GetRegisters()->name.Y;
            argAddress = argAddress % 0x100;
            inputVal = this->cpuMemory->Read(argAddress);
            //ORA $00,X @ 78 = 00
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << lookupAddress << ",Y @ " << std::setw(2) << argAddress << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::rel:
        {
            instLen = 2;
            sByte offset = this->cpuMemory->Read(this->cpuMemory->GetRegisters()->name.PC + 1);
            argAddress = offset + this->cpuMemory->GetRegisters()->name.PC + 2;
            //BCS $F815
            ss << opcode.instr_name << std::hex << " $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << argAddress;
            break;
        }
        case Instructions::AddrM::Xind:
        {
            instLen = 2;
            byte indirrectAddress = this->cpuMemory->Read( this->cpuMemory->GetRegisters()->name.PC + 1);
            byte indirrectAddressX = indirrectAddress + this->cpuMemory->GetRegisters()->name.X;
            argAddress = BitUtil::GetDWord(this->cpuMemory.get(), indirrectAddressX, true);
            inputVal = this->cpuMemory->Read( argAddress );
            //CMP ($80,X) @ 80 = 0200 = 40
            ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << (int)indirrectAddress << ",X) @ " << std::setw(2) << (int)indirrectAddressX << " = " << std::setw(4) << argAddress << " = " << std::setw(2) << (int)inputVal;
            break;
        }
        case Instructions::AddrM::indY:
        {
            instLen = 2;
            byte indirrectAddress = this->cpuMemory->Read( this->cpuMemory->GetRegisters()->name.PC + 1);
            dword inDirAddress = BitUtil::GetDWord(this->cpuMemory.get(), indirrectAddress, true);
            argAddress = inDirAddress + this->cpuMemory->GetRegisters()->name.Y;
            inputVal = this->cpuMemory->Read( argAddress );
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
            argAddress = BitUtil::GetDWord(this->cpuMemory.get(), this->cpuMemory->GetRegisters()->name.PC + 1);
            inputVal = this->cpuMemory->Read(argAddress);
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
            dword lookupAddress = BitUtil::GetDWord(this->cpuMemory.get(), this->cpuMemory->GetRegisters()->name.PC + 1);
            argAddress = lookupAddress + this->cpuMemory->GetRegisters()->name.X;
            inputVal = this->cpuMemory->Read(argAddress);
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
            dword lookupAddress = BitUtil::GetDWord(this->cpuMemory.get(), this->cpuMemory->GetRegisters()->name.PC + 1);
            argAddress = lookupAddress + this->cpuMemory->GetRegisters()->name.Y;
            inputVal = this->cpuMemory->Read(argAddress);
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
            dword indirrectAddress = BitUtil::GetDWord(this->cpuMemory.get(), this->cpuMemory->GetRegisters()->name.PC + 1);
            argAddress = BitUtil::GetDWord(this->cpuMemory.get(), indirrectAddress, true);
            //JMP ($0200) = DB7E
            ss << opcode.instr_name << std::hex << " ($" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << indirrectAddress << ") = " << std::setw(4) << argAddress;
            break;
        }
        default:
        {
            std::cerr << "invalid memory mode" << std::endl;
            //default to zero page addressing
            instLen = 2;
            argAddress = this->cpuMemory->Read(this->cpuMemory->GetRegisters()->name.PC + 1);
            inputVal = this->cpuMemory->Read(argAddress);
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


bool Cpu::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->cpuMemory->SetCartridge(cartridge);
    return true;
}

std::string Cpu::GenerateCpuScreen()
{
    dword pc = this->cpuMemory->GetRegisters()->name.PC;
    std::vector<Instructions::Instruction> decodedInstructions;
    int currentInstrIndex = -1;
    std::unique_ptr<MemoryRepeaterVec>& prgRom = this->cpuMemory->GetCartridge()->GetPrgRom();
    int i = prgRom->startAddress;
    while(i < prgRom->endAddress)
    {
        std::unique_ptr<Instructions::Instruction> decodedInstr = this->DecodeInstruction(i);
        if(decodedInstr->pcAddress == pc)
        {
            currentInstrIndex = decodedInstructions.size();
        }
        decodedInstructions.push_back(*decodedInstr);
        i += decodedInstr->instLen;
    }
    int last10Instructions = std::max(currentInstrIndex - 10, 0);
    int next10Instructions = std::min(currentInstrIndex + 10, (int)decodedInstructions.size());
    
    std::stringstream ss;
    for(int i = last10Instructions; i < next10Instructions; ++i)
    {
        const Instructions::Instruction& curInstr = decodedInstructions.at(i);
        if(i == currentInstrIndex)
        {
            ss << "-> ";
        }
        ss << curInstr.pcAddress << " " << curInstr.addressModeText << std::endl;
    }
    return ss.str();
}

CpuRegisters * Cpu::GetRegisters()
{
    return this->cpuMemory->GetRegisters().get();
}

void Cpu::SetTotalClockCycles(int totalCycles)
{
    this->totalClockCycles = totalCycles;
}