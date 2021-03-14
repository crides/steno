EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 7850 3500 2    50   BiDi ~ 0
ROW1
Text GLabel 7850 3600 2    50   BiDi ~ 0
ROW2
Text GLabel 7850 1200 2    50   BiDi ~ 0
SCLK_5V
Text GLabel 7850 1300 2    50   BiDi ~ 0
MOSI_5V
Text GLabel 7850 1400 2    50   BiDi ~ 0
MISO
Text GLabel 7850 2400 2    50   BiDi ~ 0
ROW4
Text GLabel 7850 2500 2    50   BiDi ~ 0
ROW5
Text GLabel 7850 2600 2    50   BiDi ~ 0
ROW3
Text GLabel 6150 3450 2    50   BiDi ~ 0
MOSI_3V3
Text GLabel 5200 3250 0    50   BiDi ~ 0
MISO
Text GLabel 6150 3350 2    50   BiDi ~ 0
SCLK_3V3
Text GLabel 5200 3150 0    50   BiDi ~ 0
CS_3V3
$Comp
L steno:MT25QL128ABA1ESE U2
U 1 1 5ECCCE62
P 5600 3250
F 0 "U2" H 5675 3575 50  0000 C CNN
F 1 "MT25QL128ABA1ESE" H 5675 3484 50  0000 C CNN
F 2 "Package_SO:SOP-8_5.28x5.23mm_P1.27mm" H 5600 2850 50  0001 C CNN
F 3 "" H 5600 2850 50  0001 C CNN
	1    5600 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5F6E3053
P 8000 3200
F 0 "R4" V 7793 3200 50  0000 C CNN
F 1 "10k" V 7884 3200 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7930 3200 50  0001 C CNN
F 3 "~" H 8000 3200 50  0001 C CNN
	1    8000 3200
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 5F6E5DBF
P 6500 2100
F 0 "R2" V 6400 2100 50  0000 C CNN
F 1 " 22" V 6500 2100 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6430 2100 50  0001 C CNN
F 3 "~" H 6500 2100 50  0001 C CNN
	1    6500 2100
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C4
U 1 1 5F6E722B
P 6550 2400
F 0 "C4" V 6650 2400 50  0000 C CNN
F 1 "1uF" V 6700 2400 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6588 2250 50  0001 C CNN
F 3 "~" H 6550 2400 50  0001 C CNN
	1    6550 2400
	0    1    1    0   
$EndComp
$Comp
L Device:Crystal_GND24_Small Y1
U 1 1 5F6E834C
P 6350 1400
F 0 "Y1" V 6304 1544 50  0000 L CNN
F 1 "16MHz" V 6200 1300 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_EuroQuartz_MT-4Pin_3.2x2.5mm" H 6350 1400 50  0001 C CNN
F 3 "~" H 6350 1400 50  0001 C CNN
	1    6350 1400
	0    1    1    0   
$EndComp
$Comp
L Device:Polyfuse_Small F1
U 1 1 5F6EA8C1
P 3950 4700
F 0 "F1" V 3745 4700 50  0000 C CNN
F 1 "Polyfuse_Small" V 3836 4700 50  0000 C CNN
F 2 "Fuse:Fuse_1206_3216Metric_Castellated" H 4000 4500 50  0001 L CNN
F 3 "~" H 3950 4700 50  0001 C CNN
	1    3950 4700
	0    1    1    0   
$EndComp
$Comp
L Regulator_Linear:MCP1700-3302E_SOT23 U3
U 1 1 5F6EBB01
P 5400 2350
F 0 "U3" H 5400 2592 50  0000 C CNN
F 1 "MCP1700-3302E_SOT23" H 5400 2501 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5400 2575 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/20001826D.pdf" H 5400 2350 50  0001 C CNN
	1    5400 2350
	1    0    0    -1  
$EndComp
$Comp
L MCU_Microchip_ATmega:ATmega32U4-AU U1
U 1 1 5F6DA648
P 7250 2600
F 0 "U1" H 7250 711 50  0000 C CNN
F 1 "ATmega32U4-AU" H 7250 620 50  0000 C CNN
F 2 "Package_QFP:TQFP-44_10x10mm_P0.8mm" H 7250 2600 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7766-8-bit-AVR-ATmega16U4-32U4_Datasheet.pdf" H 7250 2600 50  0001 C CNN
	1    7250 2600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5F7162AC
P 6500 2200
F 0 "R3" V 6600 2200 50  0000 C CNN
F 1 " 22" V 6500 2200 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6430 2200 50  0001 C CNN
F 3 "~" H 6500 2200 50  0001 C CNN
	1    6500 2200
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR011
U 1 1 5F717CA4
P 6650 1900
F 0 "#PWR011" H 6650 1750 50  0001 C CNN
F 1 "+5V" V 6665 2028 50  0000 L CNN
F 2 "" H 6650 1900 50  0001 C CNN
F 3 "" H 6650 1900 50  0001 C CNN
	1    6650 1900
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V3 #PWR012
U 1 1 5F718F7E
P 6150 3150
F 0 "#PWR012" H 6150 3000 50  0001 C CNN
F 1 "+3V3" H 6165 3323 50  0000 C CNN
F 2 "" H 6150 3150 50  0001 C CNN
F 3 "" H 6150 3150 50  0001 C CNN
	1    6150 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 4400 7250 4400
Text GLabel 6350 2200 0    50   Input ~ 0
D-
Text GLabel 6350 2100 0    50   Input ~ 0
D+
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5F79174E
P 5150 4450
F 0 "#FLG01" H 5150 4525 50  0001 C CNN
F 1 "PWR_FLAG" H 5150 4623 50  0000 C CNN
F 2 "" H 5150 4450 50  0001 C CNN
F 3 "~" H 5150 4450 50  0001 C CNN
	1    5150 4450
	-1   0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR021
U 1 1 5F791D12
P 5150 4450
F 0 "#PWR021" H 5150 4300 50  0001 C CNN
F 1 "+3V3" H 5165 4623 50  0000 C CNN
F 2 "" H 5150 4450 50  0001 C CNN
F 3 "" H 5150 4450 50  0001 C CNN
	1    5150 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5F79E510
