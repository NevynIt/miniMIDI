#include "mod_Encoders.h"
#include "stdio.h"
#include "App.h"

#ifdef ENCODERS_USE_PIO
void mod_Encoders::Init()
{
    // Initialize the quadrature decoder
    if (!decoder.init(PIO_ENCODERS))
    {
        // Handle initialization failure
        return;
    }

    // Add quadrature encoders
    for (int i = 0; i < NUM_ENCODERS; ++i)
    {
        int32_t result = decoder.addQuadratureEncoder(GPIO_Enc_Base + i * 2); // Assuming consecutive pins A,B, A,B, ...
        if (result < 0)
        {
            // Handle failure to add encoder
        }
    }
}

void mod_Encoders::Tick()
{
    // Update encoder counts
    for (int i = 0; i < NUM_ENCODERS; ++i)
    {
        count[i] = decoder.getCount(i);
    }
    // char buffer[100];
    buttons = (gpio_get_all() >> (GPIO_Enc_Base + NUM_ENCODERS * 2)) & (~(~0 << (NUM_ENCODERS)));
    // sprintf(buffer, "%d, %d, %d, %d, %d\n", buttons, count[0], count[1], count[2], count[3]);
    // app.display.clear_square(0,16,128,8);
    // app.display.draw_text(0,16,1,buffer);
}

#else
void mod_Encoders::Init()
{
    // Initialize encoder pins
    for (int i = GPIO_Enc_Base; i < GPIO_Enc_Base + NUM_ENCODERS*3; i++)
    {
        gpio_init(i);
        gpio_set_dir(i, GPIO_IN);
        gpio_pull_up(i);
    }
}

void mod_Encoders::Tick()
{
    Decode();
    // char buffer[100];
    buttons = (~gpio_get_all() >> (GPIO_Enc_Base + NUM_ENCODERS * 2)) & (~(~0 << (NUM_ENCODERS)));
    // sprintf(buffer, "%d, %d, %d, %d, %d\n", buttons, count[0], count[1], count[2], count[3]);
    // app.display.clear_square(0,16,128,8);
    // app.display.draw_text(0,16,1,buffer);
}

void mod_Encoders::Test()
{
    INTERVALCHECK(1000)

    // printf("Encoder test\n");
    for (int i = 0; i < NUM_ENCODERS; i++)
    {
        count[i] += (rand() % 3 + 1) * 4 * (rand() % 2 ? 1 : -1);
        if (count[i] > 60)
            count[i] = 60;
        if (count[i] < -60)
            count[i] = -60;
    }
}

// Expected initial signal levels
#define QUADRATURE_INIT (~(~0 << (NUM_ENCODERS * 2)))

__attribute__((optimize("unroll-loops"))) inline void mod_Encoders::Decode()
{
    uint32_t curr = (gpio_get_all() >> GPIO_Enc_Base) & QUADRATURE_INIT;	
    static uint32_t prev = QUADRATURE_INIT;
    uint32_t xora = (curr ^ (prev >> 1)) & 0X55555555;
    uint32_t xorb = (prev ^ (curr >> 1)) & 0X55555555;
    uint32_t decoded = ((xora ^ xorb) << 1) | xora;
    for (int id = 0; id < NUM_ENCODERS; id++, decoded >>= 2)
    {
        if (decoded & 2)
        {
            count[id] += (decoded & 1) ? +1 : -1;
        }
    }
    prev = curr;
}
#endif