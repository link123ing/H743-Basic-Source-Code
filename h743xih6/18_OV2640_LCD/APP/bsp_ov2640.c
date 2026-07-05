/**
 ****************************************************************************************************
 * @file        bsp_ov2640.c
 * @author      自定义开发团队
 * @version     V1.0
 * @date        2024-01-01
 * @brief       OV2640 驱动代码
 * @license     Copyright (c) 2024-2030, 自定义版权信息
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:STM32系列开发板 (适配DCMI接口)
 * 技术参考:OV2640数据手册 + STM32 DCMI官方手册
 *
 * 修改说明
 * V1.0 20240101
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "bsp_ov2640.h"

//OV2640 SXGA初始化寄存器序列表
//此模式下帧率为15帧
//SXGA(1600*1200) 
const uint8_t ov2640_sxga_init_reg_tbl[][2]= 
{   
	0xff, 0x00,
	0x2c, 0xff,
	0x2e, 0xdf,
	0xff, 0x01,
	0x3c, 0x32,
	//
	0x11, 0x00,
	0x09, 0x02,
	0x04, 0xD8,//水平镜像,垂直翻转
	0x13, 0xe5,
	0x14, 0x48,
	0x2c, 0x0c,
	0x33, 0x78,
	0x3a, 0x33,
	0x3b, 0xfB,
	//
	0x3e, 0x00,
	0x43, 0x11,
	0x16, 0x10,
	//
	0x39, 0x92,
	//
	0x35, 0xda,
	0x22, 0x1a,
	0x37, 0xc3,
	0x23, 0x00,
	0x34, 0xc0,
	0x36, 0x1a,
	0x06, 0x88,
	0x07, 0xc0,
	0x0d, 0x87,
	0x0e, 0x41,
	0x4c, 0x00,
	
	0x48, 0x00,
	0x5B, 0x00,
	0x42, 0x03,
	//
	0x4a, 0x81,
	0x21, 0x99,
	//
	0x24, 0x40,
	0x25, 0x38,
	0x26, 0x82,
	0x5c, 0x00,
	0x63, 0x00,
	0x46, 0x00,
	0x0c, 0x3c,
	//
	0x61, 0x70,
	0x62, 0x80,
	0x7c, 0x05,
	//
	0x20, 0x80,
	0x28, 0x30,
	0x6c, 0x00,
	0x6d, 0x80,
	0x6e, 0x00,
	0x70, 0x02,
	0x71, 0x94,
	0x73, 0xc1, 
	0x3d, 0x34, 
	0x5a, 0x57,
	//
	0x12, 0x00,//UXGA 1600*1200
	
	0x17, 0x11,
	0x18, 0x75,
	0x19, 0x01,
	0x1a, 0x97,
	0x32, 0x36,
	0x03, 0x0f, 
	0x37, 0x40,
	// 
	0x4f, 0xca,
	0x50, 0xa8,
	0x5a, 0x23,
	0x6d, 0x00,
	0x6d, 0x38,
	//
	0xff, 0x00,
	0xe5, 0x7f,
	0xf9, 0xc0,
	0x41, 0x24,
	0xe0, 0x14,
	0x76, 0xff,
	0x33, 0xa0,
	0x42, 0x20,
	0x43, 0x18,
	0x4c, 0x00,
	0x87, 0xd5,
	0x88, 0x3f,
	0xd7, 0x03,
	0xd9, 0x10,
	0xd3, 0x82,
	//
	0xc8, 0x08,
	0xc9, 0x80,
	//
	0x7c, 0x00,
	0x7d, 0x00,
	0x7c, 0x03,
	0x7d, 0x48,
	0x7d, 0x48,
	0x7c, 0x08,
	0x7d, 0x20,
	0x7d, 0x10,
	0x7d, 0x0e,
	//
	0x90, 0x00,
	0x91, 0x0e,
	0x91, 0x1a,
	0x91, 0x31,
	0x91, 0x5a,
	0x91, 0x69,
	0x91, 0x75,
	0x91, 0x7e,
	0x91, 0x88,
	0x91, 0x8f,
	0x91, 0x96,
	0x91, 0xa3,
	0x91, 0xaf,
	0x91, 0xc4,
	0x91, 0xd7,
	0x91, 0xe8,
	0x91, 0x20,
	//
	0x92, 0x00,
	0x93, 0x06,
	0x93, 0xe3,
	0x93, 0x05,
	0x93, 0x05,
	0x93, 0x00,
	0x93, 0x04,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	//
	0x96, 0x00,
	0x97, 0x08,
	0x97, 0x19,
	0x97, 0x02,
	0x97, 0x0c,
	0x97, 0x24,
	0x97, 0x30,
	0x97, 0x28,
	0x97, 0x26,
	0x97, 0x02,
	0x97, 0x98,
	0x97, 0x80,
	0x97, 0x00,
	0x97, 0x00,
	//
	0xc3, 0xef,
	
	0xa4, 0x00,
	0xa8, 0x00,
	0xc5, 0x11,
	0xc6, 0x51,
	0xbf, 0x80,
	0xc7, 0x10,
	0xb6, 0x66,
	0xb8, 0xA5,
	0xb7, 0x64,
	0xb9, 0x7C,
	0xb3, 0xaf,
	0xb4, 0x97,
	0xb5, 0xFF,
	0xb0, 0xC5,
	0xb1, 0x94,
	0xb2, 0x0f,
	0xc4, 0x5c,
	//
	0xc0, 0xc8,
	0xc1, 0x96,
	0x8c, 0x00,
	0x86, 0x3d,
	0x50, 0x00,
	0x51, 0x90,
	0x52, 0x2c,
	0x53, 0x00,
	0x54, 0x00,
	0x55, 0x88,
	
	0x5a, 0x90,
	0x5b, 0x2C,
	0x5c, 0x05,
	
	0xd3, 0x02,//auto设置要小心
	//
	0xc3, 0xed,
	0x7f, 0x00,
	
	0xda, 0x09,
	
	0xe5, 0x1f,
	0xe1, 0x67,
	0xe0, 0x00,
	0xdd, 0x7f,
	0x05, 0x00,
};  
//OV2640 SVGA初始化寄存器序列表
//此模式下,帧率可以达到30帧
//SVGA 800*600
const uint8_t ov2640_svga_init_reg_tbl[][2]= 
{    
	0xff, 0x00,
	0x2c, 0xff,
	0x2e, 0xdf,
	0xff, 0x01,
	0x3c, 0x32,
	//
	0x11, 0x00,
	0x09, 0x02,
	0x04, 0xD8,//水平镜像,垂直翻转
	0x13, 0xe5,
	0x14, 0x48,
	0x2c, 0x0c,
	0x33, 0x78,
	0x3a, 0x33,
	0x3b, 0xfB,
	//
	0x3e, 0x00,
	0x43, 0x11,
	0x16, 0x10,
	//
	0x39, 0x92,
	//
	0x35, 0xda,
	0x22, 0x1a,
	0x37, 0xc3,
	0x23, 0x00,
	0x34, 0xc0,
	0x36, 0x1a,
	0x06, 0x88,
	0x07, 0xc0,
	0x0d, 0x87,
	0x0e, 0x41,
	0x4c, 0x00,
	0x48, 0x00,
	0x5B, 0x00,
	0x42, 0x03,
	//
	0x4a, 0x81,
	0x21, 0x99,
	//
	0x24, 0x40,
	0x25, 0x38,
	0x26, 0x82,
	0x5c, 0x00,
	0x63, 0x00,
	0x46, 0x22,
	0x0c, 0x3c,
	//
	0x61, 0x70,
	0x62, 0x80,
	0x7c, 0x05,
	//
	0x20, 0x80,
	0x28, 0x30,
	0x6c, 0x00,
	0x6d, 0x80,
	0x6e, 0x00,
	0x70, 0x02,
	0x71, 0x94,
	0x73, 0xc1,
	
	0x3d, 0x34, 
	0x5a, 0x57,
	//根据分辨率不同而设置
	0x12, 0x40,//SVGA 800*600
	0x17, 0x11,
	0x18, 0x43,
	0x19, 0x00,
	0x1a, 0x4b,
	0x32, 0x09,
	0x37, 0xc0,
	//
	0x4f, 0xca,
	0x50, 0xa8,
	0x5a, 0x23,
	0x6d, 0x00,
	0x3d, 0x38,
	//
	0xff, 0x00,
	0xe5, 0x7f,
	0xf9, 0xc0,
	0x41, 0x24,
	0xe0, 0x14,
	0x76, 0xff,
	0x33, 0xa0,
	0x42, 0x20,
	0x43, 0x18,
	0x4c, 0x00,
	0x87, 0xd5,
	0x88, 0x3f,
	0xd7, 0x03,
	0xd9, 0x10,
	0xd3, 0x82,
	//
	0xc8, 0x08,
	0xc9, 0x80,
	//
	0x7c, 0x00,
	0x7d, 0x00,
	0x7c, 0x03,
	0x7d, 0x48,
	0x7d, 0x48,
	0x7c, 0x08,
	0x7d, 0x20,
	0x7d, 0x10,
	0x7d, 0x0e,
	//
	0x90, 0x00,
	0x91, 0x0e,
	0x91, 0x1a,
	0x91, 0x31,
	0x91, 0x5a,
	0x91, 0x69,
	0x91, 0x75,
	0x91, 0x7e,
	0x91, 0x88,
	0x91, 0x8f,
	0x91, 0x96,
	0x91, 0xa3,
	0x91, 0xaf,
	0x91, 0xc4,
	0x91, 0xd7,
	0x91, 0xe8,
	0x91, 0x20,
	//
	0x92, 0x00,
	0x93, 0x06,
	0x93, 0xe3,
	0x93, 0x05,
	0x93, 0x05,
	0x93, 0x00,
	0x93, 0x04,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	0x93, 0x00,
	//
	0x96, 0x00,
	0x97, 0x08,
	0x97, 0x19,
	0x97, 0x02,
	0x97, 0x0c,
	0x97, 0x24,
	0x97, 0x30,
	0x97, 0x28,
	0x97, 0x26,
	0x97, 0x02,
	0x97, 0x98,
	0x97, 0x80,
	0x97, 0x00,
	0x97, 0x00,
	//
	0xc3, 0xed,
	0xa4, 0x00,
	0xa8, 0x00,
	0xc5, 0x11,
	0xc6, 0x51,
	0xbf, 0x80,
	0xc7, 0x10,
	0xb6, 0x66,
	0xb8, 0xA5,
	0xb7, 0x64,
	0xb9, 0x7C,
	0xb3, 0xaf,
	0xb4, 0x97,
	0xb5, 0xFF,
	0xb0, 0xC5,
	0xb1, 0x94,
	0xb2, 0x0f,
	0xc4, 0x5c,
	//根据分辨率不同而设置
	0xc0, 0x64,
	0xc1, 0x4B,
	0x8c, 0x00,
	0x86, 0x3D,
	0x50, 0x00,
	0x51, 0xC8,
	0x52, 0x96,
	0x53, 0x00,
	0x54, 0x00,
	0x55, 0x00,
	0x5a, 0xC8,
	0x5b, 0x96,
	0x5c, 0x00,
	
	0xd3, 0x02,//auto设置要小心
	//
	0xc3, 0xed,
	0x7f, 0x00,
	
	0xda, 0x09,
	
	0xe5, 0x1f,
	0xe1, 0x67,
	0xe0, 0x00,
	0xdd, 0x7f,
	0x05, 0x00,
};   
const uint8_t ov2640_jpeg_reg_tbl[][2]=
{
	0xff, 0x01, 
	0xe0, 0x14,
	0xe1, 0x77,
	0xe5, 0x1f,
	0xd7, 0x03,
	0xda, 0x10,
	0xe0, 0x00, 
};
const uint8_t ov2640_rgb565_reg_tbl[][2]=
{
	0xFF, 0x00,
	0xDA, 0x09,
	0xD7, 0x03,
	0xDF, 0x02,
	0x33, 0xa0,
	0x3C, 0x00,
	0xe1, 0x67,
	
	0xff, 0x01, 
	0xe0, 0x00,
	0xe1, 0x00,
	0xe5, 0x00,
	0xd7, 0x00, 
	0xda, 0x00,
	0xe0, 0x00,  
};
const uint8_t ov2640_yuv422_reg_tbl[][2]= 
{
	0xFF, 0x00, 
	0xDA, 0x10,
	0xD7, 0x03,
	0xDF, 0x00,
	0x33, 0x80,
	0x3C, 0x40,
	0xe1, 0x77,
	0x00, 0x00,
};


/**
 * @brief       OV2640读寄存器
 * @param       reg : 寄存器地址
 * @retval      读到的寄存器值
 */
