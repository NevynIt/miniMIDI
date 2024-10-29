#include "mod_Encoders.h"

void mod_Encoders::Init() {
    // Initialize the quadrature decoder
    if (!decoder.init(pio0)) {
        // Handle initialization failure
        return;
    }

    // Initialize encoder counts
    encoderCounts.resize(NUM_ENCODERS, 0);

    // Add quadrature encoders
    for (int i = 0; i < NUM_ENCODERS; ++i) {
        int32_t result = decoder.addQuadratureEncoder(i * 2); // Assuming pins 0&1, 2&3, 4&5, 6&7
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