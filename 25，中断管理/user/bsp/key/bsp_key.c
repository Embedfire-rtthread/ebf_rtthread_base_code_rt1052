/**
  ******************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ����Ӧ�ú����ӿ�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_iomuxc.h"
#include "fsl_gpio.h" 

#include "pad_config.h" 
#include "./key/bsp_key.h"  
#include "./bsp/nvic/bsp_nvic.h"
#include "./delay/core_delay.h"  

#include "board.h"
#include "rtthread.h"

/* �ⲿ������Ϣ���п��ƿ� */
extern rt_mq_t test_mq;

uint32_t send_data1 = 1;
uint32_t send_data2 = 2;

/******************************************************************
 * ��������
  ******************************************************************/
/* �����Ƿ񱻰��µ��жϱ�־ */
__IO bool g_KeyDown[2] = { false};

/******************************************************************
 * ��
  ******************************************************************/
/* �����жϼ�����ŵ�PAD���� */
#define KEY_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_0_OUTPUT_DRIVER_DISABLED| \
                                        SPEED_2_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_1_PULL_KEEPER_ENABLED| \
                                        PUE_1_PULL_SELECTED| \
                                        PUS_3_22K_OHM_PULL_UP| \
                                        HYS_1_HYSTERESIS_ENABLED)   
    /* ����˵�� : */
    /* ת������: ת��������
      ����ǿ��: �ر�
      �ٶ����� : medium(100MHz)
      ��©����: �ر� 
      ��/����������: ʹ��
      ��/������ѡ��: ������
      ����/����ѡ��: 22Kŷķ����
      �ͻ�������: ���� ��������ʱ��Ч��ʩ���ش�������ʹ�ܺ���Թ�������������*/

/******************************************************************
 * ����
  ******************************************************************/
static void Key_IOMUXC_MUX_Config(void);
static void Key_IOMUXC_PAD_Config(void);
static void Key_GPIO_Mode_Config(void);

/******************************************************************
 * ��������
  ******************************************************************/
/**
* @brief  ��ʼ���������IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void Key_IOMUXC_MUX_Config(void)
{
  /* ���ð������ŵĸ���ģʽΪGPIO����ʹ��SION���� */
  IOMUXC_SetPinMux(CORE_BOARD_WAUP_KEY_IOMUXC, 0U);
  IOMUXC_SetPinMux(CORE_BOARD_MODE_KEY_IOMUXC, 0U); 
}

/**
* @brief  ��ʼ�������������PAD����
* @param  ��
* @retval ��
*/
static void Key_IOMUXC_PAD_Config(void)
{
  /* ���ð����������Թ��� */    
  IOMUXC_SetPinConfig(CORE_BOARD_WAUP_KEY_IOMUXC, KEY_PAD_CONFIG_DATA); 
  IOMUXC_SetPinConfig(CORE_BOARD_MODE_KEY_IOMUXC, KEY_PAD_CONFIG_DATA); 
}

 /**
  * @brief  ��ʼ��������ص�GPIOģʽ
  * @param  ��
  * @retval ��
  */
static void Key_GPIO_Mode_Config(void)
{     
  /* ����Ϊ����ģʽ���͵�ƽ�жϣ�����ͨ��GPIO_PinInit������������ */
  gpio_pin_config_t key_config;
  
  /** ���İ�İ�����GPIO���� **/       
  key_config.direction = kGPIO_DigitalInput;    //����ģʽ
  key_config.outputLogic =  1;                  //Ĭ�ϸߵ�ƽ������ģʽʱ��Ч��
  key_config.interruptMode = kGPIO_IntLowLevel;//kGPIO_IntFallingEdge; //�͵�ƽ�����ж�
  
  /* ��ʼ�� KEY GPIO. */
  GPIO_PinInit(CORE_BOARD_WAUP_KEY_GPIO, CORE_BOARD_WAUP_KEY_GPIO_PIN, &key_config);
  GPIO_PinInit(CORE_BOARD_MODE_KEY_GPIO, CORE_BOARD_MODE_KEY_GPIO_PIN, &key_config);
}


/**
 * @brief  ��ʼ�������ж���ص�����
 * @param  ��
 * @retval ��
 */
