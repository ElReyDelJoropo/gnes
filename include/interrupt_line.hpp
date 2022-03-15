#pragma once
namespace gnes {
enum class InterruptType { None, Irq, Nmi, Reset};
    class InterruptLine{
        public:
            void setInterrupt(InterruptType i){
                _interrupt = i;
            }
            InterruptType getInterrupt(){
                return _interrupt;
            }
            void clear(){
                setInterrupt(InterruptType::None);
            }
        private:
        InterruptType _interrupt;
    };
}
