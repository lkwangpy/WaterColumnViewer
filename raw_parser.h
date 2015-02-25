#ifndef RAW_PARSER_H
#define RAW_PARSER_H

#include "raw_parser_global.h"
#include <string>
#include <vector>

typedef struct mb_time_struct
{
    u16 year;
    u8  month;
    u8  day;
    u8  hour;
    u8  minutes;
    u8  seconds;
    u16 milliseconds;
}
mbt;

typedef struct mb_attitude_struct
{    
    u64 timeTag;    // time ref. given in milliseconds
    f32 pitch;      // positive value is nose up
    f32 roll;       // positive value is roll to starboard
    f32 heave;      // positive value is sensor up
    f32 yaw;        // positive value is turn right
    f32 heading;    // same as yaw??
    mbt time;
}
mb_att;


typedef struct mb_position_struct
{
    u64 timeTag;
    d64 latitude;
    d64 longitude;
    d64 altitude;
    mbt time;
}
mb_pos;


typedef struct mb_beam_struct
{
    d64 z;                  // (z) depth from transmit transducer
    d64 y;                      // (y) acrosstrack_distance 2S -32768 to 32766
    d64 x;                      // (x) alongtrack_distance  2S -32768 to 32766
    d64 beam_depression_angle;  // in 0.01° 2S -11000 to 11000
    d64 beam_azimuth_angle;     // in 0.01° 2U 0 to 56999
    d64 range;                  // (one - way travel time) 2U 0 to 65534
    u8  quality_factor;         // 1U 0 to 254
    u8  detection_window_length;// length_of_detection_window (samples/4) 1U 1 to 254
    s8  reflectivity;           // (BS) in 0.5 dB resolution)
                                // (Example: -20 dB = 216)
                                // -128 to +126
    u16  beam_number;            // 1 to 512

    float angel_of_incidence;

    d64 local_x;
    d64 local_y;
    d64 local_z;
}
mb_bea;


typedef struct mb_ping_struct
{
    mbt time;

    d64 latitude;
    d64 longitude;
    d64 altitude;

    d64 local_x;
    d64 local_y;

    d64 roll;
    d64 pitch;
    d64 heave;
    d64 heading;

    d64 velocity;
    d64 ship_speed;

    //! correction
    d64 tide;
    d64 dynamic_draft;
    f32 tt_depth;                   // Transmit Transducer depth re water level at time of ping in m


    d64 ping_number;

    u16  max_beam_num;              // maximum number of beams possible >48
    u16  valid_beam_num;            // N, 1-512

    float beam_width_degree;

    std::vector<mb_bea> beams;
}
mb_pin;

typedef struct mb_swath_info_struct {
    mbt ping_start_time;
    mbt ping_end_time;
    int max_beam;
    float beam_width;
    std::string multibeam_model;
    std::string survey_region;
} mb_swath_info;


typedef struct mb_beam_backscatter_struct {
    u8  beam_index_number;      // 1U 0 to 253 The beam index number is the beam number - 1.
    s8  sorting_direction;      // -1 or 1 The first sample in a beam has lowest range if 1, highest if -1.
    u16 samples_num;            // per beam = Ns
    u16 centre_sample_num;      // The centre sample number is the detection point of a beam.

    std::vector<u8> amplitudes;
}mb_bscatter;

typedef struct mb_sinppet_struct
{
    u16 mean_absorption_coefficient;    // in 0.01 dB/km  1 to 20000
    u16 pulse_length;                   // in μs >50
    u16 range;                          // to normal incidence used to correct sample amplitudes in no. of samples
    u16 start_range_sample;             // Start range sample of TVG ramp if not enough, dynamic range (0 else)
    u16 stop_range_sample;              // of TVG ramp if not enough, dynamic range (0 else)
    s8  normal_incidence_BS;            // in dB (BSN)  (Example: -20 dB = 236)
    s8  oblique_BS;                     // in dB (BSO) (Example:–1 dB = 255)
    u16 tx_beamwidth;                   // in 0.1°
    u8  TVG_law_crossover_angle;        // in 0.1°
    u8  valid_beams_num;                // (N) 1 to 254

    std::vector<mb_bscatter> backscatters;
}
mb_sni;

typedef struct mb_swath_struct
{
    mb_swath_info info;
    std::vector<mb_pos> poss;
    std::vector<mb_att> atts;
    std::vector<mb_pin> pins;
    std::vector<mb_sni> snis;
}
mb_swa;

class Raw_Parser
{
public:
    virtual bool parse() = 0;

    virtual std::string basicInfo_toHtml() = 0;

    virtual mb_swa to_common_swath() = 0;
};

inline float bf2lf( const float inFloat )
{
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}

#endif // RAW_PARSER_H