static void Key_Interrupt_Config(void)   
{
  /* ����GPIO���ŵ��ж� */
  GPIO_PortEnableInterrupts(CORE_BOARD_WAUP_KEY_GPIO, 1U << CORE_BOARD_WAUP_KEY_GPIO_PIN);                           
  GPIO_PortEnableInterrupts(CORE_BOARD_MODE_KEY_GPIO, 1U << CORE_BOARD_MODE_KEY_GPIO_PIN); 
  
  /*�����ж����ȼ�,*/
  set_IRQn_Priority(CORE_BOARD_WAUP_KEY_IRQ,Group4_PreemptPriority_6, Group4_SubPriority_0);
  set_IRQn_Priority(CORE_BOARD_MODE_KEY_IRQ,Group4_PreemptPriority_6, Group4_SubPriority_1);
  
  /* ����GPIO�˿��ж� */
  EnableIRQ(CORE_BOARD_WAUP_KEY_IRQ);
  EnableIRQ(CORE_BOARD_MODE_KEY_IRQ);
}


 /**
  * @brief  ��ʼ������KEY��IO    
  * @param  ��
  * @retval ��
  */
void Key_IT_GPIO_Config(void)
{
  /* ��ʼ��GPIO���á����ԡ�ģʽ���ж�*/
  Key_IOMUXC_MUX_Config();
  Key_IOMUXC_PAD_Config();
  Key_GPIO_Mode_Config();
  Key_Interrupt_Config();
}

/********************�жϷ�����**************************/
/**
 * @brief  GPIO �����жϷ�����
 *         CORE_BOARD_WAUP_KEY_IRQHandlerֻ��һ���꣬
 *         �ڱ�������ָ��������GPIO5_Combined_0_15_IRQHandler��
 *         �жϷ��������ǹ̶��ģ������������ļ����ҵ���
 * @param  �жϷ������������������
 * @note   �жϺ���һ��ֻʹ�ñ�־λ����ָʾ����ɺ󾡿��˳���
 *         �����������ʱ�����������жϷ�������
 * @retval �жϷ����������з���ֵ
 */
void CORE_BOARD_WAUP_KEY_IRQHandler(void)
{ 
		/* �����ж� */
		rt_interrupt_enter();
		/*	��ⰴ���жϱ�־	*/	
		if(GPIO_GetPinsInterruptFlags(CORE_BOARD_WAUP_KEY_GPIO)&(1U<<CORE_BOARD_WAUP_KEY_GPIO_PIN))
		{
			CPU_TS_Tmr_Delay_MS(10);
				/*	�жϰ����Ƿ� ����	*/
			if(GPIO_PinRead(CORE_BOARD_WAUP_KEY_GPIO, CORE_BOARD_WAUP_KEY_GPIO_PIN) == 1)
			{
				/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
				rt_mq_send(	test_mq,	/* д�루���ͣ����е�ID(���) */
										&send_data1,			/* д�루���ͣ������� */
										sizeof(send_data1));			/* ���ݵĳ��� */	
			}	
		}
			/* ����жϱ�־λ */
		GPIO_PortClearInterruptFlags(CORE_BOARD_WAUP_KEY_GPIO,
																	 1U << CORE_BOARD_WAUP_KEY_GPIO_PIN);  
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
		/* �뿪�ж� */
		rt_interrupt_leave();
}

/**
 * @brief  GPIO �����жϷ�����
 *         CORE_BOARD_MODE_KEY_IRQHandlerֻ��һ���꣬
 *         �ڱ�������ָ��������GPIO1_Combined_0_15_IRQHandler��
 *         �жϷ��������ǹ̶��ģ������������ļ����ҵ���
 * @param  �жϷ������������������
 * @note   �жϺ���һ��ֻʹ�ñ�־λ����ָʾ����ɺ󾡿��˳���
 *         �����������ʱ�����������жϷ�������
 * @retval �жϷ����������з���ֵ
 */
void CORE_BOARD_MODE_KEY_IRQHandler(void)
{ 
		/* �����ж� */
		rt_interrupt_enter();

			/*	��ⰴ���жϱ�־	*/	
		if(GPIO_GetPinsInterruptFlags(CORE_BOARD_MODE_KEY_GPIO)&(1U<<CORE_BOARD_MODE_KEY_GPIO_PIN))
		{
			CPU_TS_Tmr_Delay_MS(10);
				/*	�жϰ����Ƿ� ����	*/
			if(GPIO_PinRead(CORE_BOARD_MODE_KEY_GPIO, CORE_BOARD_MODE_KEY_GPIO_PIN) == 1)
			{
    /* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
    rt_mq_send(	test_mq,	/* д�루���ͣ����е�ID(���) */
                &send_data2,			/* д�루���ͣ������� */
                sizeof(send_data2));			/* ���ݵĳ��� */	
			}
		}
		/* ����жϱ�־λ */
    GPIO_PortClearInterruptFlags(CORE_BOARD_MODE_KEY_GPIO,
                                 1U << CORE_BOARD_MODE_KEY_GPIO_PIN);  
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
		/* �뿪�ж� */
		rt_interrupt_leave();
}

