#include "stm32l4xx.h"
#include "gpio.h"
#include "menu.h"
#include "stdio.h"
#include "ST7789v.h"
#include "XPT2046.h"
#include "usart.h"

uint32_t cntpage;            //页数
struct page *navigate[32]; //指针
extern Pen_Holder Pen_Point;//定义笔实体

enum
{
  KEY_UP = 1,
  KEY_DOWN,
  KEY_ADD,
  KEY_SUB,
  KEY_ENTER,
  KEY_LEAVE,
  KEY_START
} KEY_TYPE;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      在tg下添加子菜单类型
//  @param      tg              添加级的地址
//  @param      name            名称 最长13位
//  @param          v                           当前添加下一级菜单地址
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void add_subpage(struct page *tg, char *name, struct page *v)
{
  struct item it;
  uint8_t i;

  it.type = subpage;
  for (i = 0; i < 13 && name[i]; i++)
    it.name[i] = name[i];
  it.name[i] = 0;
  it.chf = 0;
  it.addr = v;
  tg->itemlist[(int)tg->count++] = it;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      在tg下添加值类型
//  @param      tg                                  这一级菜单
//  @param      name                            名称
//  @param          v                                           该值的大小
//  @param          changedCallBack             函数指针 执行修改该值的函数
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void add_value(struct page *tg, char *name, int *v, int16_t dt, void (*changedCallBack)())
{
  struct item it;
  uint8_t i;

  it.type = value;
  for (i = 0; i < 13 && name[i]; i++)
    it.name[i] = name[i];
  it.name[i] = 0;
  it.dlt = dt;
  it.chf = changedCallBack;
  it.addr = v;
  tg->itemlist[(int)tg->count++] = it;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      在tg下添加值类型
//  @param      tg                                  这一级菜单
//  @param      name                            名称
//  @param          v                                           该值的大小
//  @param          changedCallBack             函数指针 执行修改该值的函数
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void add_value_uint8(struct page *tg, char *name, uint8_t *v, uint8_t dt, void (*changedCallBack)())
{
  struct item it;
  uint8_t i;

  it.type = value;
  for (i = 0; i < 10 && name[i]; i++)
    it.name[i] = name[i];
  it.name[i] = 0;
  it.dlt = dt;
  it.chf = changedCallBack;
  it.addr = v;
  tg->itemlist[(int)tg->count++] = it;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      在tg下添加值类型
//  @param      tg                                  这一级菜单
//  @param      name                            名称
//  @param          v                                           该值的大小
//  @param          changedCallBack             函数指针 执行修改该值的函数
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void add_title(struct page *tg, char *name, char length)
{
  struct item it;
  uint8_t i;

  it.type = title;
  it.name[0] = ' ';
  for (i = 1; i < 6; i++)
    it.name[i] = '-';
  for (; i < 6 + length; i++)
    it.name[i] = name[i - 6];
  for (; i < 19; i++)
    it.name[i] = '-';
  it.name[i] = 0;
  tg->itemlist[(int)tg->count++] = it;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      在tg下添加浮点数类型
//  @param      tg                                  这一级菜单
//  @param      name                            名称
//  @param          v                                           该值的大小
//  @param          changedCallBack             函数指针 执行修改该值的函数
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void add_value_float(struct page *tg, char *name, float *v, float dt, void (*changedCallBack)())
{
  struct item it;
  uint8_t i;

  it.type = value_float;
  for (i = 0; i < 13 && name[i]; i++)
    it.name[i] = name[i];
  it.name[i] = 0;
  it.dlt_float = dt;
  it.chf = changedCallBack;
  it.addr = v;
  tg->itemlist[(int)tg->count++] = it;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      在tg下添加int32数类型
//  @param      tg                                  这一级菜单
//  @param      name                            名称
//  @param          v                                           该值的大小
//  @param          changedCallBack             函数指针 执行修改该值的函数
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void add_value_int32(struct page *tg, char *name, int32_t *v, int32_t dt, void (*changedCallBack)())
{
  struct item it;
  uint8_t i;

  it.type = value_int32;
  for (i = 0; i < 13 && name[i]; i++)
    it.name[i] = name[i];
  it.name[i] = 0;
  it.dlt = dt;
  it.chf = changedCallBack;
  it.addr = v;
  tg->itemlist[(int)tg->count++] = it;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      在tg下添加开关类型
//  @param      tg                  这一级菜单
//  @param      name            名称
//  @param          v                           为1 0两值 1为on 0为off
//  @param          operate             执行开关操作的函数
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void add_switc(struct page *tg, char *name, int16_t *v, void (*operate)())
{
  struct item it;
  uint8_t i;

  it.type = switc;

  for (i = 0; i < 18 && name[i]; i++)
    it.name[i] = name[i];
  it.name[i] = 0;
  it.dlt = 0;
  it.chf = operate;
  it.addr = v;
  tg->itemlist[(int)tg->count++] = it;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      在tg下添加函数类型
//  @param      tg                  这一级菜单
//  @param      name            名称
//  @param          v                           执行该操作函数的名称
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void add_func(struct page *tg, char *name, void (*v)())
{
  struct item it;
  uint8_t i;

  it.type = func;
  for (i = 0; i < 20 && name[i]; i++)
    it.name[i] = name[i];
  it.name[i] = 0;
  it.chf = 0;
  it.addr = v;
  tg->itemlist[(int)tg->count++] = it;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      显示光标
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LCD_BeginUpdate(void)
{
  LCD_Clear(WHITE);
  LCD_ShowString(0, (navigate[cntpage]->pos - navigate[cntpage]->rpos)*16, "-",BLACK);
}

void LCD_EndUpdate(void)
{
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      打印输出 菜单扫描
//  @param      full_update                 0 或 1 选择全部打印或者只刷新变化部分
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void MenuRender(char full_update)
{
  if (full_update)
  {
    LCD_BeginUpdate();

    int tmp = 0;
    for (int i = navigate[cntpage]->rpos; i < navigate[cntpage]->rpos + 19 && i < navigate[cntpage]->count; i++)
    {

      LCD_ShowString(8, (i - navigate[cntpage]->rpos)*16, navigate[cntpage]->itemlist[i].name, BLACK);

      switch (navigate[cntpage]->itemlist[i].type)
      {
      case title:
        break;
      case subpage:
        break;
      case switc:
        tmp = *((int16_t *)navigate[cntpage]->itemlist[i].addr);
        if (tmp)
          LCD_ShowString(18 * 6, (i - navigate[cntpage]->rpos)*16, "On",BLACK);
        else
          LCD_ShowString(18 * 6, (i - navigate[cntpage]->rpos)*16, "Off",BLACK);
        break;
      case func:
        break;
      case value:
      {
        if (!navigate[cntpage]->dymantic_page)
        {
          tmp = *((int16_t *)navigate[cntpage]->itemlist[i].addr);
          LCD_ShowNum((uint8_t)14 * 6, (uint8_t)(i - navigate[cntpage]->rpos)*16, (int16_t)tmp, 6,BLACK);
        }
        break;
      }
      case value_float:
      {
         if (!navigate[cntpage]->dymantic_page)
         {
           float temp_float = (*((float *)navigate[cntpage]->itemlist[i].addr));
           LCD_ShowFloat(14 * 6, (i - navigate[cntpage]->rpos)*16, temp_float, 2, 3,BLACK);
         }
         break;
      }
      default:
        break;
      }
    }

    if (!navigate[cntpage]->dymantic_page)
      LCD_ShowString(0, (navigate[cntpage]->pos - navigate[cntpage]->rpos)*16, "-",BLACK);

    LCD_EndUpdate();
  }
  else
  {
    if (navigate[cntpage]->dymantic_page) // ֻ���¶�̬ҳ��ֻ����value���͵������ʾ���
    {
      for (int i = navigate[cntpage]->rpos; i < navigate[cntpage]->rpos + 8 && i < navigate[cntpage]->count; i++)
      {
        switch (navigate[cntpage]->itemlist[i].type)
        {
        case value:
        {
          int16_t tmpv = *((int16_t *)navigate[cntpage]->itemlist[i].addr);
          LCD_ShowNum(15 * 6, i - navigate[cntpage]->rpos, tmpv, 6,BLACK);
          break;
        }
        case value_int32:
        {
          int32_t tmpv = *((int32_t *)navigate[cntpage]->itemlist[i].addr);
          LCD_ShowNum(15 * 3, i - navigate[cntpage]->rpos, tmpv, 6,BLACK);
          break;
        }
         case value_float:
         {
           float tmpv = *((float *)navigate[cntpage]->itemlist[i].addr);
           LCD_ShowFloat(14 * 6, (i - navigate[cntpage]->rpos)*16, tmpv, 2, 3,BLACK);
           break;
        }
        default:
          break;
        }
      }
      // pit_ms_init(TIM1_PIT, 10);
    }
  }
}

void MenuInit(struct page *mainpage)
{
  navigate[0] = mainpage;

  MenuRender(1);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      执行按键操作
//  @param      cmd                                     按键功能 0 为无操作
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void MenuCmd(char cmd)
{

  if (cmd)
  {
    switch (cmd)
    {
    case KEY_UP:
    {
      if (navigate[cntpage]->dymantic_page)
      {
        if (navigate[cntpage]->rpos > 0)
          navigate[cntpage]->rpos--;
      }
      else
      {
        if (navigate[cntpage]->pos > 0)
        {
          navigate[cntpage]->pos--;
          if (navigate[cntpage]->rpos > navigate[cntpage]->pos)
            navigate[cntpage]->rpos--;
        }
        else
        {
          navigate[cntpage]->pos = navigate[cntpage]->count - 1;
          if (navigate[cntpage]->count > 8)
            navigate[cntpage]->rpos = navigate[cntpage]->count - 8;
        }
      }

      break;
    }
    case KEY_DOWN:
    {
      if (navigate[cntpage]->dymantic_page)
      {
        if (navigate[cntpage]->rpos + 8 < navigate[cntpage]->count)
          navigate[cntpage]->rpos++;
      }
      else
      {
        if (navigate[cntpage]->pos < navigate[cntpage]->count - 1)
        {
          navigate[cntpage]->pos++;
          if (navigate[cntpage]->rpos < navigate[cntpage]->pos - 7)
            navigate[cntpage]->rpos++;
        }
        else
        {
          navigate[cntpage]->pos = 0;
          navigate[cntpage]->rpos = 0;
        }
      }

      break;
    }
    case KEY_ADD:
    {
      if (navigate[cntpage]->dymantic_page)
        return;

      if (navigate[cntpage]->itemlist[navigate[cntpage]->pos].type == value)
      {
        *((int16_t *)navigate[cntpage]->itemlist[navigate[cntpage]->pos].addr) += navigate[cntpage]->itemlist[navigate[cntpage]->pos].dlt;

        void (*func)() = navigate[cntpage]->itemlist[navigate[cntpage]->pos].chf;
        if (func)
          func();
      }
      if (navigate[cntpage]->itemlist[navigate[cntpage]->pos].type == value_float)
      {
        *((float *)navigate[cntpage]->itemlist[navigate[cntpage]->pos].addr) += navigate[cntpage]->itemlist[navigate[cntpage]->pos].dlt_float;

        void (*func)() = navigate[cntpage]->itemlist[navigate[cntpage]->pos].chf;
        if (func)
          func();
      }
      break;
    }
    case KEY_SUB:
    {
      if (navigate[cntpage]->dymantic_page)
        return;

      if (navigate[cntpage]->itemlist[navigate[cntpage]->pos].type == value)
      {
        *((int16_t *)navigate[cntpage]->itemlist[navigate[cntpage]->pos].addr) -= navigate[cntpage]->itemlist[navigate[cntpage]->pos].dlt;

        void (*func)() = navigate[cntpage]->itemlist[navigate[cntpage]->pos].chf;
        if (func)
          func();
      }
      if (navigate[cntpage]->itemlist[navigate[cntpage]->pos].type == value_float)
      {
        *((float *)navigate[cntpage]->itemlist[navigate[cntpage]->pos].addr) -= navigate[cntpage]->itemlist[navigate[cntpage]->pos].dlt_float;

        void (*func)() = navigate[cntpage]->itemlist[navigate[cntpage]->pos].chf;
        if (func)
          func();
      }
      break;
    }
    case KEY_ENTER:
    {
      if (navigate[cntpage]->itemlist[navigate[cntpage]->pos].type == subpage)
      {
        if (cntpage < 32)
        {
          navigate[cntpage + 1] = navigate[cntpage]->itemlist[navigate[cntpage]->pos].addr;
          cntpage++;
        }
      }
      else if (navigate[cntpage]->itemlist[navigate[cntpage]->pos].type == func)
      {
        void (*func)() = navigate[cntpage]->itemlist[navigate[cntpage]->pos].addr;
        if (func)
          func();
        MenuRender(0);
        return;
      }
      else if (navigate[cntpage]->itemlist[navigate[cntpage]->pos].type == switc)
      {
        int16_t *value = (int16_t *)navigate[cntpage]->itemlist[navigate[cntpage]->pos].addr;
        void (*func)() = navigate[cntpage]->itemlist[navigate[cntpage]->pos].chf;
        if (value)
        {
          if (*value)
            *value = 0;
          else
            *value = 1;
        }
        if (func)
          func();
      }
      break;
    }
    case KEY_LEAVE:
    {
      if (cntpage != 0)
      {
        cntpage--;
      }
      break;
    }
    case KEY_START:
    {
      break;
    }
    default:
      break;
    }
    MenuRender(1);
  }
  else
    MenuRender(0);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      key_scan();右上角数值是2048，2048，左下角0，0
//  @param      void
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32_t posx = 0;
uint32_t posy = 0;

uint8_t key_scan(void) 
{
	if(Pen_Point.Key_Sta == 1)
	{
		 Pen_Point.Key_Sta = 0;
     posx = Pen_Point.X*240.0/2048.0;
     posy = 360-Pen_Point.Y*360.0/2048.0;
		 debug_printf("\r\n[posx:%u,posy:%u]\r\n",posx,posy);
		 if(posx>=80 && posx <=130 && posy>=160 && posy<=220)
		 {
			 return KEY_UP;
		 }
		 if(posx>=80 && posx <=130 && posy>=240 && posy<=300)
		 {
			 return KEY_DOWN;
		 }
		 if(posx>=20 && posx <=70 && posy>=220 && posy<=260)
		 {
			 return KEY_ADD;
		 }
		 if(posx>=150 && posx <=200 && posy>=220 && posy<=260)
		 {
			 return KEY_SUB;
		 }
		 if(posx>=10 && posx <=50 && posy>=280 && posy<=320)
		 {
			 return KEY_ENTER;
		 }
		 if(posx>=170 && posx <=220 && posy>=280 && posy<=320)
		 {
			 return KEY_LEAVE;
		 }
	}
	return 0;
}
