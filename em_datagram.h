#ifndef EM_DATAGRAM_H
#define EM_DATAGRAM_H

#include "raw_parser_global.h"

#pragma pack(1)


/* datagram start and end byte */
#define	EM3_START_BYTE		0x02
#define	EM3_END_BYTE		0x03
#define	EM3_END             0x03

//! Multibeam data
#define EM_Depth_Datagram               0x44    // 68  K This datagram is used for EM 2000, EM 3000, EM 3002, EM 1002, EM 300 and EM 120.
#define EM_XYZ_88                       0x58    // 88  X This datagram replaces the previous depth (D) datagram for the new multibeam models (EM 2040, EM 710, EM 122, EM 302, ME 70).
#define EM_Central_Beams_Echogram       0x4B    // 75  K This datagram is only available for EM 120 and EM 300.
#define EM_Raw_Range_and_Beam_Angle_F   0x46    // 70  F Only used for EM 3000, old
#define EM_Raw_Range_and_Beam_Angle_f   0x66    // 102 f Used for EM 120, EM 300, EM 1002, EM 2000, EM 3000 and EM 3002
#define EM_Raw_Range_and_Angle_78       0x4e    // 78  N This datagram replaces the previous Raw range and beam angle f datagram for the new multibeam models (EM 2040, EM 710, EM 302, EM 122, ME 70).
#define EM_Seabed_image_datagram        0x53    // 83  S This datagram is used for EM 2000, EM 3000, EM 3002, EM 1002, EM 300 and EM 120.
#define EM_Seabed_Image_Data_89         0x59    // 89  Y This datagram replaces the previous Seabead image (S) datagram for the new multibeam models (EM 2040, EM 710, EM 302, EM 122, ME 70).
#define EM_Water_Column_Datagram        0x6B    // 107 k Used for EM 122, EM 302, EM 710, EM 2040, EM 3002 and ME 70.

//! External sensors
#define EM_Attitude_Datagram            0x41    // 65  A(ttitude data)
#define EM_Net_Att_Velocity_datagram    0x6E    // 110 n(etwork data)
#define EM_Clock_Datagram               0x43    // 67  C(lock data)
#define EM_Depth_or_Height_Datagram     0x68    // 104 h(eight data)
#define EM_Heading_Datagram             0x48    // 72  H(eading data)
#define EM_Position_Datagrams           0x50    // 80  P(osition data)
#define EM_Single_beam_depth_Datagrams  0x45    // 69  E(cho sounder data)
#define EM_Tide_Datagram                0x54    // 84  T(ide data)


//! Sound speed
#define EM_Surface_Sound_Speed          0x47    // 71  G
#define EM_Sound_Speed_Profile_Datagram 0x55    // 85  U This datagram will contain the profile actually used in the real time raybending calculations to convert range and angle to xyz data. It will usually be issued together with the installation parameter datagram.
#define EM_Sound_Velocity_Profile       0x56    // 86  V A little different from 0x55, used by older EM3000
#define EM_Kongsberg_Maritime_SSP       0x57    // 87  W

//! Multibeam parameters
#define EM_Install_Parameters_Start     0x49    // 73  I(nstallation parameters)
#define EM_Install_Parameters_Sop       0x69    // 105 i(nstallation parameters)
#define EM_Install_Parameters_Remote    0x70    //! error exist!!! 112 p !=114 r(emote information) 0x72???
#define EM_Runtime_Parameters           0x52    // 82  R(untime parameter)
#define EM_Mechanical_Transducer_Tilt   0x4A    // 74  J
#define EM_ExtraParameters_Datagram     0x33    // 51  3

//! PU information and status
#define EM_PU_Id_output                 0x30    // 48  0
#define EM_PU_Status_output             0x31    // 49  1
#define EM_PU_BIST_result_output        0x42    // 66  B

typedef std::pair<u8, std::string> EM_TYRES_STR_PAIR;
#define EM_TYRES_NUM 30

