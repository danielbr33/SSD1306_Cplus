#include "Buffer.h"
#include "bitoperations.h"
#include "string.h"

void Buffer::fill(Color color) {
	for (uint8_t i = 0; i < this->buffer_height; i++)
		for (uint8_t j = 0; j < this->buffer_width / BUFFOR_PART_WIDTH; j++) {
			if (color == Black)
				table[j][i] = BUFFOR_FILLED;
			else
				table[j][i] = 0;
		}
}

void Buffer::print() {
	for (uint8_t i = 0; i < this->buffer_height; i++) {
		for (uint8_t k = 0; k < (this->buffer_width/BUFFOR_PART_WIDTH); k++) {
			for (uint8_t j = 8; j > 0; j--) {
				if (getBit(table[k][i], j - 1) == 0)
					cout << "-";
				else
					cout << "+";
			}
		}
		cout << endl;
	}
}

Buffer::Buffer(uint8_t buffer_width, uint8_t buffer_height){
	this->buffer_height = buffer_height;
	this->buffer_width = buffer_width;
	table = new uint8_t* [this->buffer_width/BUFFOR_PART_WIDTH];
	for (uint8_t i = 0; i < (this->buffer_width / BUFFOR_PART_WIDTH); i++)
		table[i] = new uint8_t[this->buffer_height];
	for (uint8_t j=0; j < (this->buffer_width/BUFFOR_PART_WIDTH); j++)
		for (uint8_t i = 0; i < this->buffer_height; i++)
		table[j][i] = 0;
	font6x8_ready = 0;
	font7x10_ready = 0;
	font11x18_ready = 0;
}

Buffer::~Buffer(){
	delete[] table;
}

void Buffer::addLetter(uint8_t letter, Font font, Color color, uint8_t coord_X, uint8_t coord_Y) {
	createFont(font);
	if (font == Font6x8)
		Actual_Font = Font_6x8;
	else if (font == Font7x10)
		Actual_Font = Font_7x10;
	else if (font == Font11x18)
		Actual_Font = Font_11x18;

	uint8_t number_of_collumn = (uint8_t)(coord_X/8);
	uint8_t offset = coord_X % 8;
	uint8_t state;
	for (uint8_t i = 0; i < Actual_Font->getHeight(); i++) {
		if (coord_Y + i >= this->buffer_height)
			break;

		if (color == Black) {
			if (number_of_collumn < ((uint8_t)this->buffer_width / 8)) {
				table[number_of_collumn][i + coord_Y] |= (Actual_Font->getLetter(letter)[i] >> (BUFFOR_PART_WIDTH + offset));
			}
			if ((offset != 0) && (number_of_collumn + 1) < ((uint8_t)buffer_width / 8)) {
				table[number_of_collumn + 1][i + coord_Y] |= (Actual_Font->getLetter(letter)[i] >> (offset));
			}
		}

		else if (color == White) {
			if (number_of_collumn < ((uint8_t)this->buffer_width / 8)) {
				if (table[number_of_collumn][i + coord_Y] == 0)
					table[number_of_collumn][i + coord_Y] = ~0;
				table[number_of_collumn][i + coord_Y] &= ~(Actual_Font->getLetter(letter)[i] >> (BUFFOR_PART_WIDTH + offset));
			}
			if ((offset != 0) && (number_of_collumn + 1) < ((uint8_t)buffer_width / 8)) {
				if (table[number_of_collumn+1][i + coord_Y] == 0)
					table[number_of_collumn+1][i + coord_Y] = ~0;
				table[number_of_collumn + 1][i + coord_Y] &= ~(Actual_Font->getLetter(letter)[i] >> (offset));
			}
		}
	}
}

void Buffer::addText(char* text, Font font, Color color, uint8_t coord_X, uint8_t coord_Y) {
	createFont(font);
	if (font == Font6x8)
		Actual_Font = Font_6x8;
	else if (font == Font7x10)
		Actual_Font = Font_7x10;
	else if (font == Font11x18)
		Actual_Font = Font_11x18;
	for (uint8_t i = 0; i < strlen((char*)text); i++) {
		uint8_t current_X = coord_X + i * Actual_Font->getWidth();
		addLetter(text[i], font, color, current_X, coord_Y);
	}
}

void Buffer::createFont(Font font) {
	if (font == Font6x8 && font6x8_ready == 0) {
		Font_6x8 = new Fonts();
		Font_6x8->createFont6x8();
		font6x8_ready = 1;
	}
	else if(font == Font7x10 && font7x10_ready == 0){
		Font_7x10 = new Fonts();
		Font_7x10->createFont7x10();
		font7x10_ready = 1;
	}
	else if (font == Font11x18 && font11x18_ready == 0) {
		Font_11x18 = new Fonts();
		Font_11x18->createFont11x18();
		font11x18_ready = 1;
	}
}