P 8150 3200
F 0 "#PWR016" H 8150 2950 50  0001 C CNN
F 1 "GND" V 8155 3072 50  0000 R CNN
F 2 "" H 8150 3200 50  0001 C CNN
F 3 "" H 8150 3200 50  0001 C CNN
	1    8150 3200
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR01
U 1 1 5F7A2679
P 3600 4700
F 0 "#PWR01" H 3600 4550 50  0001 C CNN
F 1 "VCC" H 3615 4873 50  0000 C CNN
F 2 "" H 3600 4700 50  0001 C CNN
F 3 "" H 3600 4700 50  0001 C CNN
	1    3600 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 4700 3600 4700
$Comp
L power:+5V #PWR02
U 1 1 5F7AC381
P 4050 4700
F 0 "#PWR02" H 4050 4550 50  0001 C CNN
F 1 "+5V" H 4065 4873 50  0000 C CNN
F 2 "" H 4050 4700 50  0001 C CNN
F 3 "" H 4050 4700 50  0001 C CNN
	1    4050 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 5F7ACCAA
P 6450 2400
F 0 "#PWR013" H 6450 2150 50  0001 C CNN
F 1 "GND" V 6455 2272 50  0000 R CNN
F 2 "" H 6450 2400 50  0001 C CNN
F 3 "" H 6450 2400 50  0001 C CNN
	1    6450 2400
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5F7B17CE
P 5400 2650
F 0 "#PWR020" H 5400 2400 50  0001 C CNN
F 1 "GND" H 5405 2477 50  0000 C CNN
F 2 "" H 5400 2650 50  0001 C CNN
F 3 "" H 5400 2650 50  0001 C CNN
	1    5400 2650
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR019
U 1 1 5F7B2A35
P 5700 2350
F 0 "#PWR019" H 5700 2200 50  0001 C CNN
F 1 "+3V3" H 5715 2523 50  0000 C CNN
F 2 "" H 5700 2350 50  0001 C CNN
F 3 "" H 5700 2350 50  0001 C CNN
	1    5700 2350
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR025
U 1 1 5F7B329A
P 7250 4400
F 0 "#PWR025" H 7250 4150 50  0001 C CNN
F 1 "GND" H 7255 4227 50  0000 C CNN
F 2 "" H 7250 4400 50  0001 C CNN
F 3 "" H 7250 4400 50  0001 C CNN
	1    7250 4400
	1    0    0    -1  
$EndComp
Connection ~ 7250 4400
$Comp
L power:GND #PWR06
U 1 1 5F7E9379
P 6400 800
F 0 "#PWR06" H 6400 550 50  0001 C CNN
F 1 "GND" V 6405 672 50  0000 R CNN
F 2 "" H 6400 800 50  0001 C CNN
F 3 "" H 6400 800 50  0001 C CNN
	1    6400 800 
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C2
U 1 1 5F7F0325
P 6150 1500
F 0 "C2" V 6200 1300 50  0000 C CNN
F 1 "22pF" V 6200 1500 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6150 1500 50  0001 C CNN
F 3 "~" H 6150 1500 50  0001 C CNN
	1    6150 1500
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C1
U 1 1 5F7F08B1
P 6150 1300
F 0 "C1" V 5921 1300 50  0000 C CNN
F 1 "22pF" V 6012 1300 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6150 1300 50  0001 C CNN
F 3 "~" H 6150 1300 50  0001 C CNN
	1    6150 1300
	0    1    1    0   
$EndComp
Wire Wire Line
	6650 1500 6350 1500
Connection ~ 6350 1500
Wire Wire Line
	6350 1500 6250 1500
Wire Wire Line
	6650 1300 6350 1300
Connection ~ 6350 1300
Wire Wire Line
	6350 1300 6250 1300
Wire Wire Line
	6450 1400 6450 1600
Wire Wire Line
	6450 1600 6050 1600
Wire Wire Line
	6050 1600 6050 1500
Connection ~ 6050 1500
Wire Wire Line
	6050 1500 6050 1400
Wire Wire Line
	6250 1400 6050 1400
Connection ~ 6050 1400
Wire Wire Line
	6050 1400 6050 1300
$Comp
L power:GND #PWR07
U 1 1 5F7FB7B3
P 6050 1400
F 0 "#PWR07" H 6050 1150 50  0001 C CNN
F 1 "GND" V 6055 1272 50  0000 R CNN
F 2 "" H 6050 1400 50  0001 C CNN
F 3 "" H 6050 1400 50  0001 C CNN
	1    6050 1400
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C10
U 1 1 5F80B738
P 5700 2450
F 0 "C10" H 5608 2404 50  0000 R CNN
F 1 "1uF" H 5608 2495 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5738 2300 50  0001 C CNN
F 3 "~" H 5700 2450 50  0001 C CNN
	1    5700 2450
	-1   0    0    1   
$EndComp
Connection ~ 5700 2350
$Comp
L Device:C_Small C9
U 1 1 5F80F482
P 5100 2450
F 0 "C9" H 5008 2404 50  0000 R CNN
F 1 "1uF" H 5008 2495 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5138 2300 50  0001 C CNN
F 3 "~" H 5100 2450 50  0001 C CNN
	1    5100 2450
	-1   0    0    1   
$EndComp
Connection ~ 5100 2350
Wire Wire Line
	5100 2550 5100 2650
Wire Wire Line
	5100 2650 5400 2650
Connection ~ 5400 2650
Wire Wire Line
	5400 2650 5700 2650
Wire Wire Line
	5700 2650 5700 2550
$Comp
L power:+5V #PWR04
U 1 1 5F8171A9
P 7150 800
F 0 "#PWR04" H 7150 650 50  0001 C CNN
F 1 "+5V" H 7165 973 50  0000 C CNN
F 2 "" H 7150 800 50  0001 C CNN
F 3 "" H 7150 800 50  0001 C CNN
	1    7150 800 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 800  7250 800 
Connection ~ 7150 800 
Connection ~ 7250 800 
Wire Wire Line
	7250 800  7350 800 
$Comp
L power:+5V #PWR08
U 1 1 5F81F672
P 5450 1600
F 0 "#PWR08" H 5450 1450 50  0001 C CNN
F 1 "+5V" H 5465 1773 50  0000 C CNN
F 2 "" H 5450 1600 50  0001 C CNN
F 3 "" H 5450 1600 50  0001 C CNN
	1    5450 1600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5F8221D0