const EM_TYRES_STR_PAIR G_EM_TYRES_STR[EM_TYRES_NUM] = {
  EM_TYRES_STR_PAIR ( 0x44 , " EM_Depth_Datagram               "),
  EM_TYRES_STR_PAIR ( 0x58 , " EM_XYZ_88                       "),
  EM_TYRES_STR_PAIR ( 0x4B , " EM_Central_Beams_Echogram       "),
  EM_TYRES_STR_PAIR ( 0x46 , " EM_Raw_Range_and_Beam_Angle_F   "),
  EM_TYRES_STR_PAIR ( 0x66 , " EM_Raw_Range_and_Beam_Angle_f   "),
  EM_TYRES_STR_PAIR ( 0x4e , " EM_Raw_Range_and_Angle_78       "),
  EM_TYRES_STR_PAIR ( 0x53 , " EM_Seabed_image_datagram        "),
  EM_TYRES_STR_PAIR ( 0x59 , " EM_Seabed_Image_Data_89         "),
  EM_TYRES_STR_PAIR ( 0x6B , " EM_Water_Column_Datagram        "),
  EM_TYRES_STR_PAIR ( 0x41 , " EM_Attitude_Datagram            "),
  EM_TYRES_STR_PAIR ( 0x6E , " EM_Net_Att_Velocity_datagram    "),
  EM_TYRES_STR_PAIR ( 0x43 , " EM_Clock_Datagram               "),
  EM_TYRES_STR_PAIR ( 0x68 , " EM_Depth_or_Height_Datagram     "),
  EM_TYRES_STR_PAIR ( 0x48 , " EM_Heading_Datagram             "),
  EM_TYRES_STR_PAIR ( 0x50 , " EM_Position_Datagrams           "),
  EM_TYRES_STR_PAIR ( 0x45 , " EM_Single_beam_depth_Datagrams  "),
  EM_TYRES_STR_PAIR ( 0x54 , " EM_Tide_Datagram                "),
  EM_TYRES_STR_PAIR ( 0x47 , " EM_Surface_Sound_Speed          "),
  EM_TYRES_STR_PAIR ( 0x55 , " EM_Sound_Speed_Profile_Datagram "),
  EM_TYRES_STR_PAIR ( 0x56 , " EM_Sound_Velocity_Profile       "),
  EM_TYRES_STR_PAIR ( 0x57 , " EM_Kongsberg_Maritime_SSP       "),
  EM_TYRES_STR_PAIR ( 0x49 , " EM_Install_Parameters_Start     "),
  EM_TYRES_STR_PAIR ( 0x69 , " EM_Install_Parameters_Sop       "),
  EM_TYRES_STR_PAIR ( 0x70 , " EM_Install_Parameters_Remote    "),
  EM_TYRES_STR_PAIR ( 0x52 , " EM_Runtime_Parameters           "),
  EM_TYRES_STR_PAIR ( 0x4A , " EM_Mechanical_Transducer_Tilt   "),
  EM_TYRES_STR_PAIR ( 0x33 , " EM_ExtraParameters_Datagram     "),
  EM_TYRES_STR_PAIR ( 0x30 , " EM_PU_Id_output                 "),
  EM_TYRES_STR_PAIR ( 0x31 , " EM_PU_Status_output             "),
  EM_TYRES_STR_PAIR ( 0x42 , " EM_PU_BIST_result_output        ")
};

enum EM_ExPara_Content_identifier {
    Calib_File = 1,
    Log_All_Heights,
    Sound_Velocity_at_Transducer,
    Sound_Velocity_Profile
};

typedef struct em_datagram_header_struct
{
    u32 number_of_bytes;    // Number of bytes in datagram
    u8  start_id;           // STX(Always 0x02)
    u8  type_of_datagram;   //
    u16 em_model_number;    // (examle:EM3000=3000)
    u32 date;               // Date = year*10000 + month*100 + day; Feb 26,1995=19950226
    u32 millsecond;         // eg.08:12:51.234=29571234;
    u16 sequential_counter; // sequential counter, ping counter when 0x44, survey line number when 0x49, profile counter when 0x56, so on
    u16 serial_number;      // system serial number
}
em_datagram_header;


