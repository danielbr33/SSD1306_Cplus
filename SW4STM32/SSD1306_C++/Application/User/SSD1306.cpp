/*
 * SSD1306.cpp
 *
 *  Created on: 22.03.2020
 *      Author: danie
 */

#include "SSD1306.h"

//#if defined(SSD1306_USE_I2C)
//void SSD1306::ssd1306_Reset(void) {
	/* for I2C - do nothing */
//}
// Send a byte to the command register
//void SSD1306::ssd1306_WriteCommand(uint8_t byte) {
//	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
//}
// Send data
//void SSD1306::ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
//	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
//}

//#elif defined(SSD1306_USE_SPI)
void SSD1306::ssd1306_Reset(void) {
	// CS = High (not selected)
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET);

	// Reset the OLED
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}
// Send a byte to the command register
void SSD1306::ssd1306_WriteCommand() {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_RESET); // command
	HAL_SPI_Transmit_DMA(&SSD1306_SPI_PORT, lineCommands, 3);
}
// Send data
void SSD1306::ssd1306_WriteData() {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_SET); // data
	HAL_SPI_Transmit_DMA(&SSD1306_SPI_PORT, &SSD1306_Buffer[SSD1306_WIDTH*counter], SSD1306_WIDTH);
}

//#else
//#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro"
//#endif

void SSD1306::SPI_Interrupt(){
	/*
	if (status==2);
	else if (status==0){
        lineCommands[0]=0xB0 + counter;
        lineCommands[1]=0x00;
        lineCommands[2]=0x10;
        status=1;
		ssd1306_WriteCommand();
	}
	else{
		status=0;
		counter+=1;
		if (counter==8)
			counter=0;
		ssd1306_WriteData();
	}*/
}

void SSD1306::loop(){
	for (int i=0; i<8 ; i++){
        lineCommands[0]=0xB0 + counter;
        lineCommands[1]=0x00;
        lineCommands[2]=0x10;
        status=1;
		ssd1306_WriteCommand();
	    HAL_Delay(100);
		status=0;
		counter+=1;
		if (counter==8)
			counter=0;
		ssd1306_WriteData();
	    HAL_Delay(100);
	}
	HAL_Delay(1000);
}

void SSD1306::ssd1306_Init(void) {
	// Reset OLED
	ssd1306_Reset();
    // Wait for the screen to boot
    HAL_Delay(100);

    // Init OLED
    initCommands[0]=0xAE; //display off

    initCommands[1]=0x20; //Set Memory Addressing Mode
    initCommands[2]=0x00; // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    initCommands[3]=0xB0; //Set Page Start Address for Page Addressing Mode,0-7

	#ifdef SSD1306_MIRROR_VERT
		initCommands[4]=0xC0; // Mirror vertically
	#else
		initCommands[4]=0xC8; //Set COM Output Scan Direction
	#endif

    initCommands[5]=0x00; //---set low column address
    initCommands[6]=0x10; //---set high column address

    initCommands[7]=0x40; //--set start line address - CHECK

    initCommands[8]=0x81; //--set contrast control register - CHECK
    initCommands[9]=0xFF;

	#ifdef SSD1306_MIRROR_HORIZ
		initCommands[10]=0xA0; // Mirror horizontally
	#else
		initCommands[10]=0xA1; //--set segment re-map 0 to 127 - CHECK
	#endif

	#ifdef SSD1306_INVERSE_COLOR
		initCommands[11]=0xA7; //--set inverse color
	#else
		initCommands[11]=0xA6; //--set normal color
	#endif

    initCommands[12]=0xA8; //--set multiplex ratio(1 to 64) - CHECK
	#if (SSD1306_HEIGHT == 32)
		initCommands[13]=0x1F; //
	#elif (SSD1306_HEIGHT == 64)
		initCommands[13]=0x3F; //
	#else
	#error "Only 32 or 64 lines of height are supported!"
	#endif

    initCommands[14]=0xA4; //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    initCommands[15]=0xD3; //-set display offset - CHECK
    initCommands[16]=0x00; //-not offset

    initCommands[17]=0xD5; //--set display clock divide ratio/oscillator frequency
    initCommands[18]=0xF0; //--set divide ratio

    initCommands[19]=0xD9; //--set pre-charge period
    initCommands[20]=0x22; //

    initCommands[21]=0xDA; //--set com pins hardware configuration - CHECK
	#if (SSD1306_HEIGHT == 32)
		initCommands[22]=0x02;
	#elif (SSD1306_HEIGHT == 64)
		initCommands[22]=0x12;
	#else
	#error "Only 32 or 64 lines of height are supported!"
	#endif

    initCommands[23]=0xDB; //--set vcomh
    initCommands[24]=0x20; //0x20,0.77xVcc

    initCommands[25]=0x8D; //--set DC-DC enable
    initCommands[26]=0x14; //
    initCommands[27]=0xAF; //--turn on SSD1306 panel
    status=2;

    currentX = 0;
    currentY = 0;

    initialized = 1;
//-------------------------------------------------------------------------------------
    ssd1306_Fill(White);
    HAL_SPI_Transmit_DMA(&SSD1306_SPI_PORT, initCommands, 28);
    ssd1306_Fill(White);
    loop();
    status=0;
    ssd1306_Fill(Black);
    loop();
    SPI_Interrupt();
//-------------------------------------------------------------------------------------
    // Flush buffer to screen

    // Set default values for screen object

}

void SSD1306::process(){
	//components to display


	HAL_Delay(5);
}

// Fill the whole screen with the given color
void SSD1306::ssd1306_Fill(SSD1306_COLOR color) {
    /* Set memory */
    uint32_t i;

    for(i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void SSD1306::ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }

    // Check if pixel should be inverted
    if(inverted) {
        color = (SSD1306_COLOR)!color;
    }

    // Draw in the right color
    if(color == White) {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char SSD1306::ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color) {
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (currentX + Font.FontWidth) ||
        SSD1306_HEIGHT < (currentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++) {
            if((b << j) & 0x8000)  {
                ssd1306_DrawPixel(currentX + j, (currentY + i), (SSD1306_COLOR) color);
            } else {
                ssd1306_DrawPixel(currentX + j, (currentY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    currentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

// Write full string to screenbuffer
char SSD1306::ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color) {
    // Write until null-byte
    while (*str) {
        if (ssd1306_WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }

        // Next char
        str++;
    }

    // Everything ok
    return *str;
}

// Position the cursor
void SSD1306::ssd1306_SetCursor(uint8_t x, uint8_t y) {
    currentX = x;
    currentY = y;
}

SSD1306::SSD1306() {
	// TODO Auto-generated constructor stub
	counter=7;
}

SSD1306::~SSD1306() {
	// TODO Auto-generated destructor stub
}
