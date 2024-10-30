#include "mod_Encoders.h"

void mod_Encoders::Init() {
    // Initialize the quadrature decoder
    if (!decoder.init(PIO_ENCODERS)) {
        // Handle initialization failure
        return;
    }

    // Initialize encoder counts
    encoderCounts.resize(NUM_ENCODERS, 0);

    // Add quadrature encoders
    for (int i = 0; i < NUM_ENCODERS; ++i) {
        int32_t result = decoder.addQuadratureEncoder(GPIO_Enc_Base + i * 2); // Assuming consecutive pins A,B, A,B, ...
        if (result < 0) {
            // Handle failure to add encoder
        }
    }
}

void mod_Encoders::Tick() {
    // Update encoder counts
    for (int i = 0; i < NUM_ENCODERS; ++i) {
        encoderCounts[i] = decoder.getCount(i);
    }
}

int32_t mod_Encoders::GetEncoderCount(int index) {
    if (index < 0 || index >= NUM_ENCODERS) {
        return 0; // Handle invalid index
    }
    return encoderCounts[index];
}