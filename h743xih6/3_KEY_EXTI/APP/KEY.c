#include "KEY.h"

/*取消外部中断*/
#if External_Interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//由于同一个PIN的IO外部中断是挂载在同一条外部中断线上的
	//所以这里不需要传入端口直接传入PIN号即可触发
	//但同时也暴露了局限性，就是外部中断无法分辨不同PORT的同一PIN
  if (GPIO_Pin == KEY1_Pin)
  {
    LEDR_Toggle;
		LEDG_OFF;
		LEDB_OFF;
  }else if(GPIO_Pin == KEY2_Pin)
	{
		LEDG_Toggle;
		LEDR_OFF;
		LEDB_OFF;
	}else if(GPIO_Pin == KEY3_Pin)
	{
		LEDB_Toggle;
		LEDG_OFF;
		LEDR_OFF;
	}
}
#endif

/*使用轮询*/
#if Polling
uint8_t Key_Val = 0;
uint8_t Key_Old = 0;
uint8_t Key_Down = 0;
uint8_t Key_Up = 0;

#define KEY_DEBOUNCE_MS   10        /* 消抖时间，单位 ms */
static uint8_t key_last = 0;        /* 上一次原始电平 */
static uint8_t key_debounced = 0;   /* 消抖后的稳定键值 */
static uint32_t key_tick = 0;       /* 记录第一次出现新电平的时刻 */

/*只读取按键电平*/
uint8_t RawKey_Read(void)
{
    if(HAL_GPIO_ReadPin(GPIO_KEY_PORT1,GPIO_KEY_PIN1)==GPIO_PIN_RESET) return 1;
    if(HAL_GPIO_ReadPin(GPIO_KEY_PORT2,GPIO_KEY_PIN2)==GPIO_PIN_RESET) return 2;
    if(HAL_GPIO_ReadPin(GPIO_KEY_PORT3,GPIO_KEY_PIN3)==GPIO_PIN_RESET) return 3;
    return 0;
}

/* 对按键读取电平后消抖 */
uint8_t Key_Read(void)
{
    uint8_t raw = RawKey_Read();
    uint32_t now = HAL_GetTick();          //系统毫秒 tick 

    if(raw != key_last)                    //电平发生变化
    {
        key_last   = raw;
        key_tick   = now;
        //还不更新，等消抖时间
        return key_debounced;
    }

    //电平稳定且超过消抖时间
    if(now - key_tick >= KEY_DEBOUNCE_MS)
    {
        key_debounced = key_last;
    }
    return key_debounced;
		
}

/* 实际按键按下后所需执行的任务处理函数 */
void Key_proc(void)
{
    uint8_t key = Key_Read();          //消抖后的稳定键值
    static uint8_t key_old = 0;

    //按下沿
    Key_Down = key & (key_old ^ key);
    //松开沿
    Key_Up   = ~key & (key_old ^ key);
    key_old  = key;
	
		//这个函数可以进行按键按下和按键松开的任务执行判断
		//接下来以按键按下为例
		switch(Key_Down)
		{
			case 1:					//只操作红灯
				LEDR_Toggle;
				LEDG_OFF;
				LEDB_OFF;
			break;
			
			case 2:					//只操作绿灯
				LEDG_Toggle;
				LEDR_OFF;
				LEDB_OFF;
			break;
			
			case 3:					//只操作蓝灯
				LEDB_Toggle;
				LEDG_OFF;
				LEDR_OFF;
			break;
			
			default:
				
			break;
		}
		
		
		//下面是长按的代码框架，但是未进行验证
#if 0
		static uint8_t down_cleared = 1;  // 标志：按下事件是否已处理

		if(Key_Down)       // 刚按下瞬间
				down_cleared = 0;  // 标记：有新的按下事件，未处理

		else if(!down_cleared)
		{
				// 这里写：长按逻辑 / 持续按下逻辑
				// 只要按键一直按着，就会一直进这里
		}
#endif
		
}

#endif