P 5450 1800
F 0 "#PWR09" H 5450 1550 50  0001 C CNN
F 1 "GND" H 5455 1627 50  0000 C CNN
F 2 "" H 5450 1800 50  0001 C CNN
F 3 "" H 5450 1800 50  0001 C CNN
	1    5450 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C5
U 1 1 5F822919
P 5050 1700
F 0 "C5" H 4958 1654 50  0000 R CNN
F 1 "104" H 4958 1745 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5088 1550 50  0001 C CNN
F 3 "~" H 5050 1700 50  0001 C CNN
	1    5050 1700
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C8
U 1 1 5F823D04
P 5700 1700
F 0 "C8" H 5608 1654 50  0000 R CNN
F 1 "10uF" H 5608 1745 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5738 1550 50  0001 C CNN
F 3 "~" H 5700 1700 50  0001 C CNN
	1    5700 1700
	-1   0    0    1   
$EndComp
Wire Wire Line
	5700 1600 5450 1600
Connection ~ 5450 1600
Connection ~ 5450 1800
Wire Wire Line
	5450 1800 5700 1800
$Comp
L Device:C_Small C3
U 1 1 5F8C8215
P 6550 1700
F 0 "C3" V 6650 1700 50  0000 C CNN
F 1 "1uF" V 6450 1700 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6588 1550 50  0001 C CNN
F 3 "~" H 6550 1700 50  0001 C CNN
	1    6550 1700
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5F8CA7F9
P 6450 1700
F 0 "#PWR010" H 6450 1450 50  0001 C CNN
F 1 "GND" V 6455 1572 50  0000 R CNN
F 2 "" H 6450 1700 50  0001 C CNN
F 3 "" H 6450 1700 50  0001 C CNN
	1    6450 1700
	0    1    1    0   
$EndComp
Wire Wire Line
	5050 1800 5250 1800
Wire Wire Line
	5050 1600 5250 1600
$Comp
L Device:C_Small C7
U 1 1 5F914272
P 5450 1700
F 0 "C7" H 5358 1654 50  0000 R CNN
F 1 "104" H 5358 1745 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5488 1550 50  0001 C CNN
F 3 "~" H 5450 1700 50  0001 C CNN
	1    5450 1700
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C6
U 1 1 5F9146F5
P 5250 1700
F 0 "C6" H 5158 1654 50  0000 R CNN
F 1 "104" H 5158 1745 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5288 1550 50  0001 C CNN
F 3 "~" H 5250 1700 50  0001 C CNN
	1    5250 1700
	-1   0    0    1   
$EndComp
Connection ~ 5250 1800
Wire Wire Line
	5250 1800 5450 1800
Connection ~ 5250 1600
Wire Wire Line
	5250 1600 5450 1600
$Comp
L power:GND #PWR024
U 1 1 5F79887B
P 5400 3850
F 0 "#PWR024" H 5400 3600 50  0001 C CNN
F 1 "GND" H 5405 3677 50  0000 C CNN
F 2 "" H 5400 3850 50  0001 C CNN
F 3 "" H 5400 3850 50  0001 C CNN
	1    5400 3850
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR022
U 1 1 5F7935BA
P 5550 4450
F 0 "#PWR022" H 5550 4300 50  0001 C CNN
F 1 "+5V" H 5565 4623 50  0000 C CNN
F 2 "" H 5550 4450 50  0001 C CNN
F 3 "" H 5550 4450 50  0001 C CNN
	1    5550 4450
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5F790BAA
P 5550 4450
F 0 "#FLG02" H 5550 4525 50  0001 C CNN
F 1 "PWR_FLAG" H 5550 4623 50  0000 C CNN
F 2 "" H 5550 4450 50  0001 C CNN
F 3 "~" H 5550 4450 50  0001 C CNN
	1    5550 4450
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG04
U 1 1 5F6F1B0A
P 5400 3850
F 0 "#FLG04" H 5400 3925 50  0001 C CNN
F 1 "PWR_FLAG" H 5400 4023 50  0000 C CNN
F 2 "" H 5400 3850 50  0001 C CNN
F 3 "~" H 5400 3850 50  0001 C CNN
	1    5400 3850
	-1   0    0    1   
$EndComp
$Comp
L steno:LCD2.2in U4
U 1 1 5F837536
P 6550 5550
F 0 "U4" H 6878 5601 50  0000 L CNN
F 1 "LCD2.2in" H 6878 5510 50  0000 L CNN
F 2 "steno:LCD2.2in" H 6550 4950 50  0001 C CNN
F 3 "" H 6550 4950 50  0001 C CNN
	1    6550 5550
	1    0    0    -1  
$EndComp
Text GLabel 6350 5500 0    50   BiDi ~ 0
MISO
Text GLabel 6350 5200 0    50   Input ~ 0
C\D_3V3
Text GLabel 6350 5300 0    50   Input ~ 0
LCD_CS_3V3
$Comp
L Device:R R5
U 1 1 5F8001B1
P 6300 3150
F 0 "R5" V 6093 3150 50  0000 C CNN
F 1 "10k" V 6184 3150 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 6230 3150 50  0001 C CNN
F 3 "~" H 6300 3150 50  0001 C CNN
	1    6300 3150
	0    1    1    0   
$EndComp
Connection ~ 6150 3150
Wire Wire Line
	6450 3150 6450 3250
Wire Wire Line
	6450 3250 6150 3250
Wire Wire Line
	6450 3150 6450 2900
Wire Wire Line
	6450 2900 4900 2900
Wire Wire Line
	4900 2900 4900 3350
Wire Wire Line
	4900 3350 5200 3350
Connection ~ 6450 3150
Text GLabel 7850 2900 2    50   Input ~ 0
LCD_CS
$Comp
L power:+5V #PWR018
U 1 1 5F7B2059
P 5100 2350
F 0 "#PWR018" H 5100 2200 50  0001 C CNN
F 1 "+5V" H 5115 2523 50  0000 C CNN
F 2 "" H 5100 2350 50  0001 C CNN
F 3 "" H 5100 2350 50  0001 C CNN
	1    5100 2350
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5F6E92B3
P 6600 800
F 0 "SW1" H 6600 1085 50  0000 C CNN
F 1 "SW_Push" H 6600 994 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_B3U-1000P" H 6600 1000 50  0001 C CNN
F 3 "~" H 6600 1000 50  0001 C CNN
	1    6600 800 
	1    0    0    -1  
