#ifndef MIDI_BRIDGE_DATA_H
#define MIDI_BRIDGE_DATA_H

#include <stdio.h>

typedef uint8_t MIDI_msg_t[4];

typedef enum{
  port_usb  = 0, 
  port_ser1 = 1,
  port_ser2 = 2,
  port_midi = 3
} port_t;

typedef uint16_t ruleset_t;

#define ADC_BITS 12
typedef uint8_t comp_t[(1u << ADC_BITS)];

typedef uint16_t decomp_t[256];

typedef struct {
  uint16_t match, action, data;
} rule_t;

typedef struct {
  uint16_t type, param;
} match_t;

typedef struct {
  uint16_t transform, delta;
} sequence_t;

typedef struct {
  uint8_t target, data[3];
} transformation_t;

typedef uint8_t lookup_t[256];

#define FL_ENVELOPE_ELEMENTS 16
typedef struct {
  uint8_t data[FL_ENVELOPE_ELEMENTS];
} envelope_t;

#define FL_LIST_ELEMENTS 30
typedef struct {
  uint16_t continuation;
  uint8_t data[FL_LIST_ELEMENTS];
} list_t;

#define FL_CONFIG_ELEMENTS 64
typedef struct {
  uint8_t data[FL_CONFIG_ELEMENTS];
} config_t;

typedef struct {
  port_t port;
  uint8_t cable, channel, note;
} active_note_t;

#define SEQ_POLYPHONY 96
typedef struct {
  uint8_t S0, S1;
  port_t port;
  MIDI_msg_t msg;
  uint16_t start_sequence, transform;
  uint16_t current_sequence;
  uint8_t index;
  uint32_t alarm;
  active_note_t active_notes[SEQ_POLYPHONY];
} seq_slot_t;

typedef struct {
  uint16_t envelope;
  uint8_t phase;
  uint32_t t0;
} active_envelope_t;

typedef struct {
  uint8_t l,r;
} sample_t;

#define MAX_CMD_SIZE (256+128)
typedef uint8_t cmd_buffer_t[MAX_CMD_SIZE*2];

#endif
