  /**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   RT-Threadϵͳ ˫������
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����Ұ��I.MX RTȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 

#include "board.h"
#include "fsl_debug_console.h"  
/* RT-Thread ͷ�ļ� */
#include "rtthread.h"
#include "cpuusage.h"

/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ� */
static rt_thread_t led1_thread = RT_NULL;
static rt_thread_t get_cpu_use_thread = RT_NULL;
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
static void get_cpu_use_thread_entry(void* parameter);

/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
/*
     * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
     * ����component.c�ļ��е�rtthread_startup()����������ˡ�
     * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
     */

    rt_kprintf("����һ��[Ұ��]-I.MX RTȫϵ�п�����-RTT-CPU������ͳ��ʵ��\r\n");

    led1_thread =                          /* �߳̿��ƿ�ָ�� */
        rt_thread_create( "led1",              /* �߳����� */
                          led1_thread_entry,   /* �߳���ں��� */
                          RT_NULL,             /* �߳���ں������� */
                          512,                 /* �߳�ջ��С */
                          3,                   /* �̵߳����ȼ� */
                          20);                 /* �߳�ʱ��Ƭ */

/* �����̣߳��������� */
if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
else
return -1;

    get_cpu_use_thread =                          /* �߳̿��ƿ�ָ�� */
        rt_thread_create( "get_cpu_use",              /* �߳����� */
                          get_cpu_use_thread_entry,   /* �߳���ں��� */
                          RT_NULL,             /* �߳���ں������� */
                          512,                 /* �߳�ջ��С */
                          5,                   /* �̵߳����ȼ� */
                          20);                 /* �߳�ʱ��Ƭ */

/* �����̣߳��������� */
if (get_cpu_use_thread != RT_NULL)
        rt_thread_startup(get_cpu_use_thread);
else
return -1;
}

void delay_test(int num)
{
	int i=0;
	int j=0;
	for(j=0;j<num;j++)
	{
		for(;i<1000000;i++)
		{
		}
	}
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{
    rt_uint16_t i;

    while (1) 
    {
        LED1_TOGGLE;
        /* ģ��ռ��CPU��Դ���޸���ֵ��Ϊģ����� */
        for (i = 0; i < 1000; i++)
        {
					delay_test(1000);
        }
			rt_thread_delay(500);   /* ��ʱ5��tick */
    }
}

static void get_cpu_use_thread_entry(void* parameter)
{
    rt_uint8_t major,minor;
    while (1) 
    {
        /* ��ȡCPU���������� */
        cpu_usage_get(&major,&minor);

        /* ��ӡCPU������ */
        rt_kprintf("CPU������ = %d.%d%\r\n",major,minor);

        rt_thread_delay(1000);   /* ��ʱ1000��tick */
    }
}
 
 /*******************************END OF FILE****************************/