uint8_t ov2640_read_reg(uint16_t reg)
{
    uint8_t data = 0;

    sccb_start();                       /* 启动SCCB传输  */
    sccb_send_byte(OV2640_ADDR);        /* 写器件ID */
		Delay_us(100);
    sccb_send_byte(reg);           /* 写寄存器高8位地址 */
		Delay_us(100);
    sccb_stop();
		Delay_us(100);
    /* 设置寄存器地址后，执行读操作 */
    sccb_start();
    sccb_send_byte(OV2640_ADDR | 0X01); /* 发送读命令 */
		Delay_us(100);
    data = sccb_read_byte();            /* 读取数据 */
    sccb_nack();
    sccb_stop();

    return data;
}

/**
 * @brief       OV2640写寄存器
 * @param       reg : 寄存器地址
 * @param       data: 要写入寄存器的值
 * @retval      0, 成功; 1, 失败.
 */
uint8_t ov2640_write_reg(uint16_t reg, uint8_t data)
{
    uint8_t res = 0;

    sccb_start();                            /* 启动SCCB传输 */
    if (sccb_send_byte(OV2640_ADDR)) res = 1; /* 写器件ID */
		Delay_us(100);
    if (sccb_send_byte(reg)) res = 1;    /* 写寄存器高8位地址 */
		Delay_us(100);
    if (sccb_send_byte(data)) res = 1;        /* 写数据 */
    sccb_stop();

    return res;
}

