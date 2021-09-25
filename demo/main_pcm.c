/*

DMA:
  32 bit transfer
  START_SOUND_FIFO
  DMA_REPEAT
  sound fifo destination address (fixed)
*/

#include "type.h"
#include "base.h"
#include "register.h"

extern u32 _binary_pcm_28_pcm_start;
extern u32 _binary_pcm_28_pcm_size;

static u32 wave[4] = {0};
static u32 offset = 0;

void _user_isr(void)
{
  *(volatile u16 *)(IO_REG + IME) = 0;

  *(volatile u32 *)(IO_REG + DMA1_SAD_L) = (u32)&wave[0];

  *(volatile u32 *)(IO_REG + DMA1_DAD_L) = (IO_REG + FIFO_A_L);

  wave[0] = ((u32 *)(&_binary_pcm_28_pcm_start))[offset + 0];
  wave[1] = ((u32 *)(&_binary_pcm_28_pcm_start))[offset + 1];
  wave[2] = ((u32 *)(&_binary_pcm_28_pcm_start))[offset + 2];
  wave[3] = ((u32 *)(&_binary_pcm_28_pcm_start))[offset + 3];
  offset += 4;
  if ((offset * 4) >= ((u32)&_binary_pcm_28_pcm_size))
    offset = 0;

  *(volatile u16 *)(IO_REG + DMA1_CNT_H) =
    ( DMAn_CNT_H__ENABLE
    | DMAn_CNT_H__INT_ENABLE
    | DMAn_CNT_H__START_SOUND_FIFO
    | DMAn_CNT_H__WORD_32_BIT
      //| DMAn_CNT_H__REPEAT
    | DMAn_CNT_H__SRC_INCREMENT
    | DMAn_CNT_H__DST_FIXED
    );

  *(volatile u16 *)(IO_REG + IF) = IE__DMA_1;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;
}

void _main(void)
{
  *(volatile u16 *)(IO_REG + FIFO_A_L) = wave[0];

  *(volatile u32 *)(IO_REG + DMA1_SAD_L) = (u32)&wave[0];

  *(volatile u32 *)(IO_REG + DMA1_DAD_L) = (IO_REG + FIFO_A_L);

  *(volatile u16 *)(IO_REG + DMA1_CNT_L) = 4;

  *(volatile u16 *)(IO_REG + DMA1_CNT_H) =
    ( DMAn_CNT_H__ENABLE
    | DMAn_CNT_H__INT_ENABLE
    | DMAn_CNT_H__START_IMMEDIATE
    | DMAn_CNT_H__WORD_32_BIT
      //| DMAn_CNT_H__REPEAT
    | DMAn_CNT_H__SRC_INCREMENT
    | DMAn_CNT_H__DST_FIXED
    );

  *(volatile u16 *)(IO_REG + SOUNDCNT_X) =
    ( SOUNDCNT_X__ENABLE
    );

  *(volatile u16 *)(IO_REG + SOUNDCNT_H) =
    ( SOUNDCNT_H__OUTPUT_SOUNDA_RATIO_FULL
    | SOUNDCNT_H__OUTPUT_SOUNDA_L
    | SOUNDCNT_H__OUTPUT_SOUNDA_R
    | SOUNDCNT_H__OUTPUT_SOUNDA_TIMER0
    | SOUNDCNT_H__OUTPUT_SOUNDA_RESET
    );

  *(volatile u32 *)(IWRAM_USER_ISR) = (u32)(&_user_isr);

  //  8000 = 2098
  // 22050 = 761
  // 44100 = 380
  *(volatile u16 *)(IO_REG + TM0CNT_L) = 0xffff - 761;

  *(volatile u16 *)(IO_REG + TM0CNT_H) =
    ( TM_CNT_H__ENABLE
    | TM_CNT_H__PRESCALAR_1
    );
  *(volatile u16 *)(IO_REG + IE) = IE__DMA_1;
  *(volatile u16 *)(IO_REG + IME) = IME__INT_MASTER_ENABLE;

  while (1) {
    *(volatile u8 *)(IO_REG + HALTCNT) = 0;
  }
}

void _start(void)
{
  while (1) _main();
}
