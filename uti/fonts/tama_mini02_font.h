const uint8_t font[] = {
	8, 5, 1, 32, 126, //height, width, spacing, begin, end
	0x00,0x00,0x00,0x00,0x00, //  
	0x00,0x2f,0x00,0x00,0x00, // !
	0x03,0x00,0x03,0x00,0x00, // "
	0x3e,0x14,0x3e,0x14,0x00, // #
	0x2e,0x6a,0x2b,0x3a,0x00, // $
	0x12,0x08,0x24,0x32,0x00, // %
	0x1c,0x17,0x15,0x34,0x00, // &
	0x00,0x03,0x00,0x00,0x00, // '
	0x00,0x1e,0x21,0x00,0x00, // (
	0x00,0x21,0x1f,0x00,0x00, // )
	0x08,0x1c,0x08,0x22,0x00, // *
	0x08,0x1c,0x08,0x00,0x00, // +
	0x00,0x40,0x20,0x00,0x00, // ,
	0x00,0x08,0x08,0x00,0x00, // -
	0x00,0x20,0x00,0x00,0x00, // .
	0x10,0x08,0x04,0x02,0x00, // /
	0x3f,0x21,0x21,0x3f,0x00, // 0
	0x00,0x01,0x3f,0x00,0x00, // 1
	0x3d,0x25,0x25,0x27,0x00, // 2
	0x25,0x25,0x25,0x3f,0x00, // 3
	0x07,0x04,0x04,0x3f,0x00, // 4
	0x27,0x25,0x25,0x3d,0x00, // 5
	0x3f,0x25,0x25,0x3d,0x00, // 6
	0x01,0x39,0x05,0x03,0x00, // 7
	0x3f,0x25,0x25,0x3f,0x00, // 8
	0x27,0x25,0x25,0x3f,0x00, // 9
	0x00,0x28,0x00,0x00,0x00, // :
	0x00,0x40,0x28,0x00,0x00, // ;
	0x04,0x0a,0x11,0x00,0x00, // <
	0x00,0x14,0x14,0x00,0x00, // =
	0x11,0x0a,0x04,0x00,0x00, // >
	0x01,0x2d,0x05,0x07,0x00, // ?
	0x21,0x3d,0x25,0x1f,0x00, // @
	0x3f,0x09,0x09,0x3f,0x00, // A
	0x3f,0x25,0x27,0x3c,0x00, // B
	0x3f,0x21,0x21,0x21,0x00, // C
	0x3f,0x21,0x21,0x1f,0x00, // D
	0x3f,0x25,0x25,0x25,0x00, // E
	0x3f,0x05,0x05,0x05,0x00, // F
	0x3f,0x21,0x25,0x3d,0x00, // G
	0x3f,0x04,0x04,0x3f,0x00, // H
	0x21,0x3f,0x21,0x00,0x00, // I
	0x38,0x20,0x21,0x3f,0x00, // J
	0x3f,0x04,0x04,0x3f,0x00, // K
	0x3f,0x20,0x20,0x20,0x00, // L
	0x01,0x3f,0x01,0x3f,0x00, // M
	0x3f,0x02,0x04,0x3f,0x00, // N
	0x3f,0x21,0x21,0x3f,0x00, // O
	0x3f,0x09,0x09,0x0f,0x00, // P
	0x3f,0x21,0x31,0x3f,0x00, // Q
	0x3f,0x09,0x39,0x2f,0x00, // R
	0x27,0x25,0x25,0x3d,0x00, // S
	0x01,0x01,0x3f,0x01,0x00, // T
	0x3f,0x20,0x20,0x3f,0x00, // U
	0x0f,0x10,0x30,0x0f,0x00, // V
	0x20,0x3f,0x20,0x3f,0x00, // W
	0x3b,0x04,0x04,0x3f,0x00, // X
	0x0f,0x08,0x38,0x0f,0x00, // Y
	0x31,0x29,0x25,0x23,0x00, // Z
	0x00,0x3f,0x21,0x00,0x00, // [
	0x10,0x08,0x04,0x02,0x00, // "\"
	0x00,0x21,0x3f,0x00,0x00, // ]
	0x02,0x01,0x01,0x02,0x00, // ^
	0x00,0x20,0x20,0x00,0x00, // _
	0x00,0x01,0x02,0x00,0x00, // `
	0x38,0x24,0x24,0x3c,0x00, // a
	0x3f,0x24,0x24,0x3c,0x00, // b
	0x3c,0x24,0x24,0x24,0x00, // c
	0x3c,0x24,0x24,0x3f,0x00, // d
	0x3c,0x2c,0x2c,0x2c,0x00, // e
	0x04,0x3f,0x05,0x00,0x00, // f
	0xbc,0xa4,0xa4,0xfc,0x00, // g
	0x3f,0x04,0x04,0x3c,0x00, // h
	0x00,0x3d,0x00,0x00,0x00, // i
	0x00,0x80,0xfd,0x00,0x00, // j
	0x3f,0x08,0x08,0x2c,0x00, // k
	0x00,0x3f,0x00,0x00,0x00, // l
	0x04,0x3c,0x04,0x3c,0x3c, // m
	0x3c,0x04,0x04,0x3c,0x00, // n
	0x3c,0x24,0x24,0x3c,0x00, // o
	0xfc,0x24,0x24,0x3c,0x00, // p
	0x3c,0x24,0x24,0xfc,0x00, // q
	0x3c,0x08,0x04,0x00,0x00, // r
	0x2c,0x2c,0x2c,0x3c,0x00, // s
	0x04,0x3f,0x24,0x00,0x00, // t
	0x3c,0x20,0x20,0x3c,0x00, // u
	0x0c,0x10,0x30,0x0c,0x00, // v
	0x20,0x3c,0x20,0x3c,0x00, // w
	0x34,0x08,0x08,0x3c,0x00, // x
	0xbc,0xa0,0xa0,0xfc,0x00, // y
	0x24,0x34,0x2c,0x24,0x00, // z
	0x04,0x3f,0x21,0x00,0x00, // {
	0x00,0x3f,0x00,0x00,0x00, // |
	0x21,0x3f,0x04,0x00,0x00, // }
	0x01,0x02,0x02,0x01,0x00, // ~
};

#define FONT_NAME "tama_mini02_font"
