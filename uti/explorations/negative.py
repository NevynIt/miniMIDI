import struct

for i in range(-128, 128):
    #print the binary form of the number as a signed byte
    b = struct.pack('b', i)
    #convert the byte to a unsigned integer
    i2 = struct.unpack('B', b)[0]
    print(i, i2, bin(i2))

#numbers are stored in two's complement form, which means that the most significant bit is the sign bit, 0 for positive and 1 for negative
#alternatively, think that [0, 127] is positive and [128, 255] is negative
#seeing this as a phase of a sinusoidal wave, the sin of the negative number is the same as the sin of the positive number with the same binary representation
