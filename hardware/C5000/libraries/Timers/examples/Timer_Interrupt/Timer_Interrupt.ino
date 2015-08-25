#include "Timers.h"

void setup()
{
  // put your setup code here, to run once:
      GPT_Config              hwConfig;
  Serial.begin(9600);
  
      Timer.selectTimer(GPT0);

    // Configure GPT module 
    hwConfig.autoLoad    = TRUE;
    hwConfig.ctrlTim     = TRUE;
    hwConfig.preScaleDiv = GPT_PRE_SC_DIV_7;
    hwConfig.prdLow      = 0xFFFF;
    hwConfig.prdHigh     = 0x0000;

    Timer.configTimer(hwConfig);

  // Init interrupt module
  initInterrupt(0x0000);

  // Plugin the ISR to vector table and enbale interrupts 	
  attachInterrupt(INTERRUPT_TINT, (INTERRUPT_IsrPtr)TIMER1_ISR, 0);    
  enableInterrupt(INTERRUPT_TINT);

    // Start the Timer 
    Timer.start();


}

interrupt void TIMER1_ISR(void)
{
  IRQ_clear(TINT_EVENT);
  /* Clear Timer Interrupt Aggregation Flag Register (TIAFR) */
  CSL_SYSCTRL_REGS->TIAFR = 0x01;
  Serial.println("Ping!");
  //IRQ_clear(INTERRUPT_TINT);
}
void loop()
{
  // put your main code here, to run repeatedly:
  
}
