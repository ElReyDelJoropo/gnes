#pragma once

#include "interrupt_line.hpp"
#include "mmc.hpp"
#include "types.hpp"

namespace gnes {

enum AddressingMode {
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    AbsoluteX,
    AbsoluteY,
    IndexedIndirect,
    IndirectIndexed,
    Implied,
    Accumulator,
    Immediate,
    Absolute,
    Relative,
    Indirect
};

class Cpu {
  public:
    Cpu(Mmc &, InterruptLine &);

    void step();

  private:
    static constexpr unsigned int CLOCK_FREQUENCY = 1789773;
    static constexpr std::uint16_t NMI_ADDRESS = 0xFFFA;
    static constexpr std::uint16_t RESET_ADDRESS = 0xFFFC;
    static constexpr std::uint16_t IRQ_ADDRESS = 0xFFFE;

    static const struct instruction {
        const char *name;
        void (Cpu::*func)(std::uint16_t);
        AddressingMode addressing_mode;
        int size;
        int cycle_lenght;
    } instruction_lookup_table[0xFF];

    uByte _X, _Y; // X and Y registers
    uByte _SP;    // Stack pointer
    uByte _A;     // Accumulator
    std::uint16_t _PC;    // Program counter
    // XXX: An obscure but efficient approach
    union {
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            uByte carry : 1;
            uByte zero : 1;
            uByte interrupt_disable : 1;
            uByte decimal : 1;
            uByte brk : 1;
            uByte unused : 1;
            uByte overflow : 1;
            uByte negative : 1;
#else
            uByte negative : 1;
            uByte overflow : 1;
            uByte unused : 1;
            uByte brk : 1;
            uByte decimal : 1;
            uByte interrupt_disable : 1;
            uByte zero : 1;
            uByte carry : 1;
#endif
        };
        uByte data;
    } _P;

    uByte _opcode;

    int _cycles;
    Mmc &_mmc;
    InterruptLine &_interrupt_line;

    void powerUp();
    void reset();

    std::uint16_t translateAddress(AddressingMode);
    void handleInterrupt();

    void push(uByte);
    void push16(std::uint16_t);

    uByte pull();
    std::uint16_t pull16();

    void branch(std::uint16_t);
    bool is_page_crossed(std::uint16_t,std::uint16_t);

    //Instructions
    void ADC(std::uint16_t);
    void AND(std::uint16_t);
    void ASL(std::uint16_t);
    void BCC(std::uint16_t);
    void BCS(std::uint16_t);
    void BEQ(std::uint16_t);
    void BIT(std::uint16_t);
    void BMI(std::uint16_t);
    void BNE(std::uint16_t);
    void BPL(std::uint16_t);
    void BRK(std::uint16_t);
    void BVC(std::uint16_t);
    void BVS(std::uint16_t);
    void CLC(std::uint16_t);
    void CLD(std::uint16_t);
    void CLI(std::uint16_t);
    void CLV(std::uint16_t);
    void CMP(std::uint16_t);
    void CPX(std::uint16_t);
    void CPY(std::uint16_t);
    void DEC(std::uint16_t);
    void DEX(std::uint16_t);
    void DEY(std::uint16_t);
    void EOR(std::uint16_t);
    void INC(std::uint16_t);
    void INX(std::uint16_t);
    void INY(std::uint16_t);
    void JMP(std::uint16_t);
    void JSR(std::uint16_t);
    void LDA(std::uint16_t);
    void LDX(std::uint16_t);
    void LDY(std::uint16_t);
    void LSR(std::uint16_t);
    void NOP(std::uint16_t);
    void ORA(std::uint16_t);
    void PHA(std::uint16_t);
    void PHP(std::uint16_t);
    void PLA(std::uint16_t);
    void PLP(std::uint16_t);
    void ROL(std::uint16_t);
    void ROR(std::uint16_t);
    void RTI(std::uint16_t);
    void RTS(std::uint16_t);
    void SBC(std::uint16_t);
};
} // namespace gnes