typedef struct em_datagram_end_struct
{
    u8  spare_byte;         // Sensor system descriptor when 0x41
    u8  end_identifier;
    u16 check_sum;
}
em_datagram_end;


typedef struct em_datagram_end2_struct
{
    u8  end_identifier;
    u16 check_sum;
}
em_datagram_end2;

//! SVP, V (Always 056h), not 055
//! em_svp_datagram's full struct: em_datagram_header + em_svp_datagram + N * em_svp_entry + em_datagram_end
typedef struct em_sound_velocity_profile_datagram_struct
{
//    em_datagram_header  header;
//    u16     profile_counter;    // (sequential counter)
//    u16     serial_number;
    u32     date;
    u32     time;
    u16     number_of_entries;  // N
    u16     depth_resolution;   // cm
    // N*em_svp_entry
    // em_datagram_end
}
em_svp_datagram;


typedef struct em_sound_velocity_profile_entry_struct
{
    u16	depth;              // 0 to 1200000
    u16	sound_velocity;     // in dm/s 14000-17000
}
em_svp_entry;


/**
  * Sound speed profie datagram, Type of datagram = U (Always 055h)
  *
  */
//typedef struct em_sound_speed_profile_datagram_struct
//{
//    u32     date;
//    u32     time;               /** since midnight in milliseconds */
//    u16     number_of_entries;  // N
//    u16     depth_resolution;   // cm
//}
//em_ssp_datagram;


typedef em_svp_datagram em_ssp_datagram;

typedef struct em_sound_speed_profile_entry_struct
{
    u32	depth;              // 0 to 1200000
    u32	sound_velocity;     // in dm/s 14000-17000
}
em_ssp_entry;

//! Installation parameters, I or i(nstallation parameters) or r(emote information) Start = 049h Stop = 069h Remote info = 70h
//! em_install_datagram's full struct: em_datagram_header + em_svp_datagram + N * em_svp_entry + em_datagram_end
typedef struct em_install_parameters_datagram_struct
{
    // em_datagram_header

    u16 sec_serial_num;

    // a lot of chars

    // em_datagram_end
}
em_ins_datagram;


//! R(untime parameter) (Always 052h)
//! em_install_datagram's full struct: em_datagram_header + em_rtp_datagram + em_datagram_end
typedef struct em_runtime_parameters_struct
{
    // em_datagram_header _header;

    u8  Operator_Station_status;
    u8  CPU_status;
    u8  BSP_status;
    u8  sonar_head_or_rransceiver_status;
    u8  mode;
    u8  filter_identifier;                      // 0 to 255
    u16 minimum_depth;                          // in m  0 to 10000 —
    u16 maximum_depth;                          // in m  1 to 12000 —
    u16 absorption_coefficient;                 // in 0.01 dB/km  1 to 20000
    u16 transmit_pulse_length;                  // in μs  1 to 50000
    u16 transmit_beamwidth;                     // in 0.1 degrees  1 to 300
    s8  transmit_power_re_max;                  // in dB
    u8  receive_beamwidth;                      // in 0.1 degrees  5 to 80
    u8  receive_bandwidth;                      // in 50 Hz resolution  1 to 255
    u8  mode_2;                                 // 0 to 50
                                                // or Receiver fixed gain setting in dB
    u8  TVG_law_crossover_angle;                // in degrees 1U 2 to 30 —
    u8  source_of_sound_speed_at_transducer;    // 0 to 3
    u16 maximum_port_swath_width;               // in m  10 to 30000
    u8  beam_spacing;                           // 0 to 3
    u8  maximum_port_coverage_in_degrees;       // 10 to 110
    u8  yaw_and_pitch_stabilization_mode;
    u8  maximum_starboard_coverage_in_degrees;  // 10 to 110
    u16 maximum_starboard_swath_width;          // in m  10 to 30000
    s16 transmit_along_tilt;                    // in 0.1 deg.
                                                // or durotong speed in dm/s
    // em_datagram_end _end;
    // _end.spare_byte is Filter identifier 2 0 to 255
    // or HiLo frequency absorption coefficient ratio 0 to 120
}
em_rtp_datagram;