/**
 * @brief       设置摄像头模块PWDN脚的状态
 * @param       sta : 0, 上电; 1, 掉电;
 * @retval      无
 */
void ov2640_pwdn_set(uint8_t sta)
{
    OV2640_PWDN(sta);
}

/**
 * @brief       初始化OV2640 
 * @note        配置完以后,默认输出RGB565格式、320*240分辨率
 * @param       无
 * @retval      0,成功; 其他,错误代码
 */
uint8_t ov2640_init(void)
{ 
    uint16_t i = 0;
    uint16_t chip_id = 0;
//    GPIO_InitTypeDef gpio_init_struct;

//    /* 初始化复位和电源引脚 */
//    OV2640_RST_GPIO_CLK_ENABLE();
//    OV2640_PWDN_GPIO_CLK_ENABLE();

//    /* 复位引脚配置 */
//    gpio_init_struct.Pin = OV2640_RST_GPIO_PIN;
//    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
//    gpio_init_struct.Pull = GPIO_PULLUP;
//    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(OV2640_RST_GPIO_PORT, &gpio_init_struct);

//    /* 电源引脚配置 */
//    gpio_init_struct.Pin = OV2640_PWDN_GPIO_PIN;
//    HAL_GPIO_Init(OV2640_PWDN_GPIO_PORT, &gpio_init_struct);

    /* 硬件复位OV2640 */
		OV2640_PWDN(0);
		HAL_Delay(10);
		OV2640_RST(0);
		HAL_Delay(10);
		OV2640_RST(1);  /* 结束复位 */
    

    /* 初始化SCCB */
    sccb_init();
    HAL_Delay(5);
		
		ov2640_write_reg(OV2640_DSP_RA_DLMT,0x01);//操作sensor寄存器
		ov2640_write_reg(OV2640_SENSOR_COM7, 0x80);//软复位OV2640
		
    /* 读取芯片ID */
    chip_id = ov2640_read_reg(OV2640_SENSOR_MIDH) << 8;
    chip_id |= ov2640_read_reg(OV2640_SENSOR_MIDL);
    if (chip_id != OV2640_MID)
    {
        Uart_Printf(&huart1,"MID Error: 0x%X\r\n", chip_id);
        return 1; /* ID匹配失败 */
    }
		Uart_Printf(&huart1,"MID: 0x%X\r\n", chip_id);
		
		chip_id = ov2640_read_reg(OV2640_SENSOR_PIDH) << 8;
    chip_id |= ov2640_read_reg(OV2640_SENSOR_PIDL);
    if (chip_id != OV2640_PID)
    {
        Uart_Printf(&huart1,"PID Error: 0x%X\r\n", chip_id);
        return 2; /* ID匹配失败 */
    }	
		Uart_Printf(&huart1,"PID: 0x%X\r\n", chip_id);


    /* 初始化寄存器配置 */
    for(i=0;i<sizeof(ov2640_sxga_init_reg_tbl)/2;i++)
	{
	   	ov2640_write_reg(ov2640_sxga_init_reg_tbl[i][0],ov2640_sxga_init_reg_tbl[i][1]);
 	} 

    return 0; /* 初始化成功 */
}

