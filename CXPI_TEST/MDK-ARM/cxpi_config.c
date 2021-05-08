#include "type.h"
#include "cxpi_driver.h"
#include "cxpi_main.h"

//t_cxpi_sched_table schedule0[3] = {{17,2176,80},{146,2176,80},{226,2176,80}};    //this schedule should be config by PC
t_cxpi_sched_table schedule0[cxpi_MAX_NUM];

//t_cxpi_sched_table schedule1[5] = {{17,2176,100},{146,2176,100},{97,640,100},{226,640,100},{163,640,100}};
//t_cxpi_sched_table schedule2[2] = {{60,2048,100},{125,2048,100}};
//t_cxpi_sched_table schedule3[1] = {{125,2048,100}};
//t_cxpi_sched_table schedule4[1] = {{60,2048,100}};
//t_cxpi_sched_table schedule5[1] = {{60,2048,1000}};
//t_cxpi_sched_table schedule6[1] = {{125,2048,1000}};
l_u8 schedTblSizeList[1];
l_ScheduleTableList* scheduleList[1];
void Init_ScheduleTableList(void)
{
  scheduleList[0] = (l_ScheduleTableList*)schedule0;
//  scheduleList[1] = (l_ScheduleTableList*)schedule1;
//  scheduleList[2] = (l_ScheduleTableList*)schedule2;
//  scheduleList[3] = (l_ScheduleTableList*)schedule3;
//  scheduleList[4] = (l_ScheduleTableList*)schedule4;
//  scheduleList[5] = (l_ScheduleTableList*)schedule5;
//  scheduleList[6] = (l_ScheduleTableList*)schedule6;
}
/**
   @brief  This function handles the timout monitoring
   @note   This function is called via the cyclic task in the application
           (ld_task).
   @pre    LIN driver initialized
   @param  void
   @return void
*/
void ld_config_task(void)
{
	;
}