/**
 * D(epth data) (Always 44h)
 * This datagram is used for EM 2000, EM 3000, EM 3002, EM 1002, EM 300 and EM 120.
 * em_depth_datagram_struct's full struct: em_datagram_header + em_dep_datagram + N * em_dep_entry + em_datagram_end
 * Heave, roll, pitch, sound speed at the transducer depth and ray bending through the water column have been applied.
 * \note
 * Heading is not applied!
 * More info refer to the documents
 */
typedef struct em_depth_datagram_struct
{
    // em_datagram_header _header;

    u16  heading;                   // heading of vessel in 0.01°
    u16  sound_speed;               // sound speed at transducer in dm/s
    u16  transducer_depth_re;       // Transmit Transducer depth re water level at time of ping in cm
    u8   max_beam_num;              // maximum number of beams possible >48
    u8   valid_beam_num;            // N, 1-254
    u8   z_resolution;              // in cm 1-254
    u8   x_y_resolution;            // in cm 1-254
    u16  sampling_rate;             // f in Hz
                                    //! Depth difference between sonar heads in the EM 3000D

    //Repeat cycle N em_dep_entry
}
em_dep_datagram;


typedef struct em_depth_entry_struct {
    u16 depth;                  // (z) from transmit transducer
                                //! (unsigned for EM 120 and EM 300)
                                // -32768 to +32766 or
                                // 1 to 65534
    s16 acrosstrack_distance;   // (y) 2S -32768 to 32766
    s16 alongtrack_distance;    // (x) 2S -32768 to 32766
    u16 beam_depression_angle;  // in 0.01° 2S -11000 to 11000
    u16 beam_azimuth_angle;     // in 0.01° 2U 0 to 56999
    u16 range;                  // (one - way travel time) 2U 0 to 65534
    u8  quality_factor;         // 1U 0 to 254
    u8  detection_window_length;// length_of_detection_window (samples/4) 1U 1 to 254
    s8  reflectivity;           // (BS) in 0.5 dB resolution)
                                // (Example: -20 dB = 216)
                                // -128 to +126
    u8  beam_number;            // 1 to 254
}
em_dep_entry;


/**
  * This datagram is used for the models EM 2040, EM 2040C, EM 710, EM 122, EM
  * 302 and ME70BO. All receiver beams are included, check detection info and real time
  * cleaning for beam status (note 4 and 5)
  */
typedef struct em_xyz88_datagram_struct
{
    u16 heading;                    // heading of vessel in 0.01°
    u16 sound_speed;                // sound speed at transducer in dm/s
    f32 tt_depth;                   // Transmit Transducer depth re water level at time of ping in m
    u16 N;                          // maximum number of beams
    u16 valid_N;                    // Number of valid detections N, 1-254
    f32 sampling_rate;              // f in Hz
    u8  scanning_info;
    u8  spare[3];
}
em_xyz88_datagram;

typedef struct em_xyz88_entry_struct
{
    f32 z;                          // Depth (z) from transmit transducer in m
    f32 y;                          // Acrosstrack distance (y) in m
    f32 x;                          // Alongtrack distance (x) in m
    u16 detection_window_length;    // Detection window length in samples
    u8 quality_factor;              // 0 – 254 1U Quality factor
    s8 IBA;                         // Beam incidence angle adjustment (IBA) in 0.1 deg 1S
    u8 dete_info;                   // Detection information
    s8 realtime_cleam_info;         // Real time cleaning information
    s16 BS;                         // Reflctivity (BS) in 0.1 dB resolution
                                    // (Example: –20.1 dB = FF37h= 65335)
}
em_xyz88_entry;

typedef struct em_xyz88_packet_struct {
    em_datagram_header xyz88_header;
    em_xyz88_datagram xyz88_info;
    std::vector<em_xyz88_entry> xyz88_data;
}
em_xyz88_pak;

/**
  * raw range and beam angle datagrams f
  */
