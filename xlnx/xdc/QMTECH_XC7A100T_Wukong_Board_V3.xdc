QMTECH_XC7A100T_Wukong_Board

set_property BITSTREAM.General.UnconstrainedPins {Allow} [current_design]

## Clock Signal
create_clock -add -name sys_clk_pin -period 20.00 -waveform {0 10} [get_ports clk_in]
set_property -dict { PACKAGE_PIN M21 IOSTANDARD LVCMOS33 } [get_ports { clk_in }];

## LEDs
set_property -dict { PACKAGE_PIN G20 IOSTANDARD LVCMOS33 } [get_ports { csr_out[0] }];
set_property -dict { PACKAGE_PIN G21 IOSTANDARD LVCMOS33 } [get_ports { uart_irq_o }];

## UART
set_property -dict { PACKAGE_PIN F3 IOSTANDARD LVCMOS33 } [get_ports { uart_rx_i }];
set_property -dict { PACKAGE_PIN E3 IOSTANDARD LVCMOS33 } [get_ports { uart_tx_o }];

## Pmod Header JC
#set_property -dict { PACKAGE_PIN    IOSTANDARD LVCMOS33 } [get_ports { spi_csn_o }]; #IO_L24P_T3_34 Sch=jb_p[3]
#set_property -dict { PACKAGE_PIN    IOSTANDARD LVCMOS33 } [get_ports { spi_mosi_o }]; #IO_L24N_T3_34 Sch=jb_n[3]
#set_property -dict { PACKAGE_PIN   IOSTANDARD LVCMOS33 } [get_ports { spi_gpio_o[0] }]; #IO_L19P_T3_34 Sch=jb_p[2]
#set_property -dict { PACKAGE_PIN   IOSTANDARD LVCMOS33 } [get_ports { spi_clk_o }]; #IO_L23N_T3_34 Sch=jb_n[4]

# GPIO LEDs
#set_property -dict { PACKAGE_PIN  IOSTANDARD LVCMOS33 } [get_ports { csr_out[1] }];
#set_property -dict { PACKAGE_PIN  IOSTANDARD LVCMOS33 } [get_ports { csr_out[2] }];
#set_property -dict { PACKAGE_PIN  IOSTANDARD LVCMOS33 } [get_ports { csr_out[3] }];
#set_property -dict { PACKAGE_PIN  IOSTANDARD LVCMOS33 } [get_ports { csr_out[4] }];
#set_property -dict { PACKAGE_PIN  IOSTANDARD LVCMOS33 } [get_ports { csr_out[5] }];

# Pushbuttons
set_property -dict { PACKAGE_PIN H7 IOSTANDARD LVCMOS33 } [get_ports { rst_cpu }];
set_property -dict { PACKAGE_PIN M6 IOSTANDARD LVCMOS33 } [get_ports { bootloader_i }];
#set_property -dict { PACKAGE_PIN IOSTANDARD LVCMOS33 } [get_ports { rst_clk }];
