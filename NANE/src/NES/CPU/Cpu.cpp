#include "Cpu.h"

Cpu::Cpu(std::unique_ptr<CpuRegisters> cpuRegisters, std::shared_ptr<PpuRegisters> ppuRegisters, std::shared_ptr<ApuRegisters> apuRegisters)
{
    this->registers = std::move(cpuRegisters);
    this->memory = std::unique_ptr<CpuMemoryMap>( new CpuMemoryMap(ppuRegisters, apuRegisters) );
}

bool Cpu::PowerCycle()
{
    //TODO
    this->registers->name.PC = 0xC000;
    return true;
}

int Cpu::Step()
{
    //get optCode from memory
    byte optCode = this->memory->Read(this->registers->name.PC);

    Instructions::Opcode opcode = Instructions::Opcodes[optCode];

    if( opcode.addrm == Instructions::Instr::INVALID )
    {
        std::cerr << "failed to read instruction" << std::endl;
    }

    std::cout<< this->registers->name.PC << " ";

    byte inputVal = 0;
    dword address = 0;
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
            break;
        }
        case Instructions::AddrM::acc:
        {
            instLen = 1;
            inputVal = this->registers->name.A;
            outputToAccum = true;
            break;
        }

        //2 byte instructions
        case Instructions::AddrM::imm:
        {
            instLen = 2;
            address = this->memory->Read(this->registers->name.PC + 1);
            inputVal = this->memory->Read(address);
            break;
        }
        case Instructions::AddrM::zpg_:
        {
            instLen = 2;
            address = this->memory->Read(this->registers->name.PC + 1);
            inputVal = this->memory->Read(address);
            break;
        }
        case Instructions::AddrM::zpgX:
        {
            instLen = 2;
            address = this->memory->Read(this->registers->name.PC + 1);
            address = address + this->registers->name.X;
            inputVal = this->memory->Read(address);
            break;
        }
        case Instructions::AddrM::zpgY:
        {
            instLen = 2;
            address = this->memory->Read(this->registers->name.PC + 1);
            address = address + this->registers->name.Y;
            inputVal = this->memory->Read(address);
            break;
        }
        case Instructions::AddrM::rel:
        {
            instLen = 2;
            sByte offset = this->memory->Read(this->registers->name.PC + 1);
            address = offset + this->registers->name.PC + 2;
            break;
        }
        case Instructions::AddrM::Xind:
        {
            instLen = 2;
            byte indirrectAddress = this->memory->Read( this->registers->name.PC + 1);
            indirrectAddress = indirrectAddress + this->registers->name.X;
            address = BitUtil::GetDWord(this->memory.get(), indirrectAddress);
            inputVal = this->memory->Read( address );
            break;
        }
        case Instructions::AddrM::indY:
        {
            instLen = 2;
            byte indirrectAddress = this->memory->Read( this->registers->name.PC + 1);
            address = BitUtil::GetDWord(this->memory.get(), indirrectAddress);
            address = address + this->registers->name.Y;
            inputVal = this->memory->Read( address );
            break;
        }
            

            //3 byte instructions
        case Instructions::AddrM::abs_:
        {
            instLen = 3;
            address = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
            inputVal = this->memory->Read(address);
            break;
        }
        case Instructions::AddrM::absX:
        {
            instLen = 3;
            address = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
            address = address + this->registers->name.X;
            inputVal = this->memory->Read(address);
            break;
        }
        case Instructions::AddrM::absY:
        {
            instLen = 3;
            address = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
            address = address + this->registers->name.Y;
            inputVal = this->memory->Read(address);
            break;
        }
        case Instructions::AddrM::ind_:
        {
            instLen = 3;
            dword indirrectAddress = BitUtil::GetDWord(this->memory.get(), this->registers->name.PC + 1);
            address = BitUtil::GetDWord(this->memory.get(), indirrectAddress);
            break;
        }
        default:
        {
            std::cerr << "invalid memory mode" << std::endl;
            break;
        }
    }

    //increment PC
    this->registers->name.PC += instLen;
    
    //execute instructions
    switch(opcode.instr)
    {
        case Instructions::Instr::ADC:
        {
            this->UpdateRegsForOverflow(inputVal);
            this->registers->name.A = this->registers->name.A + this->registers->name.C + inputVal;
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
            if(outputToAccum)
            {
                this->registers->name.A = inputVal << 1;
            }
            else
            {
                byte output = inputVal << 1;
                this->memory->Write(address, output);
            }
            this->UpdateRegsForAccZeroAndNeg(inputVal);
            break;
        }
        case Instructions::Instr::BCC:
        {
            if( this->registers->name.C == false )
            {
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BCS:
        {
            if( this->registers->name.C == true )
            {
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BEQ:
        {
            if( this->registers->name.Z == true )
            {
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BIT:
        {
            byte result = this->registers->name.A & inputVal == 0;
            this->UpdateRegsForZeroAndNeg(result);
            this->registers->name.V = BitUtil::GetBits(inputVal, 6);
            this->registers->name.N = BitUtil::GetBits(inputVal, 7);
            break;
        }
        case Instructions::Instr::BMI:
        {
            if( this->registers->name.N == true )
            {
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BNE:
        {
            if( this->registers->name.Z == false )
            {
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BRK:
        {
            throw std::logic_error("Haven't implemented break command");
            break;
        }
        case Instructions::Instr::BVC:
        {
            if( this->registers->name.V == false )
            {
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::BVS:
        {
            if( this->registers->name.V == true )
            {
                this->registers->name.PC = address;
            }
            break;
        }
        case Instructions::Instr::CLC:
        {
            this->registers->name.C == false;
            break;
        }
        case Instructions::Instr::CLD:
        {
            this->registers->name.D == false;
            break;
        }
        case Instructions::Instr::CLI:
        {
            this->registers->name.I == false;
            break;
        }
        case Instructions::Instr::CLV:
        {
            this->registers->name.V == false;
            break;
        }
        case Instructions::Instr::CMP:
        {
            byte result = this->registers->name.A - inputVal;
            this->UpdateRegsForCompaire(result);
            break;
        }
        case Instructions::Instr::CPX:
        {
            byte result = this->registers->name.X - inputVal;
            this->UpdateRegsForCompaire(result);
            break;
        }
        case Instructions::Instr::CPY:
        {
            byte result = this->registers->name.Y - inputVal;
            this->UpdateRegsForCompaire(result);
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
            this->Push(this->registers->name.PC);
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
            if(outputToAccum)
            {
                this->registers->name.A = inputVal >> 1;
            }
            else
            {
                byte output = inputVal >> 1;
                this->memory->Write(address, output);
            }
            this->UpdateRegsForZeroAndNeg(inputVal);
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
            this->Push(this->registers->name.P);
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
            this->registers->name.P = this->Pop();
            break;
        }
        case Instructions::Instr::ROL:
        {
            byte result = inputVal << 1;
            result = inputVal | this->registers->name.C;
            if(outputToAccum)
            {
                this->registers->name.A = result;
            }
            else
            {
                this->memory->Write(address, result);
            }
            this->registers->name.C = BitUtil::GetBits(inputVal, 7);
            this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::ROR:
        {
            byte result = inputVal >> 1;
            result = inputVal | (this->registers->name.C << 7);
            if(outputToAccum)
            {
                this->registers->name.A = result;
            }
            else
            {
                this->memory->Write(address, result);
            }
            this->registers->name.C = BitUtil::GetBits(inputVal, 0);
            this->UpdateRegsForAccZeroAndNeg(result);
            break;
        }
        case Instructions::Instr::RTI:
        {
            throw std::logic_error("Haven't implemented RTI command");
            break;
        }
        case Instructions::Instr::RTS:
        {
            this->registers->name.PC = this->Pop();
            break;
        }
        case Instructions::Instr::SBC:
        {
            bool oldSign = BitUtil::GetBits(this->registers->name.A, 7);
            this->registers->name.A = this->registers->name.A - (inputVal + (1 - this->registers->name.C));
            bool newSign = BitUtil::GetBits(this->registers->name.A, 7);
            this->registers->name.V = 0;
            this->registers->name.C = (oldSign != newSign);
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
            throw std::logic_error("Unknown instruction");
            break;
        }
    }
    return cycleCount;
}

void Cpu::Push(dword value)
{
    union
    {
        dword orgValue : 16;
        struct
        {
            byte lower : 8;
            byte upper : 8;
        }bytes;
    }temp;
    temp.orgValue = value;
    this->Push(temp.bytes.lower);
    this->Push(temp.bytes.upper);
}

void Cpu::Push(byte value)
{
    //update stack to next free byte
    this->registers->name.S -= 1;
    //write value to stack
    this->memory->Write(this->registers->name.S, value);
}

byte Cpu::Pop()
{
    byte value = this->memory->Read(this->registers->name.S);
    this->registers->name.S -= 1;
    return value;
}


void Cpu::UpdateRegsForOverflow(byte inputVal)
{
    //convert to dwords so can detect overflow
    dword dInputVal = inputVal;
    dword dA = this->registers->name.A;
    dword dC = this->registers->name.C;
    dword newA = dInputVal + dA + dC;

    if(newA > 255)
    {
        this->registers->name.C = true;
    }
}

void Cpu::UpdateRegsForZeroAndNeg(byte inputVal)
{
    this->registers->name.Z = (inputVal == 0);
    
    this->registers->name.N = BitUtil::GetBits(inputVal, 7);
}

void Cpu::UpdateRegsForAccZeroAndNeg(byte inputVal)
{
    this->registers->name.Z = (this->registers->name.A == 0);
    
    this->registers->name.N = BitUtil::GetBits(inputVal, 7);
}

void Cpu::UpdateRegsForCompaire(byte value)
{
    this->UpdateRegsForZeroAndNeg(value);
    if(value <= 0)
    {
        this->registers->name.C = true;
    }
}


bool Cpu::SetCartridge(std::shared_ptr<ICartridge> cartridge)
{
    this->memory->SetCartridge(cartridge);
    return true;
}