typedef struct em_raw_range_and_beam_angle_datagrams_struct
{
    u16 Ntx;    // 1 to 20 Number of transmit sectors = Ntx
    u16 N;      // 1 to 1999 Number of valid receive beams = N
    u32 F;      // 100 to 100000 * 100 Sampling frequency in 0.01 Hz (F)
    int ROV_depth; // in 0.01 m
    u16 sound_speed; //  14000 to 16000 2U Sound speed at transducer in 0.1 m/s
    u16 max_possible_beams_no;   // 1 to 1999 Maximum number of beams possible
    u16 spare[2];
}
em_rrba_datagram;

typedef struct em_rrba_transmit_entry_struct
{
    s16 tilt_angle; //— -2900 to 2900 2S Tilt angle ref TX array in 0.01°
    u16 focus_range;//— 0 to 65535 2U Focus range in 0.1 m (0 = No focus)
    u32 signal_length; //— — 4U Signal length in μs
    u32 transmit_time_offset; //— — 4U Transmit time offset in μs
    u32 center_frequency;  //— — 4U Center frequency in Hz
    u16 bandwidth;//— 1 to 65535 2U Bandwidth in 10 Hz
    u8 signal_waveform_identifir;//1 0 to 99 1U Signal waveform identifir
    u8 transmit_sector_number;//— 0 to 99 1U Transmit sector number
}
em_rrba_transmit_entry;

typedef struct em_rrba_beam_entry_struct
{
    s16 beam_pointing_angle; //— -11000 to 11000 Beam pointing angle ref RX array in 0.01° 2S
    u16 range;//2 0 to 65535 2U Range in 0.25 samples (R)
    u8 ts_no;//— 0 to 19 1U Transmit sector number
    s8 reflctivity;//— -128 to 127 1S Reflctivity (BS) in 0.5 dB resolution
    u8 quality_factor;//— 0 to 254 1U Quality factor
    u8 detection_window_length;//Detection window length in samples (/4 if phase) — 1 to 254 1U
    s16 beam_number;//-1999 to 1999 3 2S Beam number
    u16 spare;
}
em_rrba_beam_entry;

struct em_rrba_pak {
    em_datagram_header rrba_header;
    em_rrba_datagram rrba_info;
    std::vector<em_rrba_transmit_entry> rrba_tx;
    std::vector<em_rrba_beam_entry> rrba_beams;
};

/**
  * raw range and beam angle datagrams 78
  */
typedef struct em_raw_range_and_beam_angle78_datagrams_struct
{
    u16 sound_speed;// — 14000 to 16000 2U Sound speed at transducer in 0.1 m/s
    u16 Ntx;        // 1 to 20 Number of transmit sectors = Ntx
    u16 Nrx;          // 1 to 1999 Number of valid receive beams = N
    u16 valid_N;    // Number of valid detections
    f32 F;          // Sampling frequency in Hz (F)
    u32 dspare;
}
em_rrba78_datagram;

typedef struct em_rrba78_transmit_entry_struct
{
    s16 tilt_angle;                 // -2900 to 2900 Tilt angle ref TX array in 0.01°
    u16 focus_range;                // 0 to 65535 Focus range in 0.1 m (0 = No focus)
    f32 signal_length;              // Signal length in s
    f32 transmit_time_offset;       // Transmit time offset in s
    f32 center_frequency;           // Center frequency in Hz
    u16 mean_absorption_coeff;      // mean absorption coeff. in 0.01 dB/k
    u8  signal_waveform_identifir;  // 0 to 99 1U Signal waveform identifir
    u8  transmit_sector_number;     /// TX array index
    f32 signal_bandwidth;           // in Hz
}
em_rrba78_transmit_entry;

typedef struct em_rrba78_beam_entry_struct
{
    s16 beam_pointing_angle;     // -11000 to 11000 Beam pointing angle ref RX array in 0.01° 2S
    u8  ts_no;                   // 0 to 19 1U Transmit sector number
    u8  detect_info;
    u16 detection_window_length; // Detection window length in samples
    u8  quality_factor;          // 0 to 254 1U Quality factor
    s8  D_corr;
    f32 two_way_travel_time;     // in s
    s16 BS;                      // Reflctivity (BS) in 0.1 dB resolution
    s8  real_time_cleaning_info;
    u8  spare;
}
em_rrba78_beam_entry;

