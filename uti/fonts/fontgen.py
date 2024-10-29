#
# FontGen
# Author: Jared Sanson (jared@jared.geek.nz)
#
# Requires Python Imaging Library (PIL)
#
# To add a new font, create a new FONT dictionary and tweak the parameters
# until the output .png looks correct.
# Monospaced fonts work best, but some variable-width ones work well too.
#
# Once the png file looks good, you can simply include the .h file in your
# project and use it. (how you use it is up to you)
#
# original website: https://jared.geek.nz/2014/01/custom-fonts-for-microcontrollers/
#
# Updated by Gylian.me
# font website: https://www.dafont.com/bitmap.php

from PIL import Image, ImageFont, ImageDraw
import os.path

def savefont(FONT):
    #TODO: Support variable-width character fonts -- NO, keep monospaced for simplicity for now

    FONT_FILE = FONT['fname']
    FONT_SIZE = FONT['size']
    FONT_Y_OFFSET = FONT.get('yoff', 0)

    CHAR_WIDTH = FONT.get('w', 5)
    CHAR_HEIGHT = FONT.get('h', 8)
    THRESHOLD = FONT.get('thr', 155)

    FONT_BEGIN = ' '
    FONT_END = '~'
    if FONT.get('up', False):
        FONTSTR = ''.join(chr(x).upper() for x in range(ord(FONT_BEGIN), ord(FONT_END)+1))
    else:
        FONTSTR = ''.join(chr(x) for x in range(ord(FONT_BEGIN), ord(FONT_END)+1))

    OUTPUT_NAME = os.path.splitext(FONT_FILE)[0] + '_font'
    OUTPUT_PNG = OUTPUT_NAME + '.png'
    OUTPUT_H = OUTPUT_NAME + '.h'
    OUTPUT_BIN = OUTPUT_NAME + '.bin'

    GLYPH_WIDTH = CHAR_WIDTH + 1

    WIDTH = GLYPH_WIDTH * len(FONTSTR)
    HEIGHT = CHAR_HEIGHT

    #check if the png file exists
    if os.path.exists(OUTPUT_PNG):
        print('Font file already exists: %s' % OUTPUT_PNG)
    else:
        print('Generating font: %s' % OUTPUT_PNG)

        img = Image.new("RGBA", (WIDTH, HEIGHT), (255,255,255))
        #fnt = ImageFont.load_default()
        fnt = ImageFont.truetype(FONT_FILE.lower(), FONT_SIZE)

        drw = ImageDraw.Draw(img)
        #drw.fontmode = 1

        for i in range(len(FONTSTR)):
            #center the character in the box
            bbox = fnt.getbbox(FONTSTR[i])
            char_width = bbox[2] - bbox[0]
            char_height = bbox[3] - bbox[1]
            x = i*GLYPH_WIDTH + int((CHAR_WIDTH - char_width) // 2)
            y = 0# (CHAR_HEIGHT - char_height) // 2
            drw.text((x, y+FONT_Y_OFFSET), FONTSTR[i], (0,0,0), font=fnt)

        #convert the image to monochrome before saving
        img = img.convert('L').point(lambda x: 0 if x < THRESHOLD else 255)
        img.save(OUTPUT_PNG)

    img = Image.open(OUTPUT_PNG)
    img = img.convert('L')

    #### Convert to C-header format
    f = open(OUTPUT_H, 'w')
    fbin = open(OUTPUT_BIN, 'wb')
    num_chars = len(FONTSTR)
    f.write('const uint8_t font[] = {\n')

    f.write('\t%d, %d, %d, %d, %d, //height, width, spacing, begin, end\n' % (CHAR_HEIGHT, CHAR_WIDTH, 1, ord(FONT_BEGIN), ord(FONT_END)))
    fbin.write(bytes([CHAR_HEIGHT, CHAR_WIDTH, 1, ord(FONT_BEGIN), ord(FONT_END)]))

    for i in range(num_chars):
        ints = []
        for j in range(CHAR_WIDTH):
            x = i*GLYPH_WIDTH + j
            val = 0
            for y in range(CHAR_HEIGHT):
                rgb = img.getpixel((x,y))
                val = (val >> 1) | (0x80 if rgb == 0 else 0)
                if (y % 8) == 7:
                    ints.append('0x%.2x' % (val))
                    fbin.write(bytes([val]))
                    val = 0
            if (CHAR_HEIGHT % 8) != 0:
                ints.append('0x%.2x' % (val))
                fbin.write(bytes([val]))
                
        c = FONTSTR[i]
        if c == '\\': c = '"\\"' # bugfix
        f.write('\t%s, // %s\n' % (','.join(ints), c))


    f.write('};\n\n')

    f.write('#define FONT_NAME "%s"\n' % OUTPUT_NAME)

    f.close()
    fbin.close()


all_fonts = [
    # MONOSPACE:
    {'fname': r'5x5.ttf', 'size': 10, 'yoff':-2, 'w': 6, 'h': 8},
    {'fname': r'7linedigital.ttf', 'size': 8, 'yoff':0, 'w': 4, 'h': 8},  # 7-seg. NOTE: can't display certain letters like 'M'
    {'fname': r'BMplain.ttf', 'size': 7, 'yoff':0, 'w': 6, 'h': 8},
    {'fname': r'BMSPA.ttf', 'size': 9, 'yoff':0, 'w': 8, 'h': 8, 'up': True}, #NOTE: add .upper() because lower characters are broken
    {'fname': r'bubblesstandard.ttf', 'size': 15, 'yoff':-1, 'w': 7, 'h': 8},
    {'fname': r'formplex12.ttf', 'size': 11, 'yoff':0, 'w': 8, 'h': 8},
    {'fname': r'HUNTER.ttf', 'size': 9, 'yoff':-1, 'w': 8, 'h': 8},
    {'fname': r'm38.ttf', 'size': 8, 'yoff':-0, 'w': 8, 'h': 8},
    {'fname': r'pzim3x5.ttf', 'size': 9, 'yoff':1, 'w': 3, 'h': 8},
    {'fname': r'sloth.ttf', 'size': 15, 'yoff':-2, 'w': 6, 'h': 8},

    # VARIABLE-WIDTH:
    {'fname': r'acme_5_outlines.ttf', 'size': 8, 'yoff':-5, 'w': 6, 'h': 8},
    {'fname': r'aztech.ttf', 'size': 16, 'yoff':-1, 'w': 6, 'h': 8},
    {'fname': r'Blokus.otf', 'size': 9, 'yoff':-2, 'w': 6, 'h': 8, 'thr': 215},
    {'fname': r'Commo-Monospaced.otf', 'size': 8, 'yoff':-6, 'w': 8, 'h': 8},
    {'fname': r'crackers.ttf', 'size': 21, 'yoff':-4, 'w': 6, 'h': 8},
    {'fname': r'haiku.ttf', 'size': 11, 'yoff':-2, 'w': 6, 'h': 8, 'thr': 150},
    {'fname': r'HISKYF21.ttf', 'size': 9, 'yoff':0, 'w': 6, 'h': 8},
    {'fname': r'homespun.ttf', 'size': 9, 'yoff':-1, 'w': 7, 'h': 8},  # Non-monospaced
    {'fname': r'kiwisoda.ttf', 'size': 16, 'yoff':0, 'w': 8, 'h': 16},
    {'fname': r'Minimum.ttf', 'size': 16, 'yoff':-8, 'w': 6, 'h': 8},
    {'fname': r'Minimum+1.ttf', 'size': 16, 'yoff':-8, 'w': 7, 'h': 8},
    {'fname': r'renew.ttf', 'size': 8, 'yoff':-2, 'w': 7, 'h': 8},
    {'fname': r'SUPERDIG.ttf', 'size': 9, 'yoff':-1, 'w': 6, 'h': 8}, # Missing some symbols
    {'fname': r'tama_mini02.TTF', 'size': 11, 'yoff': -2, 'w': 5, 'h': 8},
    {'fname': r'zxpix.ttf', 'size': 10, 'yoff':-2, 'w': 6, 'h': 8},
]

for font in all_fonts:
    print(f"Saving: {font['fname']}")
    savefont(font)