$EndComp
Text GLabel 1000 3300 0    50   BiDi ~ 0
ROW5
Text GLabel 1000 2650 0    50   BiDi ~ 0
ROW4
Text GLabel 1000 2000 0    50   BiDi ~ 0
ROW3
Text GLabel 1000 1350 0    50   BiDi ~ 0
ROW2
Text GLabel 2250 1050 3    50   BiDi ~ 0
COL4
Text GLabel 1850 1050 3    50   BiDi ~ 0
COL3
Text GLabel 1450 1050 3    50   BiDi ~ 0
COL2
Text GLabel 1050 1050 3    50   BiDi ~ 0
COL1
Text GLabel 3050 1050 3    50   BiDi ~ 0
COL6
Text GLabel 2650 1050 3    50   BiDi ~ 0
COL5
Text GLabel 1000 700  0    50   BiDi ~ 0
ROW1
Text GLabel 7850 2700 2    50   Input ~ 0
C\D
Text GLabel 7850 3000 2    50   BiDi ~ 0
COL1
Text GLabel 7850 1600 2    50   BiDi ~ 0
COL3
Text GLabel 7850 1500 2    50   BiDi ~ 0
COL2
Text GLabel 7850 1700 2    50   BiDi ~ 0
COL4
Text GLabel 7850 2100 2    50   BiDi ~ 0
COL6
Text GLabel 7850 2000 2    50   BiDi ~ 0
COL5
Text GLabel 7850 2300 2    50   Input ~ 0
LITE_5V
Text GLabel 7850 2800 2    50   BiDi ~ 0
CS_5V
Text GLabel 9000 2350 0    50   BiDi ~ 0
CS_5V
Text GLabel 9000 1400 0    50   BiDi ~ 0
MOSI_5V
Text GLabel 9000 1900 0    50   BiDi ~ 0
SCLK_5V
Text GLabel 9600 2350 2    50   BiDi ~ 0
CS_3V3
Text GLabel 9600 1900 2    50   BiDi ~ 0
SCLK_3V3
Text GLabel 9600 1400 2    50   BiDi ~ 0
MOSI_3V3
Text GLabel 6350 5100 0    50   BiDi ~ 0
SCLK_3V3
Text GLabel 6350 5400 0    50   BiDi ~ 0
MOSI_3V3
$Comp
L power:GND #PWR0108
U 1 1 5F8C0651
P 6350 4900
F 0 "#PWR0108" H 6350 4650 50  0001 C CNN
F 1 "GND" V 6355 4727 50  0000 C CNN
F 2 "" H 6350 4900 50  0001 C CNN
F 3 "" H 6350 4900 50  0001 C CNN
	1    6350 4900
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5F8C4316
P 5200 3450
F 0 "#PWR0109" H 5200 3200 50  0001 C CNN
F 1 "GND" H 5205 3277 50  0000 C CNN
F 2 "" H 5200 3450 50  0001 C CNN
F 3 "" H 5200 3450 50  0001 C CNN
	1    5200 3450
	0    1    1    0   
$EndComp
$Comp
L 4xxx:4050 U5
U 1 1 5F9368CD
P 9300 1900
F 0 "U5" H 9300 2217 50  0000 C CNN
F 1 "4050" H 9300 2126 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 9300 1900 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/intersil/documents/cd40/cd4050bms.pdf" H 9300 1900 50  0001 C CNN
	1    9300 1900
	1    0    0    -1  
$EndComp
$Comp
L 4xxx:4050 U5
U 2 1 5F937A2D
P 9300 1400
F 0 "U5" H 9300 1717 50  0000 C CNN
F 1 "4050" H 9300 1626 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 9300 1400 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/intersil/documents/cd40/cd4050bms.pdf" H 9300 1400 50  0001 C CNN
	2    9300 1400
	1    0    0    -1  
$EndComp
$Comp
L 4xxx:4050 U5
U 3 1 5F938B9C
P 9300 2350
F 0 "U5" H 9300 2667 50  0000 C CNN
F 1 "4050" H 9300 2576 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 9300 2350 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/intersil/documents/cd40/cd4050bms.pdf" H 9300 2350 50  0001 C CNN
	3    9300 2350
	1    0    0    -1  
$EndComp
$Comp
L 4xxx:4050 U5
U 7 1 5F93BD17
P 8750 3700
F 0 "U5" H 8980 3746 50  0000 L CNN
F 1 "4050" H 8980 3655 50  0000 L CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 8750 3700 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/intersil/documents/cd40/cd4050bms.pdf" H 8750 3700 50  0001 C CNN
	7    8750 3700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5F9AA961
P 8750 4200
F 0 "#PWR0102" H 8750 3950 50  0001 C CNN
F 1 "GND" H 8755 4027 50  0000 C CNN
F 2 "" H 8750 4200 50  0001 C CNN
F 3 "" H 8750 4200 50  0001 C CNN
	1    8750 4200
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0103
U 1 1 5F9ACDB8
P 8750 3200
F 0 "#PWR0103" H 8750 3050 50  0001 C CNN
F 1 "+3V3" H 8765 3373 50  0000 C CNN
F 2 "" H 8750 3200 50  0001 C CNN
F 3 "" H 8750 3200 50  0001 C CNN
	1    8750 3200
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 602B341B
P 8050 1100
F 0 "SW2" H 8050 1385 50  0000 C CNN
F 1 "SW_Push" H 8050 1294 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_B3U-1000P" H 8050 1300 50  0001 C CNN
F 3 "~" H 8050 1300 50  0001 C CNN
	1    8050 1100
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 602B4471
P 8250 1100
F 0 "#PWR0105" H 8250 850 50  0001 C CNN
F 1 "GND" V 8255 972 50  0000 R CNN
F 2 "" H 8250 1100 50  0001 C CNN
F 3 "" H 8250 1100 50  0001 C CNN
	1    8250 1100
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 60376206
P 6350 5600
F 0 "#PWR0106" H 6350 5350 50  0001 C CNN
F 1 "GND" V 6355 5427 50  0000 C CNN
F 2 "" H 6350 5600 50  0001 C CNN
F 3 "" H 6350 5600 50  0001 C CNN
	1    6350 5600
	0    1    1    0   
