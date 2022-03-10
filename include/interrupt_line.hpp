#pragma once
namespace gnes {
enum INTERRUPT_TYPES { NONE, IRQ, NMI, RESET, ERROR };
    class InterruptLine{
        public:
            void setInterrupt(INTERRUPT_TYPES i){
                _interrupt = i;
            }
            INTERRUPT_TYPES getInterrupt(){
                return _interrupt;
            }
            void clear(){
                setInterrupt(NONE);
            }
        private:
        INTERRUPT_TYPES _interrupt;
    };
}
