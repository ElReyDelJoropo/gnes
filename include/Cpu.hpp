#pragma once

#include "InterruptLine.hpp"
#include "Debugger.hpp"
#include "CpuBus.hpp"
#include "types.hpp"

#include <sstream>
#include <string>

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
    friend class Debugger;
  public:
    Cpu(CpuBus *, InterruptLine *, Debugger *);

    void step();
    void powerUp();
    void reset();

    int getLastStepCount() const;

  private:
    static constexpr unsigned int CLOCK_FREQUENCY = 1789773;
    static constexpr std::uint16_t NMI_ADDRESS = 0xFFFA;
    static constexpr std::uint16_t RESET_ADDRESS = 0xFFFC;
    static constexpr std::uint16_t IRQ_ADDRESS = 0xFFFE;

    static const struct instruction {
        const char *name;
        void (Cpu::*func)(std::uint16_t);
        AddressingMode addressing_mode;
        int cycle_lenght;
    } instruction_lookup_table[0x100];
    static const std::uint16_t instruction_sizes[13];

    uByte _x{};
    uByte _y{};      // X and Y registers
    uByte _sp{};         // Stack pointer
    uByte _a{};          // Accumulator
    std::uint16_t _pc{}; // Program counter
    // XXX: An obscure but efficient approach
    union {
        struct {
            bool carry : 1;
            bool zero : 1;
            bool interrupt_disable : 1;
            bool decimal : 1;
            bool brk : 1;
            bool unused : 1;
            bool overflow : 1;
            bool negative : 1;
        };
        uByte data;
    } _p{};

    uByte _opcode{};
    int _cycles{};

    CpuBus *_bus;
    InterruptLine *_interrupt_line;
    Debugger *_debugger;


    std::uint16_t translateAddress(AddressingMode);
    void handleInterrupt();

    void push(uByte);
    void push16(std::uint16_t);

    uByte pull();
    std::uint16_t pull16();

    void branch(std::uint16_t);
    static bool isPageCrossed(std::uint16_t, std::uint16_t);

    void dumpCpuState(std::uint16_t);
    std::string assembleInstruction(const char *, std::uint16_t) const;
    std::string statusRegisterToString() const;
    // Instructions
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
    void SEC(std::uint16_t);
    void SED(std::uint16_t);
    void SEI(std::uint16_t);
    void STA(std::uint16_t);
    void STX(std::uint16_t);
    void STY(std::uint16_t);
    void TAX(std::uint16_t);
    void TAY(std::uint16_t);
    void TSX(std::uint16_t);
    void TXA(std::uint16_t);
    void TXS(std::uint16_t);
    void TYA(std::uint16_t);
    void XXX(std::uint16_t);
};
} // namespace gnes
