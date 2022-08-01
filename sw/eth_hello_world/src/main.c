#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "printf.h"
#include "riscv_csr_encoding.h"
#include "eth.h"
#include "eth_csr.h"

#define FREQ_SYSTEM     50000000
#define BR_UART         115200

//#define REAL_UART

#define ERR_CFG         0xFFFF0000
#define PRINT_ADDR      0xD0000008
#define LEDS_ADDR       0xD0000000
#define RST_CFG         0xC0000000
#define UART_TX         0xB000000C
#define UART_RX         0xB0000008
#define UART_STATS      0xB0000004
#define UART_CFG        0xB0000000
#define ETH_CFG         0x20000000

#define MTIMER_LSB      0xF0000000
#define MTIMER_MSB      0xF0000004
#define MTIMER_CMP_LSB  0xF0000008
#define MTIMER_CMP_MSB  0xF000000C

volatile uint32_t* const addr_leds  = (uint32_t*) LEDS_ADDR;
volatile uint32_t* const addr_print = (uint32_t*) PRINT_ADDR;
volatile uint32_t* const uart_stats = (uint32_t*) UART_STATS;
volatile uint32_t* const uart_print = (uint32_t*) UART_TX;
volatile uint32_t* const uart_rx    = (uint32_t*) UART_RX;
volatile uint32_t* const uart_cfg   = (uint32_t*) UART_CFG;
volatile uint32_t* const rst_cfg    = (uint32_t*) RST_CFG;
volatile uint32_t* const err_cfg    = (uint32_t*) ERR_CFG;
volatile uint32_t* const eth_cfg    = (uint32_t*) ETH_CFG;

volatile uint64_t* const mtimer     = (uint64_t*) MTIMER_LSB;
volatile uint64_t* const mtimer_cmp = (uint64_t*) MTIMER_CMP_LSB;

#ifdef UART_SIM
void _putchar(char character){
  *addr_print = character;
}
#else
void _putchar(char character){
  while((*uart_stats & 0x10000) == 0);
  *uart_print = character;
}
#endif

uint8_t gLEDsAddr = 0x20;
uint64_t gCounter = 0x00;
uint8_t str[100];

void irq_timer_callback(void){
  uint64_t mtime_half_second = *mtimer;
  mtime_half_second += 50000000;
  *mtimer_cmp = mtime_half_second;
  if (gLEDsAddr == 0x2) {
    gLEDsAddr = 0x20;
    *addr_leds = ~gLEDsAddr;
  }
  else {
    gLEDsAddr = (gLEDsAddr >> 1);
    *addr_leds = ~gLEDsAddr;
  }

  uint8_t    payload[] = {"Hello from NoX!!"};
  eth_write_infifo_data(payload,16);
  eth_send_pkt();
  fifo_ptr_t out_ptr = eth_outfifo_ptr();
  fifo_ptr_t in_ptr = eth_infifo_ptr();
  printf("\n\rMtimer IRQ! - RX len=%d \tIn: RD=%d WR=%d \tOut: RD=%d WR=%d", eth_get_recv_len(), in_ptr.rd_ptr, in_ptr.wr_ptr, out_ptr.rd_ptr, out_ptr.wr_ptr);
}

void irq_udp_callback(void){
  uint32_t data[100];
  if (eth_get_recv_len() == 16){
    printf("\n\r UDP recv: ");
    for (int i=0;i<4;i++)
      data[i] = eth_recv_data();
    data[4] = '\0';
    printf("%s", data);
    eth_clr_infifo_ptr();
  }
  else {
    printf("\n\r Unexpected pkt received!");
    eth_clr_infifo_ptr();
  }
  eth_clr_irqs();
}

int main(void) {
  uint8_t leds_out = 0x01;
  int i = 0;

  // 50MHz / 115200 = 434
  *uart_cfg = FREQ_SYSTEM/BR_UART;

  /*********** Local Ethernet CFG **********/
  eth_local_cfg_t local_cfg;

  local_cfg.ip_addr =  0xc0a80082; // 0xc0a80182; // 192.168.001.130
  local_cfg.ip_gateway = 0xc0a80001; // 192.168.001.001
  local_cfg.subnet_mask = 0xFFFFFF00; // 255.255.255.000
  local_cfg.mac_addr.val = 0x000A35A23456; // Xilinx Inc.
  eth_set_local_cfg(local_cfg);

  /********** Send ethernet CFG ************/
  eth_cfg_t send_cfg;

  send_cfg.len = 16;
  send_cfg.src_port = 1234;
  send_cfg.dst_port = 1234;
  send_cfg.ip_addr = 0xc0a8000A; // 0xc0a8015A; // 192.168.001.090
  send_cfg.mac_addr.val = 0xe0b55ff33298;
  eth_set_send_cfg(send_cfg);
  //0x00e04c000752 - Wired     0xe0b55ff33298 - Wireless;

  /********* Filter ethernet cfg ***********/
  eth_filter_cfg_t filter_cfg;
  filter_cfg.filter_en = 0x1;
  filter_cfg.udp_port = 1234;
  filter_cfg.ip_addr = send_cfg.ip_addr;
  eth_set_filter(filter_cfg);

  uint64_t mtime_half_second = *mtimer;
  mtime_half_second += 2500;
  *mtimer_cmp = mtime_half_second;

  set_csr(mstatus,MSTATUS_MIE);
  set_csr(mie,1<<IRQ_M_TIMER);
  set_csr(mie,1<<IRQ_M_SOFT);
  while(1){
    wfi();
  }
}