$EndComp
$Comp
L power:+3V3 #PWR0107
U 1 1 6037CF2A
P 6350 5700
F 0 "#PWR0107" H 6350 5550 50  0001 C CNN
F 1 "+3V3" V 6350 5900 50  0000 C CNN
F 2 "" H 6350 5700 50  0001 C CNN
F 3 "" H 6350 5700 50  0001 C CNN
	1    6350 5700
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V3 #PWR0110
U 1 1 603BFB5D
P 6350 5800
F 0 "#PWR0110" H 6350 5650 50  0001 C CNN
F 1 "+3V3" V 6350 6000 50  0000 C CNN
F 2 "" H 6350 5800 50  0001 C CNN
F 3 "" H 6350 5800 50  0001 C CNN
	1    6350 5800
	0    -1   -1   0   
$EndComp
Text GLabel 9950 3200 0    50   Input ~ 0
C\D
Text GLabel 10550 3200 2    50   Input ~ 0
C\D_3V3
$Comp
L 4xxx:4050 U5
U 5 1 604ACEC0
P 10250 3200
F 0 "U5" H 10250 3517 50  0000 C CNN
F 1 "4050" H 10250 3426 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 10250 3200 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/intersil/documents/cd40/cd4050bms.pdf" H 10250 3200 50  0001 C CNN
	5    10250 3200
	1    0    0    -1  
$EndComp
$Comp
L 4xxx:4050 U5
U 6 1 604ADD3F
P 10250 3650
F 0 "U5" H 10250 3967 50  0000 C CNN
F 1 "4050" H 10250 3876 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 10250 3650 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/intersil/documents/cd40/cd4050bms.pdf" H 10250 3650 50  0001 C CNN
	6    10250 3650
	1    0    0    -1  
$EndComp
Text GLabel 9950 3650 0    50   Input ~ 0
LCD_CS
Text GLabel 10550 3650 2    50   Input ~ 0
LCD_CS_3V3
$Comp
L Device:R R7
U 1 1 60662AC5
P 6200 5900
F 0 "R7" V 6200 5650 50  0000 C CNN
F 1 "220" V 6200 5900 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6130 5900 50  0001 C CNN
F 3 "~" H 6200 5900 50  0001 C CNN
	1    6200 5900
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 60667545
P 6200 6000
F 0 "R8" V 6200 5750 50  0000 C CNN
F 1 "220" V 6200 6000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6130 6000 50  0001 C CNN
F 3 "~" H 6200 6000 50  0001 C CNN
	1    6200 6000
	0    1    1    0   
$EndComp
$Comp
L Device:R R9
U 1 1 606677B7
P 6200 6100
F 0 "R9" V 6200 5850 50  0000 C CNN
F 1 "220" V 6200 6100 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6130 6100 50  0001 C CNN
F 3 "~" H 6200 6100 50  0001 C CNN
	1    6200 6100
	0    1    1    0   
$EndComp
$Comp
L Device:R R10
U 1 1 60667ADE
P 6200 6200
F 0 "R10" V 6200 5950 50  0000 C CNN
F 1 "220" V 6200 6200 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6130 6200 50  0001 C CNN
F 3 "~" H 6200 6200 50  0001 C CNN
	1    6200 6200
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 60667DD4
P 5850 5900
F 0 "#PWR0111" H 5850 5650 50  0001 C CNN
F 1 "GND" V 5855 5727 50  0000 C CNN
F 2 "" H 5850 5900 50  0001 C CNN
F 3 "" H 5850 5900 50  0001 C CNN
	1    5850 5900
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 60668030
P 5850 6000
F 0 "#PWR0112" H 5850 5750 50  0001 C CNN
F 1 "GND" V 5855 5827 50  0000 C CNN
F 2 "" H 5850 6000 50  0001 C CNN
F 3 "" H 5850 6000 50  0001 C CNN
	1    5850 6000
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 6066832D
P 5850 6100
F 0 "#PWR0113" H 5850 5850 50  0001 C CNN
F 1 "GND" V 5855 5927 50  0000 C CNN
F 2 "" H 5850 6100 50  0001 C CNN
F 3 "" H 5850 6100 50  0001 C CNN
	1    5850 6100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 6066863E
P 5850 6200
F 0 "#PWR0114" H 5850 5950 50  0001 C CNN
F 1 "GND" V 5855 6027 50  0000 C CNN
F 2 "" H 5850 6200 50  0001 C CNN
F 3 "" H 5850 6200 50  0001 C CNN
	1    5850 6200
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 6200 5850 6200
Wire Wire Line
	6050 6100 5850 6100
Wire Wire Line
	5850 6000 6050 6000
Wire Wire Line
	6050 5900 5850 5900
$Comp
L 4xxx:4050 U5
U 4 1 5F93A4B9
P 9300 2800
F 0 "U5" H 9300 3117 50  0000 C CNN
F 1 "4050" H 9300 3026 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 9300 2800 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/intersil/documents/cd40/cd4050bms.pdf" H 9300 2800 50  0001 C CNN
	4    9300 2800
	1    0    0    -1  
$EndComp
Text GLabel 6650 1100 0    50   Input ~ 0
RESET
Text GLabel 6800 800  2    50   Input ~ 0
RESET
$Comp
L pg1350:PG1350 K1
U 1 1 6033885D
P 1200 900
F 0 "K1" H 1150 1050 50  0000 L CNN
F 1 "PG1350" H 1050 850 50  0000 L CNN
F 2 "pg1350:PG1350" H 1200 1050 50  0001 C CNN
F 3 "" H 1200 1050 50  0001 C CNN
	1    1200 900 
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K2
U 1 1 6033A34B
P 1600 900
F 0 "K2" H 1550 1050 50  0000 L CNN
F 1 "PG1350" H 1450 850 50  0000 L CNN
F 2 "pg1350:PG1350" H 1600 1050 50  0001 C CNN
F 3 "" H 1600 1050 50  0001 C CNN
	1    1600 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 700  1400 700 
