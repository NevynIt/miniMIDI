#include "midi_frequencies.h"
#include <cmath>

const fp_int midi_frequencies[128] = {
    (fp_int)(8.1757989156 * FP1),  // MIDI Note 0
    (fp_int)(8.6619572180 * FP1),  // MIDI Note 1
    (fp_int)(9.1770239974 * FP1),  // MIDI Note 2
    (fp_int)(9.7227182413 * FP1),  // MIDI Note 3
    (fp_int)(10.3008611535 * FP1), // MIDI Note 4
    (fp_int)(10.9133822323 * FP1), // MIDI Note 5
    (fp_int)(11.5623257097 * FP1), // MIDI Note 6
    (fp_int)(12.2498573744 * FP1), // MIDI Note 7
    (fp_int)(12.9782717994 * FP1), // MIDI Note 8
    (fp_int)(13.75 * FP1),         // MIDI Note 9
    (fp_int)(14.5676175474 * FP1), // MIDI Note 10
    (fp_int)(15.4338531643 * FP1), // MIDI Note 11
    (fp_int)(16.3515978313 * FP1), // MIDI Note 12
    (fp_int)(17.3239144361 * FP1), // MIDI Note 13
    (fp_int)(18.3540479948 * FP1), // MIDI Note 14
    (fp_int)(19.4454364826 * FP1), // MIDI Note 15
    (fp_int)(20.6017223071 * FP1), // MIDI Note 16
    (fp_int)(21.8267644646 * FP1), // MIDI Note 17
    (fp_int)(23.1246514195 * FP1), // MIDI Note 18
    (fp_int)(24.4997147489 * FP1), // MIDI Note 19
    (fp_int)(25.9565435987 * FP1), // MIDI Note 20
    (fp_int)(27.5 * FP1),          // MIDI Note 21
    (fp_int)(29.1352350949 * FP1), // MIDI Note 22
    (fp_int)(30.8677063285 * FP1), // MIDI Note 23
    (fp_int)(32.7031956626 * FP1), // MIDI Note 24
    (fp_int)(34.6478288721 * FP1), // MIDI Note 25
    (fp_int)(36.7080959897 * FP1), // MIDI Note 26
    (fp_int)(38.8908729653 * FP1), // MIDI Note 27
    (fp_int)(41.2034446141 * FP1), // MIDI Note 28
    (fp_int)(43.6535289291 * FP1), // MIDI Note 29
    (fp_int)(46.2493028390 * FP1), // MIDI Note 30
    (fp_int)(48.9994294977 * FP1), // MIDI Note 31
    (fp_int)(51.9130871975 * FP1), // MIDI Note 32
    (fp_int)(55.0 * FP1),          // MIDI Note 33
    (fp_int)(58.2704701898 * FP1), // MIDI Note 34
    (fp_int)(61.7354126570 * FP1), // MIDI Note 35
    (fp_int)(65.4063913251 * FP1), // MIDI Note 36
    (fp_int)(69.2956577442 * FP1), // MIDI Note 37
    (fp_int)(73.4161919794 * FP1), // MIDI Note 38
    (fp_int)(77.7817459305 * FP1), // MIDI Note 39
    (fp_int)(82.4068892282 * FP1), // MIDI Note 40
    (fp_int)(87.3070578583 * FP1), // MIDI Note 41
    (fp_int)(92.4986056779 * FP1), // MIDI Note 42
    (fp_int)(97.9988589954 * FP1), // MIDI Note 43
    (fp_int)(103.8261743950 * FP1),// MIDI Note 44
    (fp_int)(110.0 * FP1),         // MIDI Note 45
    (fp_int)(116.5409403795 * FP1),// MIDI Note 46
    (fp_int)(123.4708253140 * FP1),// MIDI Note 47
    (fp_int)(130.8127826503 * FP1),// MIDI Note 48
    (fp_int)(138.5913154884 * FP1),// MIDI Note 49
    (fp_int)(146.8323839587 * FP1),// MIDI Note 50
    (fp_int)(155.5634918610 * FP1),// MIDI Note 51
    (fp_int)(164.8137784564 * FP1),// MIDI Note 52
    (fp_int)(174.6141157165 * FP1),// MIDI Note 53
    (fp_int)(184.9972113558 * FP1),// MIDI Note 54
    (fp_int)(195.9977179909 * FP1),// MIDI Note 55
    (fp_int)(207.6523487900 * FP1),// MIDI Note 56
    (fp_int)(220.0 * FP1),         // MIDI Note 57
    (fp_int)(233.0818807590 * FP1),// MIDI Note 58
    (fp_int)(246.9416506281 * FP1),// MIDI Note 59
    (fp_int)(261.6255653006 * FP1),// MIDI Note 60
    (fp_int)(277.1826309769 * FP1),// MIDI Note 61
    (fp_int)(293.6647679174 * FP1),// MIDI Note 62
    (fp_int)(311.1269837221 * FP1),// MIDI Note 63
    (fp_int)(329.6275569129 * FP1),// MIDI Note 64
    (fp_int)(349.2282314330 * FP1),// MIDI Note 65
    (fp_int)(369.9944227116 * FP1),// MIDI Note 66
    (fp_int)(391.9954359817 * FP1),// MIDI Note 67
    (fp_int)(415.3046975799 * FP1),// MIDI Note 68
    (fp_int)(440.0 * FP1),         // MIDI Note 69
    (fp_int)(466.1637615181 * FP1),// MIDI Note 70
    (fp_int)(493.8833012561 * FP1),// MIDI Note 71
    (fp_int)(523.2511306012 * FP1),// MIDI Note 72
    (fp_int)(554.3652619537 * FP1),// MIDI Note 73
    (fp_int)(587.3295358348 * FP1),// MIDI Note 74
    (fp_int)(622.2539674442 * FP1),// MIDI Note 75
    (fp_int)(659.2551138257 * FP1),// MIDI Note 76
    (fp_int)(698.4564628660 * FP1),// MIDI Note 77
    (fp_int)(739.9888454233 * FP1),// MIDI Note 78
    (fp_int)(783.9908719635 * FP1),// MIDI Note 79
    (fp_int)(830.6093951599 * FP1),// MIDI Note 80
    (fp_int)(880.0 * FP1),         // MIDI Note 81
    (fp_int)(932.3275230362 * FP1),// MIDI Note 82
    (fp_int)(987.7666025122 * FP1),// MIDI Note 83
    (fp_int)(1046.5022612024 * FP1),// MIDI Note 84
    (fp_int)(1108.7305239075 * FP1),// MIDI Note 85
    (fp_int)(1174.6590716696 * FP1),// MIDI Note 86
    (fp_int)(1244.5079348883 * FP1),// MIDI Note 87
    (fp_int)(1318.5102276515 * FP1),// MIDI Note 88
    (fp_int)(1396.9129257320 * FP1),// MIDI Note 89
    (fp_int)(1479.9776908465 * FP1),// MIDI Note 90
    (fp_int)(1567.9817439270 * FP1),// MIDI Note 91
    (fp_int)(1661.2187903198 * FP1),// MIDI Note 92
    (fp_int)(1760.0 * FP1),         // MIDI Note 93
    (fp_int)(1864.6550460724 * FP1),// MIDI Note 94
    (fp_int)(1975.5332050245 * FP1),// MIDI Note 95
    (fp_int)(2093.0045224048 * FP1),// MIDI Note 96
    (fp_int)(2217.4610478150 * FP1),// MIDI Note 97
    (fp_int)(2349.3181433393 * FP1),// MIDI Note 98
    (fp_int)(2489.0158697766 * FP1),// MIDI Note 99
    (fp_int)(2637.0204553030 * FP1),// MIDI Note 100
    (fp_int)(2793.8258514640 * FP1),// MIDI Note 101
    (fp_int)(2959.9553816931 * FP1),// MIDI Note 102
    (fp_int)(3135.9634878540 * FP1),// MIDI Note 103
    (fp_int)(3322.4375806396 * FP1),// MIDI Note 104
    (fp_int)(3520.0 * FP1),         // MIDI Note 105
    (fp_int)(3729.3100921447 * FP1),// MIDI Note 106
    (fp_int)(3951.0664100490 * FP1),// MIDI Note 107
    (fp_int)(4186.0090448096 * FP1),// MIDI Note 108
    (fp_int)(4434.9220956300 * FP1),// MIDI Note 109
    (fp_int)(4698.6362866785 * FP1),// MIDI Note 110
    (fp_int)(4978.0317395533 * FP1),// MIDI Note 111
    (fp_int)(5274.0409106059 * FP1),// MIDI Note 112
    (fp_int)(5587.6517029281 * FP1),// MIDI Note 113
    (fp_int)(5919.9107633862 * FP1),// MIDI Note 114
    (fp_int)(6271.9269757080 * FP1),// MIDI Note 115
    (fp_int)(6644.8751612791 * FP1),// MIDI Note 116
    (fp_int)(7040.0 * FP1),         // MIDI Note 117
    (fp_int)(7458.6201842894 * FP1),// MIDI Note 118
    (fp_int)(7902.1328200980 * FP1),// MIDI Note 119
    (fp_int)(8372.0180896192 * FP1),// MIDI Note 120
    (fp_int)(8869.8441912599 * FP1),// MIDI Note 121
    (fp_int)(9397.2725733570 * FP1),// MIDI Note 122
    (fp_int)(9956.0634791066 * FP1),// MIDI Note 123
    (fp_int)(10548.0818212118 * FP1),// MIDI Note 124
    (fp_int)(11175.3034058561 * FP1),// MIDI Note 125
    (fp_int)(11839.8215267723 * FP1),// MIDI Note 126
    (fp_int)(12543.8539514160 * FP1) // MIDI Note 127
};