struct em_rrba78_pak
{
    em_datagram_header rrba78_header;
    em_rrba78_datagram rrba78_info;
    std::vector<em_rrba78_transmit_entry> rrba_tx;
    std::vector<em_rrba78_beam_entry> rrba_data;
};

/**
 * S(eabed image datagram) (Always 53h)
 * This datagram is used for EM 2000, EM 3000, EM 3002, EM 1002, EM 300 and EM 120.
 * em_seabed_image_datagram_struct's full struct: em_datagram_header + em_img_datagram + N * em_img_entry + em_datagram_end
 */
typedef struct em_seabed_image_datagram_struct {
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
}
em_img_datagram;


typedef struct em_image_entry_struct {
    u8  beam_index_number;      // 1U 0 to 253 The beam index number is the beam number - 1.
    s8  sorting_direction;      // -1 or 1 The first sample in a beam has lowest range if 1, highest if -1.
    u16 samples_num;            // per beam = Ns
    u16 centre_sample_num;      // The centre sample number is the detection point of a beam.
}
em_img_entry;

/**
  * Seabed image data 89
  * Used for EM 122, EM 302, EM 710, ME70 BO, EM 2040 and EM 2040C.
  */
typedef struct em_seabed_image_89_datagram_struct {
    f32 sampling_frequency;             // in Hz
    u16 range_to_normal_incidence;      // Range to normal incidence used to correct sample amplitudes  in no. of samples, 1 to 16384
    s16 BSN;                            // normal_incidence_BS in 0.1 dB (BSN)
    s16 BSO;                            // oblique_BS in 0.1 dB (BSO)
    u16 tx_beamwidth;                   // in 0.1°
    u16 TVG_law_crossover_angle;        // in 0.1°
    u16 N;                              // valid_beams_num (N) 1 to 254
}
em_img89_datagram;

typedef struct em_image89_entry_struct {
    s8  sorting_direction;      // -1 or 1 The first sample in a beam has lowest range if 1, highest if -1.
    u8  detection_info;
    u16 Ns;                     // samples_num per beam = Ns
    u16 centre_sample_num;      // The centre sample number is the detection point of a beam.
}
em_img89_entry;


struct em_img89_pak {
    em_datagram_header img89_header;
    em_img89_datagram img89_info;
    std::vector<em_img89_entry> img89_data;
    std::vector<s16> img89_amps;
};

/**
  * Water column datagram. Used for EM 122, EM 302, EM 710, EM 2040, EM 3002 and ME70BO.
  * Type of datagram = k (Always 6Bh)
  * Total Range in meters = Sound speed * detected range / (sample rate * 2) = SS10 * 3
  * DR / (FS100 * 2) = 5 * SS * DR / FS (FS100= FS/100, SS10= SS/10). The range is
  * set to zero when the beam has no bottom detection.
  */
typedef struct em_water_column_datagram_struct {
    u16 Nd;                 /** Number of datagrams, 1 to Nd  */
    u16 datagram_numbers;   /** Datagram numbers, 1 to Nd */
    u16 Ntx;                // 1 to 20  Number of transmit sectors
    u16 total_no;           // 1 to Nd  Total no. of receive beams
    u16 Nrx;                // 1 to Nd  Number of beams in this datagram = Nrx
    u16 SS;                 // 14000 to 16000 2U Sound speed in 0.1 m/s (SS)
    u32 SF;                 // 1000 to 4000000 1 4U Sampling frequency in 0.01 Hz resolution (SF)
    s16 TX_time_heave;      // 1000 to 1000 2S TX time heave (at transducer) in cm
    u8  X;                  // 20 to 40 1U TVG function applied (X)
    s8  C;                  // TVG offset in dB (C)
    u8  scanning_info;      // Scanning info.
    u8  spare[3];
}
em_wc_datagram;

typedef struct em_wc_transmit_section_entry {
    s16 tilt_angle;         // re TX array in 0.01°
    u16 center_frequency;   // in 10 Hz — 1000 to 50000
    u8 tx_no;               // transmit sector number — 0 to 19
    u8 spare;
}em_wctx_entry;

