const uint8_t font[] = {
	8, 6, 1, 32, 126, //height, width, spacing, begin, end
	0x00,0x00,0x00,0x00,0x00,0x00, //  
	0x00,0x2e,0x00,0x00,0x00,0x00, // !
	0x00,0x02,0x00,0x02,0x00,0x00, // "
	0x00,0x0a,0x1e,0x0a,0x1e,0x00, // #
	0x00,0x0e,0x2a,0x6b,0x2a,0x3a, // $
	0x00,0x06,0x06,0x26,0x18,0x06, // %
	0x38,0x3e,0x2a,0x2a,0x28,0x38, // &
	0x18,0x02,0x00,0x00,0x00,0x00, // '
	0x00,0x3e,0x22,0x00,0x00,0x00, // (
	0x00,0x22,0x3e,0x00,0x00,0x00, // )
	0x00,0x0e,0x0e,0x0e,0x04,0x00, // *
	0x00,0x08,0x1c,0x08,0x00,0x00, // +
	0x00,0x20,0x00,0x00,0x00,0x00, // ,
	0x00,0x08,0x08,0x08,0x00,0x00, // -
	0x00,0x20,0x00,0x00,0x00,0x00, // .
	0x00,0x20,0x18,0x06,0x00,0x00, // /
	0x00,0x3e,0x22,0x2a,0x22,0x3e, // 0
	0x02,0x3e,0x00,0x00,0x00,0x00, // 1
	0x00,0x38,0x28,0x2a,0x2a,0x2e, // 2
	0x00,0x22,0x2a,0x2e,0x38,0x00, // 3
	0x00,0x0e,0x08,0x08,0x3e,0x08, // 4
	0x00,0x2e,0x2a,0x2a,0x28,0x38, // 5
	0x00,0x3e,0x2a,0x2a,0x28,0x38, // 6
	0x00,0x06,0x02,0x02,0x0a,0x3e, // 7
	0x00,0x38,0x2e,0x2a,0x2e,0x38, // 8
	0x00,0x0e,0x0a,0x2a,0x2a,0x3e, // 9
	0x00,0x28,0x00,0x00,0x00,0x00, // :
	0x00,0x28,0x00,0x00,0x00,0x00, // ;
	0x00,0x08,0x14,0x22,0x00,0x00, // <
	0x00,0x14,0x14,0x14,0x14,0x00, // =
	0x00,0x22,0x14,0x08,0x00,0x00, // >
	0x00,0x06,0x02,0x2a,0x0a,0x06, // ?
	0x00,0x3e,0x02,0x3a,0x2a,0x0a, // @
	0x22,0x3e,0x02,0x0a,0x0a,0x3e, // A
	0x00,0x3e,0x22,0x2a,0x2e,0x38, // B
	0x00,0x3e,0x22,0x22,0x20,0x30, // C
	0x00,0x3e,0x22,0x22,0x22,0x3c, // D
	0x00,0x3e,0x2a,0x22,0x20,0x30, // E
	0x00,0x3e,0x0a,0x0a,0x06,0x02, // F
	0x00,0x3e,0x22,0x2a,0x28,0x38, // G
	0x00,0x3e,0x08,0x08,0x08,0x3e, // H
	0x00,0x22,0x3e,0x22,0x00,0x00, // I
	0x00,0x30,0x20,0x20,0x22,0x3e, // J
	0x00,0x3e,0x08,0x08,0x0e,0x38, // K
	0x00,0x3e,0x20,0x20,0x20,0x30, // L
	0x00,0x3e,0x02,0x3e,0x20,0x3e, // M
	0x3e,0x3e,0x02,0x3e,0x20,0x3e, // N
	0x00,0x3e,0x22,0x22,0x22,0x3e, // O
	0x00,0x3e,0x02,0x0a,0x0a,0x0e, // P
	0x00,0x3e,0x22,0x22,0x22,0x3e, // Q
	0x00,0x3e,0x02,0x0a,0x0e,0x38, // R
	0x00,0x0e,0x0a,0x2a,0x2a,0x3a, // S
	0x00,0x06,0x02,0x3e,0x02,0x06, // T
	0x00,0x3e,0x20,0x20,0x20,0x3e, // U
	0x00,0x3e,0x20,0x20,0x3e,0x00, // V
	0x00,0x3e,0x20,0x3e,0x02,0x3e, // W
	0x3e,0x3a,0x0e,0x08,0x0e,0x3a, // X
	0x00,0x0e,0x08,0x08,0x28,0x3e, // Y
	0x00,0x3a,0x2a,0x2a,0x0a,0x0e, // Z
	0x00,0x3e,0x22,0x00,0x00,0x00, // [
	0x00,0x02,0x0c,0x30,0x00,0x00, // "\"
	0x22,0x3e,0x00,0x00,0x00,0x00, // ]
	0x00,0x01,0x00,0x01,0x00,0x00, // ^
	0x00,0x20,0x20,0x20,0x00,0x00, // _
	0x00,0x01,0x00,0x00,0x00,0x00, // `
	0x00,0x38,0x28,0x28,0x08,0x38, // a
	0x00,0x3e,0x20,0x28,0x28,0x38, // b
	0x00,0x38,0x28,0x28,0x20,0x30, // c
	0x00,0x38,0x28,0x28,0x20,0x3e, // d
	0x00,0xf8,0x88,0x28,0x28,0x38, // e
	0x00,0xf8,0x28,0x28,0x18,0x08, // f
	0x00,0x38,0x28,0xa8,0x88,0xf8, // g
	0x00,0x3c,0x08,0x08,0x08,0x38, // h
	0x00,0x3a,0x00,0x00,0x00,0x00, // i
	0x80,0xfa,0x00,0x00,0x00,0x00, // j
	0x00,0x3e,0x08,0x08,0x38,0x2c, // k
	0x00,0x3e,0x20,0x00,0x00,0x00, // l
	0x00,0x38,0x08,0x38,0x20,0x38, // m
	0x38,0x38,0x08,0x38,0x20,0x38, // n
	0x00,0x38,0x28,0x28,0x28,0x38, // o
	0x00,0xf8,0x08,0x28,0x28,0x38, // p
	0x00,0x38,0x28,0x28,0x08,0xf8, // q
	0x00,0x38,0x08,0x08,0x18,0x08, // r
	0x00,0x38,0x28,0xa8,0xa8,0xe8, // s
	0x00,0x3e,0x28,0x28,0x20,0x30, // t
	0x00,0x38,0x20,0x20,0x20,0x38, // u
	0x00,0x38,0x20,0x38,0x00,0x00, // v
	0x00,0x38,0x20,0x38,0x08,0x38, // w
	0x38,0x28,0x38,0x10,0x38,0x28, // x
	0x00,0x38,0x20,0xa0,0xa0,0xf8, // y
	0x00,0xe8,0xa8,0xa8,0x28,0x38, // z
	0x08,0x3e,0x22,0x00,0x00,0x00, // {
	0x00,0x3f,0x00,0x00,0x00,0x00, // |
	0x22,0x3e,0x08,0x00,0x00,0x00, // }
	0x00,0x00,0x00,0x00,0x00,0x00, // ~
};

#define FONT_NAME "aztech_font"
