/**
   @brief  Initialization for master parts like header transmission and - if necessary -
           event triggered frame collision resolving
   @pre    CXPI driver initialized
   @param  void
   @retval void
*/
void cxpi_master_task_init(void)
{
#ifdef CXPI_EVENT_TRIGGERED_FRAME_ENABLE
	;
#endif /* ifdef CXPI_EVENT_TRIGGERED_FRAME_ENABLE */
}
/**
   @brief  Starts transmission of a frame header according to the running
           schedule table
   @pre    CXPI driver initialized
   @param  void
   @retval void
*/
void cxpi_master_task_send_PID(void)//
{
   /* transmit PID field */
	;
}
/**
   @brief  Collision resolving of event triggered frames
   @pre    CXPI driver initialized
   @param  void
   @retval void
*/
void lin_master_check_etf(void)
{
	;
}