typedef struct em_wc_beam_entry {
    s16 beam_angle; //Beam pointing angle ref vertical in 0.01° 2S
    u16 srs_no;     // Start Range sample number, 0 to 65534
    u16 Ns;         // Number of samples (Ns) 0 to 65534
    u16 DR;         // Detected range in samples (DR) 0 to 65534
    u8  tx_no;      // Transmit sector number, 0 to 19
    u8  beam_no;     // 0 to 254 1U Beam number
}
em_wcb_entry;

struct em_wcb {
    em_wcb_entry wcb;
    std::vector<s8> amps;
};

struct em_wci_pak {
    em_datagram_header h;
    em_wc_datagram wc;
    std::vector<em_wctx_entry> wctxs;
    std::vector<em_wcb> wcbs;
};

//! A(ttitude data) (Always 041h)
//! em_attitude_datagram_struct's full struct: em_datagram_header + em_att_datagram + N * em_att_entry + em_datagram_end
typedef struct em_attitude_datagram_struct {
    u16 num_of_entries;    // = N
}
em_att_datagram;

typedef struct em_attitude_entry_struct {
    u16 time;               // in milliseconds since record start 2U 0 to 65534 —
    u16 sensor_status;      // The sensor status will be copied from the input datagram’s two sync bytes if the sensor uses the EM format.
    s16 roll;               // in 0.01° 2S -18000 to 18000
    s16 pitch;              // in 0.01° 2S -18000 to 18000
    s16 heave;              // in cm 2S -1000 to 10000
    u16 heading;            // in 0.01° 2U 0 to 35999
}
em_att_entry;


//!  P(osition data) (Always 050h)
//!
typedef struct em_postion_datagram_struct {
    s32 latitude;                   // in decimal degrees*20000000 (negative if southern hemisphere)
                                    // (Example: 32°34’ S = -651333333)
    s32 longitude;                  // in decimal degrees*10000000 (negative if western hemisphere)
                                    // (Example: 110.25° E = 1102500000 )
    u16 position_fix_quality;       // measure of position fix quality in cm
    u16 vessel_speed;               // over ground in cm/s >0
    u16 vessel_course;              // over ground in 0.01° 0 to 35999
    u16 vessel_heading;             // in 0.01° 0 to 35999
    u8  position_system_descriptor; // 1 to 254
    u8  num_of_bytes_in_input_data; // <=254
}
em_pos_datagram;


//! H(eading data) (Always 048h)
//! em_heading_datagram_struct's full struct: em_datagram_header + em_hea_datagram + N * em_hea_entry + em_datagram_end
typedef struct em_heading_datagram_struct
{
    // em_datagram_header _header;
    u16 num_of_entries;    // = N
    // em_datagram_end _end;
}
em_hea_datagram;

typedef struct em_heading_entry_struct
{
    u16 mseconds;       // Time in milliseconds since record start  0 to 65534
    u16 heading;        // in 0.01°  0 to 35999
}
em_hea_entry;


/**
  * @brief em_clock_datagram_struct
  */
typedef struct em_clock_datagram_struct
{
    u32 date;   // = year*10000 + month*100 + day (from external clock input) (Example: Feb 26, 1995 = 19950226)
    u32 time;   // since midnight in milliseconds (from external clock datagram) (Example: 08:12:51.234 = 29570234) 0 to 86399999
    u8 PPS_use; // use (active or not) (0 = inactive)
}
em_clock_datagram;

/**
 * @brief em_att_data
 */
