#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
#include "stdio.h"

#define TASK_BEGIN()        static uint16_t LineNum = 0;\
							static uint16_t Counter = 0;\
							switch(LineNum){case 0:

#define TASK_END()          }

#define TASK_DELAY(n)  do                           \
    {                                               \
        Counter = GlobalTimerCnt;                   \
		LineNum = __LINE__;case __LINE__:           \
        if (GlobalTimerCnt < Counter)               \
        {                                           \
            if ((0xFFFF - Counter+GlobalTimerCnt) >= n) \
            {                                       \
                break;                              \
            }                                       \
            else                                    \
            {                                       \
                return;                             \
            }                                       \
        }                                           \
        else if((GlobalTimerCnt-Counter) >= n )     \
        {                                           \
            break;                                  \
        }                                           \
        return;                                     \
    }while(0)


static volatile uint16_t GlobalTimerCnt = 0;

void task_timerproc(void)
{
    GlobalTimerCnt++;
}

void task1(void)
{
    TASK_BEGIN();

    while (1)
    {
        TASK_DELAY(2);
        LED0 = !LED0;
    }

    TASK_END();
}

void task2(void)
{
    TASK_BEGIN();

    while (1)
    {
        TASK_DELAY(4);
        printf("task2,delay4\r\n");
        TASK_DELAY(2);
        LED1 = !LED1;
    }

    TASK_END();
}




int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
    LED_Init();         //初始化与LED连接的硬件接口
    TIM3_Int_Init(4999, 7199); //10Khz的计数频率，计数到5000为500ms
    uart_init(115200);

    while (1)
    {
        task1();
        task2();
    }
}



void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(  //使能或者失能指定的TIM中断
        TIM3, //TIM2
        TIM_IT_Update,
        ENABLE  //使能
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设

}

/**********************************************************************************************/

extern void task_timerproc(void);

void TIM3_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);    //清除TIMx的中断待处理位:TIM 中断源
        task_timerproc();
    }
}