$Comp
L pg1350:PG1350 K3
U 1 1 6034B41D
P 2000 900
F 0 "K3" H 1950 1050 50  0000 L CNN
F 1 "PG1350" H 1850 850 50  0000 L CNN
F 2 "pg1350:PG1350" H 2000 1050 50  0001 C CNN
F 3 "" H 2000 1050 50  0001 C CNN
	1    2000 900 
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K4
U 1 1 6034C827
P 2400 900
F 0 "K4" H 2350 1050 50  0000 L CNN
F 1 "PG1350" H 2250 850 50  0000 L CNN
F 2 "pg1350:PG1350" H 2400 1050 50  0001 C CNN
F 3 "" H 2400 1050 50  0001 C CNN
	1    2400 900 
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K5
U 1 1 6034D03C
P 2800 900
F 0 "K5" H 2750 1050 50  0000 L CNN
F 1 "PG1350" H 2650 850 50  0000 L CNN
F 2 "pg1350:PG1350" H 2800 1050 50  0001 C CNN
F 3 "" H 2800 1050 50  0001 C CNN
	1    2800 900 
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K6
U 1 1 6034D5E2
P 3200 900
F 0 "K6" H 3150 1050 50  0000 L CNN
F 1 "PG1350" H 3050 850 50  0000 L CNN
F 2 "pg1350:PG1350" H 3200 1050 50  0001 C CNN
F 3 "" H 3200 1050 50  0001 C CNN
	1    3200 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 900  1000 700 
Wire Wire Line
	1400 900  1400 700 
Connection ~ 1400 700 
Wire Wire Line
	1400 700  1800 700 
Wire Wire Line
	1800 900  1800 700 
Connection ~ 1800 700 
Wire Wire Line
	2200 900  2200 700 
Wire Wire Line
	1800 700  2200 700 
Connection ~ 2200 700 
Wire Wire Line
	2200 700  2600 700 
Wire Wire Line
	2600 900  2600 700 
Connection ~ 2600 700 
Wire Wire Line
	2600 700  3000 700 
Wire Wire Line
	3000 900  3000 700 
Text GLabel 2250 1700 3    50   BiDi ~ 0
COL4
Text GLabel 1850 1700 3    50   BiDi ~ 0
COL3
Text GLabel 1450 1700 3    50   BiDi ~ 0
COL2
Text GLabel 1050 1700 3    50   BiDi ~ 0
COL1
Text GLabel 3050 1700 3    50   BiDi ~ 0
COL6
Text GLabel 2650 1700 3    50   BiDi ~ 0
COL5
$Comp
L pg1350:PG1350 K7
U 1 1 603B5446
P 1200 1550
F 0 "K7" H 1150 1700 50  0000 L CNN
F 1 "PG1350" H 1050 1500 50  0000 L CNN
F 2 "pg1350:PG1350" H 1200 1700 50  0001 C CNN
F 3 "" H 1200 1700 50  0001 C CNN
	1    1200 1550
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K8
U 1 1 603B544C
P 1600 1550
F 0 "K8" H 1550 1700 50  0000 L CNN
F 1 "PG1350" H 1450 1500 50  0000 L CNN
F 2 "pg1350:PG1350" H 1600 1700 50  0001 C CNN
F 3 "" H 1600 1700 50  0001 C CNN
	1    1600 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 1350 1400 1350
$Comp
L pg1350:PG1350 K9
U 1 1 603B5453
P 2000 1550
F 0 "K9" H 1950 1700 50  0000 L CNN
F 1 "PG1350" H 1850 1500 50  0000 L CNN
F 2 "pg1350:PG1350" H 2000 1700 50  0001 C CNN
F 3 "" H 2000 1700 50  0001 C CNN
	1    2000 1550
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K10
U 1 1 603B5459
P 2400 1550
F 0 "K10" H 2350 1700 50  0000 L CNN
F 1 "PG1350" H 2250 1500 50  0000 L CNN
F 2 "pg1350:PG1350" H 2400 1700 50  0001 C CNN
F 3 "" H 2400 1700 50  0001 C CNN
	1    2400 1550
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K11
U 1 1 603B545F
P 2800 1550
F 0 "K11" H 2750 1700 50  0000 L CNN
F 1 "PG1350" H 2650 1500 50  0000 L CNN
F 2 "pg1350:PG1350" H 2800 1700 50  0001 C CNN
F 3 "" H 2800 1700 50  0001 C CNN
	1    2800 1550
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K12
U 1 1 603B5465
P 3200 1550
F 0 "K12" H 3150 1700 50  0000 L CNN
F 1 "PG1350" H 3050 1500 50  0000 L CNN
F 2 "pg1350:PG1350" H 3200 1700 50  0001 C CNN
F 3 "" H 3200 1700 50  0001 C CNN
	1    3200 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 1550 1000 1350
Wire Wire Line
	1400 1550 1400 1350
Connection ~ 1400 1350
Wire Wire Line
	1400 1350 1800 1350
Wire Wire Line
	1800 1550 1800 1350
Connection ~ 1800 1350
Wire Wire Line
	2200 1550 2200 1350
Wire Wire Line
	1800 1350 2200 1350
Connection ~ 2200 1350
Wire Wire Line
	2200 1350 2600 1350
Wire Wire Line
	2600 1550 2600 1350
Connection ~ 2600 1350
Wire Wire Line
	2600 1350 3000 1350
Wire Wire Line
	3000 1550 3000 1350
Text GLabel 2250 2350 3    50   BiDi ~ 0
COL4
Text GLabel 1850 2350 3    50   BiDi ~ 0
COL3
Text GLabel 1450 2350 3    50   BiDi ~ 0
COL2
Text GLabel 1050 2350 3    50   BiDi ~ 0
COL1
Text GLabel 3050 2350 3    50   BiDi ~ 0
COL6
Text GLabel 2650 2350 3    50   BiDi ~ 0
COL5
$Comp
L pg1350:PG1350 K13
U 1 1 603B7E31
P 1200 2200
F 0 "K13" H 1150 2350 50  0000 L CNN
F 1 "PG1350" H 1050 2150 50  0000 L CNN
F 2 "pg1350:PG1350" H 1200 2350 50  0001 C CNN
F 3 "" H 1200 2350 50  0001 C CNN
	1    1200 2200
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K14
U 1 1 603B7E37
P 1600 2200
F 0 "K14" H 1550 2350 50  0000 L CNN
F 1 "PG1350" H 1450 2150 50  0000 L CNN
F 2 "pg1350:PG1350" H 1600 2350 50  0001 C CNN
F 3 "" H 1600 2350 50  0001 C CNN
	1    1600 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 2000 1400 2000