/**
 * @brief       OV2640切换为RGB565模式
 * @param       无
 * @retval      无
 */
void ov2640_rgb565_mode(void)
{
	uint16_t i=0;
  for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++)
	{
		ov2640_write_reg(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1]); 
	} 
}

/**
 * @brief       OV2640切换为JPEG模式
 * @param       无
 * @retval      无
 */
void ov2640_jpeg_mode(void)
{
	uint16_t i=0;
    //设置:YUV422格式
	for(i=0;i<(sizeof(ov2640_yuv422_reg_tbl)/2);i++)
	{
		ov2640_write_reg(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]); 
	} 
	
	//设置:输出JPEG数据
	for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
	{
		ov2640_write_reg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);  
	}  
}

/**
 * @brief       设置OV2640输出分辨率
 * @param       width : 输出宽度 (建议值:160/320/640)
 * @param       height: 输出高度 (建议值:120/240/480)
 * @retval      无
 */
void ov2640_outsize_set(uint16_t width, uint16_t height)
{
    /* 设置图像输出宽度 */
    ov2640_write_reg(0x1702, (width >> 8) & 0xFF);
    ov2640_write_reg(0x1703, width & 0xFF);

    /* 设置图像输出高度 */
    ov2640_write_reg(0x1704, (height >> 8) & 0xFF);
    ov2640_write_reg(0x1705, height & 0xFF);
    HAL_Delay(5);
}

