/**
@file cxpi_hal.c
@brief Microcontroller specific implementation of the cxpi Driver communication
*/
/* ---------------------------------------------------------------------------
 *  void cxpi_hal_ISR(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Interrupt service routine for receive interrupt. Can be also used for
           polling (then it should be called once each bit).
--------------------------------------------------------------------------*/
void cxpi_hal_ISR(void)
{
	enum cxpi_state {idle=0,Ptype,PID,Frame_info,Data_bytes,CRC} uart_reg_stat;
	switch (uart_reg_stat){
		case idle: break;
		case Ptype: break;
		case PID: break;
		case Frame_info: break;
		case Data_bytes: break;
		case CRC: break;
	}
	
	return;
}