$Comp
L pg1350:PG1350 K15
U 1 1 603B7E3E
P 2000 2200
F 0 "K15" H 1950 2350 50  0000 L CNN
F 1 "PG1350" H 1850 2150 50  0000 L CNN
F 2 "pg1350:PG1350" H 2000 2350 50  0001 C CNN
F 3 "" H 2000 2350 50  0001 C CNN
	1    2000 2200
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K16
U 1 1 603B7E44
P 2400 2200
F 0 "K16" H 2350 2350 50  0000 L CNN
F 1 "PG1350" H 2250 2150 50  0000 L CNN
F 2 "pg1350:PG1350" H 2400 2350 50  0001 C CNN
F 3 "" H 2400 2350 50  0001 C CNN
	1    2400 2200
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K17
U 1 1 603B7E4A
P 2800 2200
F 0 "K17" H 2750 2350 50  0000 L CNN
F 1 "PG1350" H 2650 2150 50  0000 L CNN
F 2 "pg1350:PG1350" H 2800 2350 50  0001 C CNN
F 3 "" H 2800 2350 50  0001 C CNN
	1    2800 2200
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K18
U 1 1 603B7E50
P 3200 2200
F 0 "K18" H 3150 2350 50  0000 L CNN
F 1 "PG1350" H 3050 2150 50  0000 L CNN
F 2 "pg1350:PG1350" H 3200 2350 50  0001 C CNN
F 3 "" H 3200 2350 50  0001 C CNN
	1    3200 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 2200 1000 2000
Wire Wire Line
	1400 2200 1400 2000
Connection ~ 1400 2000
Wire Wire Line
	1400 2000 1800 2000
Wire Wire Line
	1800 2200 1800 2000
Connection ~ 1800 2000
Wire Wire Line
	2200 2200 2200 2000
Wire Wire Line
	1800 2000 2200 2000
Connection ~ 2200 2000
Wire Wire Line
	2200 2000 2600 2000
Wire Wire Line
	2600 2200 2600 2000
Connection ~ 2600 2000
Wire Wire Line
	2600 2000 3000 2000
Wire Wire Line
	3000 2200 3000 2000
Text GLabel 2250 3000 3    50   BiDi ~ 0
COL4
Text GLabel 1850 3000 3    50   BiDi ~ 0
COL3
Text GLabel 1450 3000 3    50   BiDi ~ 0
COL2
Text GLabel 1050 3000 3    50   BiDi ~ 0
COL1
Text GLabel 3050 3000 3    50   BiDi ~ 0
COL6
Text GLabel 2650 3000 3    50   BiDi ~ 0
COL5
$Comp
L pg1350:PG1350 K19
U 1 1 603BAF95
P 1200 2850
F 0 "K19" H 1150 3000 50  0000 L CNN
F 1 "PG1350" H 1050 2800 50  0000 L CNN
F 2 "pg1350:PG1350" H 1200 3000 50  0001 C CNN
F 3 "" H 1200 3000 50  0001 C CNN
	1    1200 2850
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K20
U 1 1 603BAF9B
P 1600 2850
F 0 "K20" H 1550 3000 50  0000 L CNN
F 1 "PG1350" H 1450 2800 50  0000 L CNN
F 2 "pg1350:PG1350" H 1600 3000 50  0001 C CNN
F 3 "" H 1600 3000 50  0001 C CNN
	1    1600 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 2650 1400 2650
$Comp
L pg1350:PG1350 K21
U 1 1 603BAFA2
P 2000 2850
F 0 "K21" H 1950 3000 50  0000 L CNN
F 1 "PG1350" H 1850 2800 50  0000 L CNN
F 2 "pg1350:PG1350" H 2000 3000 50  0001 C CNN
F 3 "" H 2000 3000 50  0001 C CNN
	1    2000 2850
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K22
U 1 1 603BAFA8
P 2400 2850
F 0 "K22" H 2350 3000 50  0000 L CNN
F 1 "PG1350" H 2250 2800 50  0000 L CNN
F 2 "pg1350:PG1350" H 2400 3000 50  0001 C CNN
F 3 "" H 2400 3000 50  0001 C CNN
	1    2400 2850
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K23
U 1 1 603BAFAE
P 2800 2850
F 0 "K23" H 2750 3000 50  0000 L CNN
F 1 "PG1350" H 2650 2800 50  0000 L CNN
F 2 "pg1350:PG1350" H 2800 3000 50  0001 C CNN
F 3 "" H 2800 3000 50  0001 C CNN
	1    2800 2850
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K24
U 1 1 603BAFB4
P 3200 2850
F 0 "K24" H 3150 3000 50  0000 L CNN
F 1 "PG1350" H 3050 2800 50  0000 L CNN
F 2 "pg1350:PG1350" H 3200 3000 50  0001 C CNN
F 3 "" H 3200 3000 50  0001 C CNN
	1    3200 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 2850 1000 2650
Wire Wire Line
	1400 2850 1400 2650
Connection ~ 1400 2650
Wire Wire Line
	1400 2650 1800 2650
Wire Wire Line
	1800 2850 1800 2650
Connection ~ 1800 2650
Wire Wire Line
	2200 2850 2200 2650
Wire Wire Line
	1800 2650 2200 2650
Connection ~ 2200 2650
Wire Wire Line
	2200 2650 2600 2650
Wire Wire Line
	2600 2850 2600 2650
Connection ~ 2600 2650
Wire Wire Line
	2600 2650 3000 2650
Wire Wire Line
	3000 2850 3000 2650
Text GLabel 2250 3650 3    50   BiDi ~ 0
COL4
Text GLabel 1850 3650 3    50   BiDi ~ 0
COL3
Text GLabel 1450 3650 3    50   BiDi ~ 0
COL2
Text GLabel 1050 3650 3    50   BiDi ~ 0
COL1
Text GLabel 3050 3650 3    50   BiDi ~ 0
COL6
Text GLabel 2650 3650 3    50   BiDi ~ 0
COL5
$Comp
L pg1350:PG1350 K25
U 1 1 603C19FA
P 1200 3500
F 0 "K25" H 1150 3650 50  0000 L CNN
F 1 "PG1350" H 1050 3450 50  0000 L CNN
F 2 "pg1350:PG1350" H 1200 3650 50  0001 C CNN
F 3 "" H 1200 3650 50  0001 C CNN
	1    1200 3500
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K26
U 1 1 603C1A00
P 1600 3500
F 0 "K26" H 1550 3650 50  0000 L CNN
F 1 "PG1350" H 1450 3450 50  0000 L CNN
F 2 "pg1350:PG1350" H 1600 3650 50  0001 C CNN
F 3 "" H 1600 3650 50  0001 C CNN
	1    1600 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 3300 1400 3300
