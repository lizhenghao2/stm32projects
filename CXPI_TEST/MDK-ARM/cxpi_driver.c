/* ---------------------------------------------------------------------------
 *  l_bool l_sys_init (void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Initialise LIN driver system.
   @pre    MCU started
   @param  void
   @retval void
*/
l_bool l_sys_init (void)
{
   lin_main_init();
   return 0u;
}