//自动曝光设置参数表,支持5个等级
const static uint8_t OV2640_AUTOEXPOSURE_LEVEL[5][8]=
{
	{
		0xFF,0x01,
		0x24,0x20,
		0x25,0x18,
		0x26,0x60,
	},
	{
		0xFF,0x01,
		0x24,0x34,
		0x25,0x1c,
		0x26,0x00,
	},
	{
		0xFF,0x01,	
		0x24,0x3e,	
		0x25,0x38,
		0x26,0x81,
	},
	{
		0xFF,0x01,
		0x24,0x48,
		0x25,0x40,
		0x26,0x81,
	},
	{
		0xFF,0x01,	
		0x24,0x58,	
		0x25,0x50,	
		0x26,0x92,	
	},
}; 
//OV2640自动曝光等级设置
//level:0~4
void OV2640_Auto_Exposure(uint8_t level)
{  
	uint8_t i;
	uint8_t *p=(uint8_t*)OV2640_AUTOEXPOSURE_LEVEL[level];
	for(i=0;i<4;i++)
	{ 
		ov2640_write_reg(p[i*2],p[i*2+1]); 
	} 
}  
//白平衡设置
//0:自动
//1:太阳sunny
//2,阴天cloudy
//3,办公室office
//4,家里home
void OV2640_Light_Mode(uint8_t mode)
{
	uint8_t regccval=0X5E;//Sunny 
	uint8_t regcdval=0X41;
	uint8_t regceval=0X54;
	switch(mode)
	{ 
		case 0://auto 
			ov2640_write_reg(0XFF,0X00);	 
			ov2640_write_reg(0XC7,0X10);//AWB ON 
			return;  	
		case 2://cloudy
			regccval=0X65;
			regcdval=0X41;
			regceval=0X4F;
			break;	
		case 3://office
			regccval=0X52;
			regcdval=0X41;
			regceval=0X66;
			break;	
		case 4://home
			regccval=0X42;
			regcdval=0X3F;
			regceval=0X71;
			break;	
	}
	ov2640_write_reg(0XFF,0X00);	 
	ov2640_write_reg(0XC7,0X40);	//AWB OFF 
	ov2640_write_reg(0XCC,regccval); 
	ov2640_write_reg(0XCD,regcdval); 
	ov2640_write_reg(0XCE,regceval);  
}
//色度设置
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Color_Saturation(uint8_t sat)
{ 
	uint8_t reg7dval=((sat+2)<<4)|0X08;
	ov2640_write_reg(0XFF,0X00);		
	ov2640_write_reg(0X7C,0X00);		
	ov2640_write_reg(0X7D,0X02);				
	ov2640_write_reg(0X7C,0X03);			
	ov2640_write_reg(0X7D,reg7dval);			
	ov2640_write_reg(0X7D,reg7dval); 		
}
//亮度设置
//0:(0X00)-2
//1:(0X10)-1
//2,(0X20) 0
//3,(0X30)+1
//4,(0X40)+2
void OV2640_Brightness(uint8_t bright)
{
  ov2640_write_reg(0xff, 0x00);
  ov2640_write_reg(0x7c, 0x00);
  ov2640_write_reg(0x7d, 0x04);
  ov2640_write_reg(0x7c, 0x09);
  ov2640_write_reg(0x7d, bright<<4); 
  ov2640_write_reg(0x7d, 0x00); 
}
//对比度设置
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Contrast(uint8_t contrast)
{
	uint8_t reg7d0val=0X20;//默认为普通模式
	uint8_t reg7d1val=0X20;
  	switch(contrast)
	{
		case 0://-2
			reg7d0val=0X18;	 	 
			reg7d1val=0X34;	 	 
			break;	
		case 1://-1
			reg7d0val=0X1C;	 	 
			reg7d1val=0X2A;	 	 
			break;	
		case 3://1
			reg7d0val=0X24;	 	 
			reg7d1val=0X16;	 	 
			break;	
		case 4://2
			reg7d0val=0X28;	 	 
			reg7d1val=0X0C;	 	 
			break;	
	}
	ov2640_write_reg(0xff,0x00);
	ov2640_write_reg(0x7c,0x00);
	ov2640_write_reg(0x7d,0x04);
	ov2640_write_reg(0x7c,0x07);
	ov2640_write_reg(0x7d,0x20);
	ov2640_write_reg(0x7d,reg7d0val);
	ov2640_write_reg(0x7d,reg7d1val);
	ov2640_write_reg(0x7d,0x06);
}
//特效设置
//0:普通模式    
//1,负片
//2,黑白   
//3,偏红色
//4,偏绿色
//5,偏蓝色
//6,复古	    
void OV2640_Special_Effects(uint8_t eft)
{
	uint8_t reg7d0val=0X00;//默认为普通模式
	uint8_t reg7d1val=0X80;
	uint8_t reg7d2val=0X80; 
	switch(eft)
	{
		case 1://负片
			reg7d0val=0X40; 
			break;	
		case 2://黑白
			reg7d0val=0X18; 
			break;	 
		case 3://偏红色
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XC0; 
			break;	
		case 4://偏绿色
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0X40; 
			break;	
		case 5://偏蓝色
			reg7d0val=0X18; 
			reg7d1val=0XA0;
			reg7d2val=0X40; 
			break;	
		case 6://复古
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XA6; 
			break;	 
	}
	ov2640_write_reg(0xff,0x00);
	ov2640_write_reg(0x7c,0x00);
	ov2640_write_reg(0x7d,reg7d0val);
	ov2640_write_reg(0x7c,0x05);
	ov2640_write_reg(0x7d,reg7d1val);
	ov2640_write_reg(0x7d,reg7d2val); 
}
//彩条测试
//sw:0,关闭彩条
//   1,开启彩条(注意OV2640的彩条是叠加在图像上面的)
void OV2640_Color_Bar(uint8_t sw)
{
	uint8_t reg;
	ov2640_write_reg(0XFF,0X01);
	reg=ov2640_read_reg(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1; 
	ov2640_write_reg(0X12,reg);
}
//设置图像输出窗口 
//sx,sy,起始地址
//width,height:宽度(对应:horizontal)和高度(对应:vertical)
void OV2640_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp; 
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;
	
	ov2640_write_reg(0XFF,0X01);			
	temp=ov2640_read_reg(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	ov2640_write_reg(0X03,temp);				//设置Vref的start和end的最低2位
	ov2640_write_reg(0X19,sy>>2);			//设置Vref的start高8位
	ov2640_write_reg(0X1A,endy>>2);			//设置Vref的end的高8位
	
	temp=ov2640_read_reg(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	ov2640_write_reg(0X32,temp);				//设置Href的start和end的最低3位
	ov2640_write_reg(0X17,sx>>3);			//设置Href的start高8位
	ov2640_write_reg(0X18,endx>>3);			//设置Href的end的高8位
}
//设置图像输出大小
//OV2640输出图像的大小(分辨率),完全由改函数确定
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV2640_OutSize_Set(uint16_t width,uint16_t height)
{
	uint16_t outh;
	uint16_t outw;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	ov2640_write_reg(0XFF,0X00);	
	ov2640_write_reg(0XE0,0X04);			
	ov2640_write_reg(0X5A,outw&0XFF);		//设置OUTW的低八位
	ov2640_write_reg(0X5B,outh&0XFF);		//设置OUTH的低八位
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	ov2640_write_reg(0X5C,temp);				//设置OUTH/OUTW的高位 
	ov2640_write_reg(0XE0,0X00);	
	return 0;
}
//设置图像开窗大小
//由:OV2640_ImageSize_Set确定传感器输出分辨率从大小.
//该函数则在这个范围上面进行开窗,用于OV2640_OutSize_Set的输出
//注意:本函数的宽度和高度,必须大于等于OV2640_OutSize_Set函数的宽度和高度
//     OV2640_OutSize_Set设置的宽度和高度,根据本函数设置的宽度和高度,由DSP
//     自动计算缩放比例,输出给外部设备.
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV2640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t hsize;
	uint16_t vsize;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	ov2640_write_reg(0XFF,0X00);	
	ov2640_write_reg(0XE0,0X04);					
	ov2640_write_reg(0X51,hsize&0XFF);		//设置H_SIZE的低八位
	ov2640_write_reg(0X52,vsize&0XFF);		//设置V_SIZE的低八位
	ov2640_write_reg(0X53,offx&0XFF);		//设置offx的低八位
	ov2640_write_reg(0X54,offy&0XFF);		//设置offy的低八位
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	ov2640_write_reg(0X55,temp);				//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	ov2640_write_reg(0X57,(hsize>>2)&0X80);	//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	ov2640_write_reg(0XE0,0X00);	
	return 0;
} 
//该函数设置图像尺寸大小,也就是所选格式的输出分辨率
//UXGA:1600*1200,SVGA:800*600,CIF:352*288
//width,height:图像宽度和图像高度
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV2640_ImageSize_Set(uint16_t width,uint16_t height)
{ 
	uint8_t temp; 
	ov2640_write_reg(0XFF,0X00);			
	ov2640_write_reg(0XE0,0X04);			
	ov2640_write_reg(0XC0,(width)>>3&0XFF);		//设置HSIZE的10:3位
	ov2640_write_reg(0XC1,(height)>>3&0XFF);		//设置VSIZE的10:3位
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	ov2640_write_reg(0X8C,temp);	
	ov2640_write_reg(0XE0,0X00);				 
	return 0;
}