$Comp
L pg1350:PG1350 K27
U 1 1 603C1A07
P 2000 3500
F 0 "K27" H 1950 3650 50  0000 L CNN
F 1 "PG1350" H 1850 3450 50  0000 L CNN
F 2 "pg1350:PG1350" H 2000 3650 50  0001 C CNN
F 3 "" H 2000 3650 50  0001 C CNN
	1    2000 3500
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K28
U 1 1 603C1A0D
P 2400 3500
F 0 "K28" H 2350 3650 50  0000 L CNN
F 1 "PG1350" H 2250 3450 50  0000 L CNN
F 2 "pg1350:PG1350" H 2400 3650 50  0001 C CNN
F 3 "" H 2400 3650 50  0001 C CNN
	1    2400 3500
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K29
U 1 1 603C1A13
P 2800 3500
F 0 "K29" H 2750 3650 50  0000 L CNN
F 1 "PG1350" H 2650 3450 50  0000 L CNN
F 2 "pg1350:PG1350" H 2800 3650 50  0001 C CNN
F 3 "" H 2800 3650 50  0001 C CNN
	1    2800 3500
	1    0    0    -1  
$EndComp
$Comp
L pg1350:PG1350 K30
U 1 1 603C1A19
P 3200 3500
F 0 "K30" H 3150 3650 50  0000 L CNN
F 1 "PG1350" H 3050 3450 50  0000 L CNN
F 2 "pg1350:PG1350" H 3200 3650 50  0001 C CNN
F 3 "" H 3200 3650 50  0001 C CNN
	1    3200 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 3500 1000 3300
Wire Wire Line
	1400 3500 1400 3300
Connection ~ 1400 3300
Wire Wire Line
	1400 3300 1800 3300
Wire Wire Line
	1800 3500 1800 3300
Connection ~ 1800 3300
Wire Wire Line
	2200 3500 2200 3300
Wire Wire Line
	1800 3300 2200 3300
Connection ~ 2200 3300
Wire Wire Line
	2200 3300 2600 3300
Wire Wire Line
	2600 3500 2600 3300
Connection ~ 2600 3300
Wire Wire Line
	2600 3300 3000 3300
Wire Wire Line
	3000 3500 3000 3300
$Comp
L Connector:USB_C_Receptacle_USB2.0 J1
U 1 1 603A274B
P 3000 5300
F 0 "J1" H 3107 6167 50  0000 C CNN
F 1 "USB_C_Receptacle_USB2.0" H 3107 6076 50  0000 C CNN
F 2 "Connector_USB:USB_C_Receptacle_HRO_TYPE-C-31-M-12" H 3150 5300 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 3150 5300 50  0001 C CNN
	1    3000 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R11
U 1 1 60403035
P 3750 4900
F 0 "R11" V 3750 5200 50  0000 C CNN
F 1 "5.1k" V 3750 4900 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3680 4900 50  0001 C CNN
F 3 "~" H 3750 4900 50  0001 C CNN
	1    3750 4900
	0    1    1    0   
$EndComp
$Comp
L Device:R R12
U 1 1 604188A6
P 3750 5000
F 0 "R12" V 3750 5300 50  0000 C CNN
F 1 "5.1k" V 3750 5000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3680 5000 50  0001 C CNN
F 3 "~" H 3750 5000 50  0001 C CNN
	1    3750 5000
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 6041F327
P 3900 4950
F 0 "#PWR0101" H 3900 4700 50  0001 C CNN
F 1 "GND" H 3900 4700 50  0000 C CNN
F 2 "" H 3900 4950 50  0001 C CNN
F 3 "" H 3900 4950 50  0001 C CNN
	1    3900 4950
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0116
U 1 1 6042B16E
P 4450 5850
F 0 "#PWR0116" H 4450 5600 50  0001 C CNN
F 1 "GND" H 4455 5677 50  0000 C CNN
F 2 "" H 4450 5850 50  0001 C CNN
F 3 "" H 4450 5850 50  0001 C CNN
	1    4450 5850
	1    0    0    -1  
$EndComp
NoConn ~ 3600 5800
NoConn ~ 3600 5900
$Comp
L power:GND #PWR0117
U 1 1 60437D30
P 3000 6200
F 0 "#PWR0117" H 3000 5950 50  0001 C CNN
F 1 "GND" H 3005 6027 50  0000 C CNN
F 2 "" H 3000 6200 50  0001 C CNN
F 3 "" H 3000 6200 50  0001 C CNN
	1    3000 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 4900 3900 4950
Connection ~ 3900 4950
Wire Wire Line
	3900 4950 3900 5000
Connection ~ 3600 4700
$Comp
L Power_Protection:SRV05-4 U6
U 1 1 603A89FD
P 4450 5350
F 0 "U6" V 3750 5350 50  0000 C CNN
F 1 "SRV05-4" V 3850 5350 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 5150 4900 50  0001 C CNN
F 3 "http://www.onsemi.com/pub/Collateral/SRV05-4-D.PDF" H 4450 5350 50  0001 C CNN
	1    4450 5350
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0115
U 1 1 60532A81
P 4450 4850
F 0 "#PWR0115" H 4450 4700 50  0001 C CNN
F 1 "VCC" H 4465 5023 50  0000 C CNN
F 2 "" H 4450 4850 50  0001 C CNN
F 3 "" H 4450 4850 50  0001 C CNN
	1    4450 4850
	1    0    0    -1  
$EndComp
Text GLabel 3600 5450 2    50   Input ~ 0
D+
Text GLabel 3600 5250 2    50   Input ~ 0
D-
Text GLabel 4950 5250 2    50   Input ~ 0
D+
Text GLabel 4950 5450 2    50   Input ~ 0
D-
Wire Wire Line
	3600 5400 3600 5500
Wire Wire Line
	3600 5200 3600 5300
$EndSCHEMATC
