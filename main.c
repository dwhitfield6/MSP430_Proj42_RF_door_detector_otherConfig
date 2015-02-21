#include <msp430.h>
#include "RF.h"

#define LED0 BIT3
#define LED1 BIT4
#define DoorIN1 BIT1
#define DoorIN2 BIT0
#define TX BIT5

int ledcount= 1000;
char DoorChange =0;

unsigned char TXdataOPEN[12] = {0,2,2,2,2,2,1,1,1,1,1,0}; // door opened
unsigned char TXdataCLOSED[12] = {2,2,2,2,2,2,1,1,1,1,1,0}; // door closed

int timeout =0;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
	  P1DIR |= (TX);                            // Set all pins but RXD to output
	  P1OUT &= ~(TX);
	  P2DIR |= LED1 + LED0;
	  P2OUT &= ~(LED1 + LED0);
	  P1REN |= DoorIN1;
	  P1OUT |= DoorIN1;
	  P1IE |= DoorIN1; // P1.3 interrupt enabled
	  P1IES &= ~DoorIN1; // P1.3 interrupt enabled
	  P1IFG &= ~DoorIN1; // P1.3 IFG cleared
	  P1IE |= DoorIN2; // P1.3 interrupt enabled
	  P1IES |= DoorIN2; // P1.3 interrupt enabled
	  P1IFG &= ~DoorIN2; // P1.3 IFG cleared

	  _BIS_SR(LPM4_bits + GIE); // Enter LPM3 w/interrupt

	  //__enable_interrupt(); // enable all interrupts                                   // Enable CPU interrupts
	while(1)
	{
__delay_cycles(100);
if(timeout == 200)
{
	P1IE &= ~(DoorIN1 + DoorIN2);
	if(DoorChange ==1)
	{
		_delay_cycles(50000);

	  if(readSwitchP1(DoorIN1)) //door closed
	  {
				P2OUT |= LED1;
				P2OUT &= ~LED0;
	 		send_rf(TX, TXdataCLOSED);
	 		__delay_cycles(1000);
	 		send_rf(TX, TXdataCLOSED);
	 		__delay_cycles(1000);
	 		send_rf(TX, TXdataCLOSED);
	 		__delay_cycles(1000);
	 		send_rf(TX, TXdataCLOSED);
	 		__delay_cycles(1000);
	 		ledcount =0;

	  }
	  else
	  {

				P2OUT |= LED0;
				P2OUT &= ~LED1;
	 		send_rf(TX, TXdataOPEN);
	 		__delay_cycles(1000);
	 		send_rf(TX, TXdataOPEN);
	 		__delay_cycles(1000);
	 		send_rf(TX, TXdataOPEN);
	 		__delay_cycles(1000);
	 		send_rf(TX, TXdataOPEN);
	 		__delay_cycles(1000);
	 		ledcount=0;

	  }
	}
}
ledcount++;
if(ledcount == 1000)
{
	P2OUT &= ~(LED0 + LED1);
	_delay_cycles(100000);
}
if(ledcount > 1050)
{
ledcount = 1050;
}
timeout++;
if(timeout == 2000)
{
	_delay_cycles(200000);
	P1IE |= (DoorIN1 + DoorIN2);
	_BIS_SR(LPM4_bits + GIE); // Enter LPM3 w/interrupt
}



if(timeout>2400)
{
	timeout =2400;
}

}
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	_BIC_SR(LPM4_EXIT); // wake up from low power mode

  if(timeout > 1500)
  {
	  DoorChange =1;
  }
  timeout=0;

_delay_cycles(10);
P1IFG &= ~DoorIN1; // P1.3 IFG cleared
P1IFG &= ~DoorIN2; // P1.3 IFG cleared


}