typedef struct em_PU_status_output_struct
{
    u16 ping_rate; //— 2U 0 – 3000 — Ping rate in centiHz
    u16 latest_ping_counter;    //— 2U 0 – 65535 — Ping counter of latest ping
    u32 swath_dis; //2) 4U 0 – 255 10 Distance between swath in 10%
    u32 UDP_port_2; //3) — 4U — Sensor input status, UDP port 2
    u32 serial_port_1; //3) — 4U — Sensor input status, serial port 1
    u32 serial_port_2; //3) — 4U — Sensor input status, serial port 2
    u32 serial_port_3; //3) — 4U — Sensor input status, serial port 3
    u32 serial_port_4; //3) — 4U — Sensor input status, serial port 4
    s8 PPS_status; // 4) — 1S — PPS status
    s8 position_status; //4) — 1S — Position status
    s8 attitude_status; //4) — 1S — Attitude status
    s8 clock_status; //4) — 1S — Clock status
    s8 heading_status; // Heading status
    u8 PU_status; // PU status
    u16 last_received_heading; // 0 – 35999 — Last received heading in 0.01°
    s16 last_received_roll; // -18000 – 18000  Last received roll in 0.01°
    s16 last_received_pitch; // -18000 – 18000  Last received pitch in 0.01°
    s16 last_received_heave; // at sonar head in cm  -999 – 999
    u16 sound_speed; // 14000 – 16000 — Sound speed at transducer dm/s
    u32 last_received_depth; // 0 – — Last received depth in cm
    s16 along_ship_velocity; //— — 2S — Along-ship velocity in 0.01 m/s
    u8 attitude_velocity_sensor_status; //12) — 1U Attitude velocity sensor status 0x81
    u8 mammal_protection_ramp; // 13) — 1U — Mammal protection ramp
    s8 backscatter_at_Oblique_angle; //7) — 1S -30 Backscatter at Oblique angle in dB
    s8 backscatter_at_normal_incidence; //7) — 1S Backscatter at normal incidence in dB -20
    s8 fixed_gain; //7) — 1S 18 Fixed gain in d
    u8 depth_to_normal_incidence; // in m 27
    u16 range_to_normal_incidence; // in m 289
    u8 port_coverage; // in degrees; // 7), 9) — 1U — Port Coverage in degrees
    u8 starboard_coverage; //7), 9) — 1U — Starboard Coverage in degrees
    u16 sound_speed_at_transducer_found_from_profie; // in dm/s 9) 2U 14000 – 16000 —
    s16 yaw_stabilization_angle; // or tilt used at 3D scanning, in centideg.  9) — 2S —
    s16 port_coverage_or_across_ship_velocity; // in degrees or Across-ship velocity in 0.01 m/s
    s16 starboard_coverage_or_downward_velocity ; // in degrees or Downward velocity in 0.01 m/
    s8 EM2040_CPU_temp; // 0 if not used 1S EM2040 CPU temp in °C
}
em_pu_status;


typedef struct em_ExtraPara2_struct{
    int active_positioning_system; // 0 – 2
    int quality_factor_setting[3];      // for pos. system 1, 2, 3.
                                        // 1=PU decodes Q-factor. Default
                                        // 0=External PU decode
    int N[3];                           // Number of quality factors for pos. system 1, 2, 3
}
em_ep2;

struct em_ep2_entry {
    int quality_factor; // 0 – m
    int limit;          // 0 cm default = not used
};


/////////////////////////////////////////////////////////

typedef std::vector<em_att_entry> em_att_data;

typedef struct em_att_packet_struct {
    em_datagram_header att_header;
    em_att_datagram att_num;
    em_att_data att_data;
}
em_att_pak;

typedef struct em_pos_packet_struct {
    em_datagram_header pos_header;
    em_pos_datagram pos_data;
} em_pos_pak;

typedef std::vector<em_dep_entry> em_pin_data;

typedef struct em_depth_packet_struct {
    em_datagram_header pin_header;
    em_dep_datagram pin_info;
    em_pin_data ping_data;
} em_depth_pak;

//! snippet data
typedef std::vector<em_img_entry> em_sni_data;
typedef std::vector<u8>           em_sni_amp;
typedef struct em_snippet_packet_struct {
    em_datagram_header sni_header;
    em_img_datagram sni_info;
    em_sni_data sni_data;
    em_sni_amp  sni_amp;
} em_sni_pak;

//! svp data
typedef struct em_ssp_pack_struct {
    em_datagram_header ssp_header;
    em_ssp_datagram ssp_info;
    std::vector<em_ssp_entry> ssp_data;
} em_ssp_pak;

#pragma pack()
#endif // EM_DATAGRAM_H
