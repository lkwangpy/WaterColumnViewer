#include "em_parser.h"
#include "stdio.h"
#include <WinSock2.h>
#include <vector>
#include <sstream>
#include <fstream>
//#include <gsl/gsl_spline.h>
#include <cmath>

 #include <QtGlobal>

EM_Parser::EM_Parser(std::string file_path) : b_little_endian(false), _file(NULL)
{
    d_file_path = file_path;
}

bool EM_Parser::parse()
{
    FILE *_file;

    if ( (_file = fopen(d_file_path.c_str(), "r+b")) == NULL) {
        return false;
    }
    else {

//        std::ofstream _log("log.txt");

        int _packet_count = 0;
        std::vector<u8> _types;
        std::vector<u32> _types_nums;
        std::vector<u8> _type_one_by_one;
        long long _all_byes = 0;

        em_datagram_header _header;
        bool _little_endian;

        fread(&_header, sizeof(em_datagram_header), 1, _file);
        if (_header.date > 19900000 && _header.date < 30000000) {
            _little_endian = true;
            d_em_modle = _header.em_model_number;
        }
        else {
            _little_endian = false;
            d_em_modle = htons(_header.em_model_number);
        }

        fseek(_file, 0, SEEK_SET);

//        while (!feof(_file))
        while ( fread(&_header, sizeof(em_datagram_header), 1, _file) != 0)
        {
//            size_t _hehe = fread(&_header, sizeof(em_datagram_header), 1, _file);
            if (!_little_endian) {
                //! big endian to little endian
                _header.number_of_bytes = htonl(_header.number_of_bytes);
                _header.date = htonl(_header.date);
                _header.em_model_number = htons(_header.em_model_number);
                _header.millsecond = htonl(_header.millsecond);
                _header.start_id = (_header.start_id);
                _header.type_of_datagram = (_header.type_of_datagram);
                _header.sequential_counter = htons(_header.sequential_counter);
                _header.serial_number = htons(_header.serial_number);
            }

            _packet_count ++;
            _all_byes += (_header.number_of_bytes + 4);

            if (_header.start_id != EM3_START_BYTE) {
                printf("error %d", _packet_count);

                fseek(_file, -sizeof(em_datagram_header), SEEK_CUR);

                char test[10];
                fread(test, 1, 10, _file);
                printf(test);
            }

            _type_one_by_one.push_back(_header.type_of_datagram);
            bool _b_type_existed = false;
            for (size_t i = 0; i < _types.size(); i++) {
                if (_header.type_of_datagram == _types[i]) {
                    _b_type_existed = true;
                    _types_nums[i] ++;
                }
            }
            if (!_b_type_existed) {
                _types.push_back(_header.type_of_datagram);
                _types_nums.push_back(1);
            }

            switch (_header.type_of_datagram)
            {
            case EM_Install_Parameters_Start:                
            case EM_Install_Parameters_Sop:
            {
                em_ins_datagram _ins;
                fread(&_ins, sizeof(em_ins_datagram), 1, _file);
                if (!_little_endian)
                    _ins.sec_serial_num = htons(_ins.sec_serial_num);

                int _n = _header.number_of_bytes - (sizeof(em_datagram_header) - sizeof(u32)) - sizeof(em_datagram_end) - sizeof(u16);
                char *_ascii = new char[_n];

                fread(_ascii, sizeof(char), _n, _file);

//                printf("%s\n", _ascii);

//                fseek(_file, _header.number_of_bytes
//                      - sizeof(em_datagram_header) + sizeof(u32), SEEK_CUR);
                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                break;
            }
            case EM_Sound_Velocity_Profile:
            {
                em_svp_datagram _svp;
                fread(&_svp, sizeof(em_svp_datagram), 1, _file);

                if (!_little_endian) {
                    //! big endian to little endian
                    _svp.date = htonl(_svp.date);
                    _svp.time = htonl(_svp.time);
                    _svp.number_of_entries = htons(_svp.number_of_entries);
                    _svp.depth_resolution = htons(_svp.depth_resolution);
                }

                std::vector<em_svp_entry> _svp_entries;
                _svp_entries.resize(_svp.number_of_entries);
                for (int i = 0; i < _svp.number_of_entries; i++) {
                    em_svp_entry _entry;
                    fread(&_entry, sizeof(em_svp_entry), 1, _file);
                    if (!_little_endian) {
                        _entry.depth = htons(_entry.depth);
                        _entry.sound_velocity = htons(_entry.sound_velocity);
                    }
                    _svp_entries[i] = _entry;
                }

                //! convert old svp to newer ssp
                std::vector<em_ssp_entry> _ssp(_svp_entries.size());
                for (size_t i = 0; i < _svp_entries.size(); i++) {
                    _ssp[i].depth = _svp_entries[i].depth * 10;
                    _ssp[i].sound_velocity = _svp_entries[i].sound_velocity;
                }

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                d_ssp.ssp_header = _header;
                d_ssp.ssp_info = _svp;
                d_ssp.ssp_data = _ssp;

                break;
            }
            case EM_Sound_Speed_Profile_Datagram:
            {
                em_ssp_datagram _ssp;
                fread(&_ssp, sizeof(em_svp_datagram), 1, _file);

                if (!_little_endian) {
                    //! big endian to little endian
                    _ssp.date = htonl(_ssp.date);
                    _ssp.time = htonl(_ssp.time);
                    _ssp.number_of_entries = htons(_ssp.number_of_entries);
                    _ssp.depth_resolution = htons(_ssp.depth_resolution);
                }

                u16 _N = _ssp.number_of_entries;
                em_ssp_entry* _entries = new em_ssp_entry[_N];
                fread(_entries, sizeof(em_ssp_entry), _N, _file);

                d_ssp.ssp_header = _header;
                d_ssp.ssp_info = _ssp;
                d_ssp.ssp_data = std::vector<em_ssp_entry>(_entries, _entries + _N);

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                break;
            }
            case EM_Runtime_Parameters:
            {
                em_rtp_datagram _rtp;
                fread(&_rtp, sizeof(em_rtp_datagram), 1, _file);
                if (!_little_endian) {
                    _rtp.minimum_depth = htons(_rtp.minimum_depth);
                    _rtp.maximum_depth = htons(_rtp.maximum_depth);
                    _rtp.absorption_coefficient = htons(_rtp.absorption_coefficient);
                    _rtp.transmit_pulse_length = htons(_rtp.transmit_pulse_length);
                    _rtp.transmit_beamwidth = htons(_rtp.transmit_beamwidth);
                    _rtp.maximum_port_swath_width = htons(_rtp.maximum_port_swath_width);
                    _rtp.maximum_starboard_swath_width = htons(_rtp.maximum_starboard_swath_width);
                }
                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                break;
            }
            case EM_Depth_Datagram:
            {
                em_dep_datagram _depth;
                fread(&_depth, sizeof(em_dep_datagram), 1, _file);
                if (!_little_endian) {
                    _depth.heading = htons(_depth.heading);
                    _depth.sound_speed = htons(_depth.sound_speed);
                    _depth.transducer_depth_re = htons(_depth.transducer_depth_re);
                    _depth.sampling_rate = htons(_depth.sampling_rate);
                }

                u8 _N = _depth.valid_beam_num;
                std::vector<em_dep_entry> _dep_entries(_N);

                for (size_t i = 0; i < _N; i++) {
                    fread(&_dep_entries[i], sizeof(em_dep_entry), 1, _file);
                    if (!_little_endian) {
                        _dep_entries[i].depth = htons(_dep_entries[i].depth);
                        _dep_entries[i].acrosstrack_distance = htons(_dep_entries[i].acrosstrack_distance);
                        _dep_entries[i].alongtrack_distance = htons(_dep_entries[i].alongtrack_distance);
                        _dep_entries[i].beam_depression_angle = htons(_dep_entries[i].beam_depression_angle);
                        _dep_entries[i].beam_azimuth_angle = htons(_dep_entries[i].beam_azimuth_angle);
                        _dep_entries[i].range = htons(_dep_entries[i].range);
                    }
                }

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

//                __headers.push_back(_header);
//                __deps.push_back(_depth);

                em_depth_pak _pak;
                _pak.pin_header = _header;
                _pak.pin_info = _depth;
                _pak.ping_data = _dep_entries;
                d_dep_packets.push_back(_pak);

                break;
            }
            case EM_XYZ_88:
            {
                em_xyz88_datagram _xyz88;
                fread(&_xyz88, sizeof(em_xyz88_datagram), 1, _file);

                em_xyz88_entry* _entries = new em_xyz88_entry[_xyz88.N];
                fread(_entries, sizeof(em_xyz88_entry), _xyz88.N, _file);

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                em_xyz88_pak _pak;
                _pak.xyz88_header = _header;
                _pak.xyz88_info = _xyz88;
                _pak.xyz88_data = std::vector<em_xyz88_entry>(_entries, _entries + _xyz88.N);
                d_xyz88_paks.push_back(_pak);

                delete[] _entries;
                break;
            }
            case EM_Seabed_Image_Data_89:
            {
                em_img89_datagram _img89;
                fread(&_img89, sizeof(em_img89_datagram), 1, _file);

                em_img89_entry* _img_entries = new em_img89_entry[_img89.N];
                fread(_img_entries, sizeof(em_img89_entry), _img89.N, _file);

                int _sum_Ns = 0;
                for (size_t i = 0; i < _img89.N; i++) _sum_Ns += _img_entries[i].Ns;
                s16* sample_amplitudes = new s16[_sum_Ns];
                fread(sample_amplitudes, sizeof(s16), _sum_Ns, _file);

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                em_img89_pak _pak;
                _pak.img89_header = _header;
                _pak.img89_info = _img89;
                _pak.img89_data = std::vector<em_img89_entry>(_img_entries, _img_entries + _img89.N);
                _pak.img89_amps = std::vector<s16>(sample_amplitudes, sample_amplitudes + _sum_Ns);
                for (size_t j = 0; j < _pak.img89_amps.size(); j++)
                    _pak.img89_amps[j] = 256 + _pak.img89_amps[j];

                d_img89_paks.push_back(_pak);

                delete[] _img_entries;
                delete[] sample_amplitudes;
                break;
            }
            case EM_Raw_Range_and_Beam_Angle_f:
            {
                em_rrba_datagram _rrba;
                fread(&_rrba, sizeof(em_rrba_datagram), 1, _file);

                em_rrba_transmit_entry* _transmit_entries = new em_rrba_transmit_entry[_rrba.Ntx];
                fread(_transmit_entries, sizeof(em_rrba_transmit_entry), _rrba.Ntx, _file);

                em_rrba_beam_entry* _beam_entries = new em_rrba_beam_entry[_rrba.N];
                fread(_beam_entries, sizeof(em_rrba_beam_entry), _rrba.N, _file);

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                em_rrba_pak _pak;
                _pak.rrba_header= _header;
                _pak.rrba_info = _rrba;
                _pak.rrba_tx = std::vector<em_rrba_transmit_entry>(_transmit_entries, _transmit_entries + _rrba.Ntx);
                _pak.rrba_beams = std::vector<em_rrba_beam_entry>(_beam_entries, _beam_entries + _rrba.N);

                break;
            }
            case EM_Raw_Range_and_Angle_78:
            {
                em_rrba78_datagram _rrba;
                fread(&_rrba, sizeof(em_rrba78_datagram), 1, _file);

                em_rrba78_transmit_entry* _transmit_entries = new em_rrba78_transmit_entry[_rrba.Ntx];
                fread(_transmit_entries, sizeof(em_rrba78_transmit_entry), _rrba.Ntx, _file);

                em_rrba78_beam_entry* _beam_entries = new em_rrba78_beam_entry[_rrba.Nrx];
                fread(_beam_entries, sizeof(em_rrba78_beam_entry), _rrba.Nrx, _file);

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                em_rrba78_pak _pak;
                _pak.rrba78_header = _header;
                _pak.rrba78_info = _rrba;
                _pak.rrba_tx = std::vector<em_rrba78_transmit_entry>(_transmit_entries, _transmit_entries + _rrba.Ntx);
                _pak.rrba_data = std::vector<em_rrba78_beam_entry>(_beam_entries, _beam_entries + _rrba.Nrx);
                d_rrba78_paks.push_back(_pak);

                delete[] _transmit_entries;
                delete[] _beam_entries;
                break;
            }
            case EM_Seabed_image_datagram:
            {
                em_img_datagram _img;
                fread(&_img, sizeof(em_img_datagram), 1, _file);
                if (!_little_endian) {
                    _img.mean_absorption_coefficient = htons(_img.mean_absorption_coefficient);
                    _img.pulse_length = htons(_img.pulse_length);
                    _img.range = htons(_img.range);
                    _img.start_range_sample = htons(_img.start_range_sample);
                    _img.stop_range_sample = htons(_img.stop_range_sample);
                    _img.tx_beamwidth = htons(_img.tx_beamwidth);
                }

                u8 _N = _img.valid_beams_num;
                std::vector<em_img_entry> _img_entries(_N);
                for (size_t i = 0; i < _N; i++) {
                    fread(&_img_entries[i], sizeof(em_img_entry), 1, _file);
                    if (!_little_endian) {
                        _img_entries[i].samples_num = htons(_img_entries[i].samples_num);
                        _img_entries[i].centre_sample_num = htons(_img_entries[i].centre_sample_num);
                    }
                }

                int _sum_Ns = 0;
//                s8 sample_amplitudes; // in 0.5 dB
                for (size_t i = 0; i < _N; i++) {
                    u16 _Ns = _img_entries[i].samples_num;
                    _sum_Ns += _Ns;
//                    for (size_t j = 0; j < _Ns; j++) {
//                        fread(&sample_amplitudes, sizeof(s8), 1, _file);
//                    }
                }
                s8* sample_amplitudes = new s8[_sum_Ns];
                fread(sample_amplitudes, sizeof(s8), _sum_Ns, _file);
//                std::vector<s8> _vsample(sample_amplitudes, sample_amplitudes + _sum_Ns);
                std::vector<u8> _vsample_amplitudes(_sum_Ns);
                //! according to documents
                for (int j = 0; j < _sum_Ns; j++) {
                    _vsample_amplitudes[j] = 256 + sample_amplitudes[j] * 2;
//                    _log << (int)_vsample_smplitudes[j] << '\t';
                }
//                _log << '\n';

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                //! a little strange
                if (_sum_Ns % 2 != 0) fseek(_file, 1, SEEK_CUR);
                /*
                int _byte_left =
                        _header.number_of_bytes - (sizeof(em_datagram_header) - 4)
                        - sizeof(em_img_datagram) - _N * sizeof(em_img_entry)
                        - _sum_Ns * sizeof(s8) - sizeof(em_datagram_end);

                if (_byte_left > 0)
                    fseek(file, _byte_left, SEEK_CUR);
                */

                em_sni_pak _sni;
                _sni.sni_header = _header;
                _sni.sni_info = _img;
                _sni.sni_data = _img_entries;
                _sni.sni_amp = _vsample_amplitudes;
                d_sni_packets.push_back(_sni);

                break;
            }
            case EM_Water_Column_Datagram:
            {
                em_wci_pak _wc_pak;
                _wc_pak.h = _header;

                em_wc_datagram _wc;
                fread(&_wc, sizeof(em_wc_datagram), 1, _file);
                _wc_pak.wc= _wc;

                em_wctx_entry* _tx_entries = new em_wctx_entry[_wc.Ntx];
                fread(_tx_entries, sizeof(em_wctx_entry), _wc.Ntx, _file);
//                std::vector<em_wctx_entry> _v_tx(_tx_entries, _tx_entries + _wc.Ntx);
                _wc_pak.wctxs = std::vector<em_wctx_entry>(_tx_entries, _tx_entries + _wc.Ntx);;

//                std::vector<em_wcb_entry> _v_wcb;
                u32 _sum_bytes = 0;
                for (u16 i = 0; i < _wc.Nrx; i++) {
                    em_wcb_entry _wcb_entry;
                    fread(&_wcb_entry, sizeof(em_wcb_entry), 1, _file);
                    s8* amplitudes = new s8[_wcb_entry.Ns];
                    fread(amplitudes, sizeof(s8), _wcb_entry.Ns, _file);

                    _sum_bytes += sizeof(em_wcb_entry) + sizeof(s8) * _wcb_entry.Ns;

                    em_wcb _wcb;
                    _wcb.wcb = _wcb_entry;
                    _wcb.amps = std::vector<s8>(amplitudes, amplitudes + _wcb_entry.Ns);
                    _wc_pak.wcbs.push_back(_wcb);
                }
                int _pad_bytes = _header.number_of_bytes
                        - sizeof(em_wc_datagram)
                        - _wc.Ntx * sizeof(em_wctx_entry)
                        - _sum_bytes
                        - sizeof(em_datagram_header) + sizeof(u32);
                if (_pad_bytes != 4) printf("wc _pad_bytes != 4, = %d", _pad_bytes);
                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                d_wci_packets.push_back(_wc_pak);
                break;
            }
            case EM_Attitude_Datagram:
            {
                em_att_pak _att;
                _att.att_header = _header;
//                em_att_datagram _att;
//                fread(&_att, sizeof(em_att_datagram), 1, _file);
//                _att.num_of_entries = htons(_att.num_of_entries);
                fread(&_att.att_num, sizeof(em_att_datagram), 1, _file);
                if (!_little_endian)
                    _att.att_num.num_of_entries = htons(_att.att_num.num_of_entries);

//                u16 _N = _att.num_of_entries;
                u16 _N = _att.att_num.num_of_entries;
                std::vector<em_att_entry> _att_entries(_N);
                for (size_t i = 0; i < _N; i++) {
                    fread(&_att_entries[i], sizeof(em_att_entry), 1, _file);
                    if (!_little_endian) {
                        _att_entries[i].time = htons(_att_entries[i].time);
                        _att_entries[i].sensor_status = htons(_att_entries[i].sensor_status);
                        _att_entries[i].roll = htons(_att_entries[i].roll);
                        _att_entries[i].pitch = htons(_att_entries[i].pitch);
                        _att_entries[i].heave = htons(_att_entries[i].heave);
                        _att_entries[i].heading = htons(_att_entries[i].heading);
                    }
                }
                _att.att_data = _att_entries;

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);

                d_att_packets.push_back(_att);

                break;
            }
            case EM_Position_Datagrams:
            {
                em_pos_datagram _pos;
                fread(&_pos, sizeof(em_pos_datagram), 1, _file);
                if (!_little_endian) {
                    _pos.latitude = htonl(_pos.latitude);
                    _pos.longitude = htonl(_pos.longitude);
                    _pos.position_fix_quality = htons(_pos.position_fix_quality);
                    _pos.vessel_speed = htons(_pos.vessel_speed);
                    _pos.vessel_course = htons(_pos.vessel_course);
                    _pos.vessel_heading = htons(_pos.vessel_heading);
                }

                char* _input_data = new char[_pos.num_of_bytes_in_input_data];
                fread(_input_data, sizeof(char), _pos.num_of_bytes_in_input_data, _file);
//                printf(_input_data);
//                fseek(_file, _pos.num_of_bytes_in_input_data, SEEK_CUR);

                int _left_bytes =  4 + _header.number_of_bytes - sizeof(em_datagram_header)
                        - sizeof(em_pos_datagram)
                        - _pos.num_of_bytes_in_input_data;

                if (_left_bytes == 3)
                {
                    char _end[3];
                    fread(&_end, 1, 3, _file);
                }
                else {
                    em_datagram_end _end;
                    fread(&_end, sizeof(em_datagram_end), 1, _file);
                }

                em_pos_pak _pak;
                _pak.pos_header = _header;
                _pak.pos_data = _pos;
                d_pos_packets.push_back(_pak);

                break;
            }
            case EM_Clock_Datagram:
            {
                em_clock_datagram _clock;
                fread(&_clock, sizeof(em_clock_datagram), 1, _file);
                em_datagram_end2 _end;
                fread(&_end, sizeof(em_datagram_end2), 1, _file);
                break;
            }
            case EM_Heading_Datagram:
            {
                em_hea_datagram _heading;
                fread(&_heading, sizeof(em_hea_datagram), 1, _file);
                if (!_little_endian)
                    _heading.num_of_entries = htons(_heading.num_of_entries);

                u16 _N = _heading.num_of_entries;
                std::vector<em_hea_entry> _heading_entries(_N);
                for (size_t i = 0; i < _N; i++) {
                    fread(&_heading_entries[i], sizeof(em_hea_entry), 1, _file);
                    if (!_little_endian) {
                        _heading_entries[i].mseconds = htons(_heading_entries[i].mseconds);
                        _heading_entries[i].heading = htons(_heading_entries[i].heading);
                    }
                }

                em_datagram_end _end;
                fread(&_end, sizeof(em_datagram_end), 1, _file);
                break;
            }
            case EM_ExtraParameters_Datagram:
            {
                printf("EM_ExtraParameters_Datagram %d is not parsed\n", _header.type_of_datagram);
                fseek(_file, _header.number_of_bytes
                      - sizeof(em_datagram_header) + sizeof(u32), SEEK_CUR);
                break;

//                u16 _content_identifier;
//                fread(&_content_identifier, sizeof(u16), 1, _file);

//                int _sum_bytes = 0;
//                switch (_content_identifier) {
//                case Log_All_Heights: {
//                    em_ep2 _ep2;
//                    fread(&_ep2, sizeof(em_ep2), 1, _file);
//                    int _sum_N = _ep2.N[0] + _ep2.N[1] + _ep2.N[2];
//                    em_ep2_entry* _entires = new em_ep2_entry[_sum_N];
//                    fread(_entires, sizeof(em_ep2_entry), _sum_N, _file);
//                    _sum_bytes += sizeof(em_ep2) + _sum_N * sizeof(em_ep2_entry);
//                    break;
//                }
//                default:
//                    break;
//                }

//                if (_sum_bytes % 2 == 1) fseek(_file, 1, SEEK_CUR);
//                em_datagram_end _end;
//                fread(&_end, sizeof(em_datagram_end), 1, _file);
//                break;
            }
            case EM_PU_Status_output:
            {
                em_pu_status _pu_status;
                fread(&_pu_status, sizeof(em_pu_status), 1, _file);
                em_datagram_end2 _end;
                fread(&_end, sizeof(em_datagram_end2), 1, _file);
                int _pad_byte = _header.number_of_bytes
                        - sizeof(em_pu_status) - sizeof(em_datagram_end2)
                        - sizeof(em_datagram_header) + sizeof(u32);
                if (_pad_byte != 0) {
                    printf("EM_PU_Status_output _pad_byte != 0 = %d", _pad_byte);
                    fseek(_file, _pad_byte, SEEK_CUR);
                }
                break;
            }
            default:
            {
                printf("undefined datagram %d\n", _header.type_of_datagram);
                fseek(_file, _header.number_of_bytes
                      - sizeof(em_datagram_header) + sizeof(u32), SEEK_CUR);
                break;
            }
            }            
        }
//        _log.close();
//        printf ("log finishes!");
        size_t _n_pin = d_dep_packets.size();
        std::vector<float> _swaths_widthes(_n_pin);
        for (size_t i = 0; i < _n_pin; i++) {
            em_depth_pak &_p = d_dep_packets[i];
            if (_p.ping_data.empty()) {
                _swaths_widthes[i] = 0;
                continue;
            }
            em_dep_entry &_d_l = _p.ping_data[0];
            em_dep_entry &_d_r = _p.ping_data[_p.ping_data.size() - 1];
            _swaths_widthes[i] = (_d_r.acrosstrack_distance - _d_l.acrosstrack_distance) / 100.;
        }

        size_t _n_sni = d_sni_packets.size();
        std::vector<int> _sample_nums(_n_sni);
        for (size_t i = 0; i < _n_sni; i++)
            _sample_nums[i] = d_sni_packets[i].sni_amp.size();


//        std::ofstream _swath_width_f("_swaths_widthes.txt");
//        for (size_t i = 0; i < _n_pin; i++) _swath_width_f << _swaths_widthes[i] << '\t';
//        _swath_width_f << '\n';

//        std::ofstream _sample_nums_f("_sample_nums.txt");
//        for (size_t i = 0; i < _n_sni; i++) _sample_nums_f << _sample_nums[i] << '\t';
//        _sample_nums_f << '\n';

        return true;
    }
}

NavData EM_Parser::get_navigation()
{
    int _wc_typeid = find_type_id(EM_Position_Datagrams);
    pak_file_offsets &_offsets = _type_pak_file_offsets[_wc_typeid];

    size_t n_pos = _offsets.size();
    NavData nav_data(n_pos);

    for (size_t i = 0; i < n_pos; i++) {
        em_pos_pak _pos = get_nav_packet(_offsets[i]);
        nav_data[i] = Point3d(
                    _pos.pos_data.longitude / 10000000.,
                    _pos.pos_data.latitude  / 20000000.,
                                0);
    }
    return nav_data;
}

em_wci_pak EM_Parser::get_full_wci(size_t index)
{
//    if (d_wc_pak_id.empty()) return false;

    int _wc_typeid = find_type_id(EM_Water_Column_Datagram);
    pak_file_offsets &_offsets = _type_pak_file_offsets[_wc_typeid];

    int _pak_start_id = d_wc_pak_id[index];
    int _pak_end_id = -1;
    if (index >= d_wc_pak_id.size() - 1) _pak_end_id = _offsets.size() - 1;
    else _pak_end_id = d_wc_pak_id[index + 1] - 1;


    em_wci_pak _pak = get_wci_packet( _offsets[_pak_start_id] );

    for (int i = _pak_start_id + 1; i <= _pak_end_id; i++ ) {
        em_wci_pak _pakj = get_wci_packet( _offsets[i] );

        for (size_t ii = 0; ii < _pakj.wcbs.size(); ii++)
            _pak.wcbs.push_back(_pakj.wcbs[ii]);
    }
    return _pak;
}

em_pos_pak EM_Parser::get_nav_packet(long long offset)
{
    if ( !_file ) _file = fopen(d_file_path.c_str(), "r+b");

    fseek(_file, offset, SEEK_SET);

    em_datagram_header _header;
    if( fread(&_header, sizeof(em_datagram_header), 1, _file) == 0 )
        printf("fatal error in get_nav_packet");

    if (!b_little_endian) {
        //! big endian to little endian
        _header.number_of_bytes = htonl(_header.number_of_bytes);
        _header.date = htonl(_header.date);
        _header.em_model_number = htons(_header.em_model_number);
        _header.millsecond = htonl(_header.millsecond);
        _header.start_id = (_header.start_id);
        _header.type_of_datagram = (_header.type_of_datagram);
        _header.sequential_counter = htons(_header.sequential_counter);
        _header.serial_number = htons(_header.serial_number);
    }
    Q_ASSERT_X(_header.type_of_datagram == EM_Position_Datagrams, "get_nav_packet", "type_of_datagram error");

    em_pos_datagram _pos;
    fread(&_pos, sizeof(em_pos_datagram), 1, _file);
    if (!b_little_endian) {
        _pos.latitude = htonl(_pos.latitude);
        _pos.longitude = htonl(_pos.longitude);
        _pos.position_fix_quality = htons(_pos.position_fix_quality);
        _pos.vessel_speed = htons(_pos.vessel_speed);
        _pos.vessel_course = htons(_pos.vessel_course);
        _pos.vessel_heading = htons(_pos.vessel_heading);
    }

    char* _input_data = new char[_pos.num_of_bytes_in_input_data];
    fread(_input_data, sizeof(char), _pos.num_of_bytes_in_input_data, _file);

    int _left_bytes =  4 + _header.number_of_bytes - sizeof(em_datagram_header)
            - sizeof(em_pos_datagram)
            - _pos.num_of_bytes_in_input_data;

    if (_left_bytes == 3)
    {
        char _end[3];
        fread(&_end, 1, 3, _file);
    }
    else {
        em_datagram_end _end;
        fread(&_end, sizeof(em_datagram_end), 1, _file);
    }

    em_pos_pak _pak;
    _pak.pos_header = _header;
    _pak.pos_data = _pos;

    return _pak;
}

em_wci_pak EM_Parser::get_wci_packet(long long offset)
{
    em_wci_pak _wc_pak;
    if ( !_file ) _file = fopen(d_file_path.c_str(), "r+b");

    fseek(_file, offset, SEEK_SET);

    em_datagram_header _header;
    if( fread(&_header, sizeof(em_datagram_header), 1, _file) != 0 )
    {
        if (!b_little_endian) {
            //! big endian to little endian
            _header.number_of_bytes = htonl(_header.number_of_bytes);
            _header.date = htonl(_header.date);
            _header.em_model_number = htons(_header.em_model_number);
            _header.millsecond = htonl(_header.millsecond);
            _header.start_id = (_header.start_id);
            _header.type_of_datagram = (_header.type_of_datagram);
            _header.sequential_counter = htons(_header.sequential_counter);
            _header.serial_number = htons(_header.serial_number);
        }
        Q_ASSERT_X(_header.type_of_datagram == EM_Water_Column_Datagram, "get_wci_packet", "type_of_datagram error");

        em_wci_pak _wc_pak;
        _wc_pak.h = _header;

        em_wc_datagram _wc;
        fread(&_wc, sizeof(em_wc_datagram), 1, _file);
        _wc_pak.wc= _wc;

        em_wctx_entry* _tx_entries = new em_wctx_entry[_wc.Ntx];
        fread(_tx_entries, sizeof(em_wctx_entry), _wc.Ntx, _file);

        _wc_pak.wctxs = std::vector<em_wctx_entry>(_tx_entries, _tx_entries + _wc.Ntx);;

        u32 _sum_bytes = 0;
        for (u16 i = 0; i < _wc.Nrx; i++) {
            em_wcb_entry _wcb_entry;
            fread(&_wcb_entry, sizeof(em_wcb_entry), 1, _file);
            s8* amplitudes = new s8[_wcb_entry.Ns];
            fread(amplitudes, sizeof(s8), _wcb_entry.Ns, _file);

            _sum_bytes += sizeof(em_wcb_entry) + sizeof(s8) * _wcb_entry.Ns;

            em_wcb _wcb;
            _wcb.wcb = _wcb_entry;
            _wcb.amps = std::vector<s8>(amplitudes, amplitudes + _wcb_entry.Ns);
            _wc_pak.wcbs.push_back(_wcb);
        }
        int _pad_bytes = _header.number_of_bytes
                - sizeof(em_wc_datagram)
                - _wc.Ntx * sizeof(em_wctx_entry)
                - _sum_bytes
                - sizeof(em_datagram_header) + sizeof(u32);
        if (_pad_bytes != 4) printf("wc _pad_bytes != 4, = %d", _pad_bytes);
        em_datagram_end _end;
        fread(&_end, sizeof(em_datagram_end), 1, _file);

        return _wc_pak;
    }
    return _wc_pak;
}

bool EM_Parser::pre_parse()
{
    if ( (_file = fopen(d_file_path.c_str(), "r+b")) == NULL) {
        return false;
    }
    else {
        int _packet_count = 0;
        _types.clear();
        _types_nums.clear();
        _type_pak_file_offsets.clear();
        _type_one_by_one.clear();
        long long _all_byes = 0;

        em_datagram_header _header;

        fread(&_header, sizeof(em_datagram_header), 1, _file);
        if (_header.date > 19900000 && _header.date < 30000000) {
            b_little_endian = true;
            d_em_modle = _header.em_model_number;
        }
        else {
            b_little_endian = false;
            d_em_modle = htons(_header.em_model_number);
        }
        fseek(_file, 0, SEEK_SET);

        while ( fread(&_header, sizeof(em_datagram_header), 1, _file) != 0)
        {
            _packet_count ++;

            if (_header.start_id != EM3_START_BYTE) {
                printf("format error %d", _packet_count);
            }

            _type_one_by_one.push_back(_header.type_of_datagram);

            bool _b_type_existed = false;
            for (size_t i = 0; i < _types.size(); i++) {
                if (_header.type_of_datagram == _types[i]) {
                    _b_type_existed = true;
                    _types_nums[i] ++;
                    _type_pak_file_offsets[i].push_back(_all_byes);
                }
            }
            if (!_b_type_existed) {
                _types.push_back(_header.type_of_datagram);
                _types_nums.push_back(1);
                pak_file_offsets _offset;
                _offset.push_back(_all_byes);
                _type_pak_file_offsets.push_back(_offset);
            }

            fseek(_file, _header.number_of_bytes
                  - sizeof(em_datagram_header) + sizeof(u32), SEEK_CUR);
            _all_byes += (_header.number_of_bytes + 4);

        }
        return true;
    }
}

bool EM_Parser::pre_parse_wci_packet()
{
    if ( _file ) {

        int _id = find_type_id(EM_Water_Column_Datagram);
        if (_id < 0) return false;

        pak_file_offsets _offsets = _type_pak_file_offsets[_id];

        d_wc_pak_id.clear();

        for ( size_t i = 0; i < _offsets.size(); i++ ) {
            em_datagram_header _header;
            fseek(_file, _offsets[i], SEEK_SET);
            if (fread(&_header, sizeof(em_datagram_header), 1, _file) != 0) {
                em_wc_datagram _wc;
                fread(&_wc, sizeof(em_wc_datagram), 1, _file);
                d_wc_pak_id.push_back(i);
                i += _wc.Nd - 1;
            }
        }
        return true;
    }
    return false;
}

bool EM_Parser::get_wci_center_beam(size_t index, em_wcb& beam, em_wc_datagram& info)
{
    if (d_wc_pak_id.empty()) return false;

    em_wci_pak _pak = get_full_wci(index);

    const std::vector<em_wcb> &_wcbs = _pak.wcbs;

    for (size_t ii = 0; ii < _wcbs.size(); ii++ ) {
        const em_wcb& _wcbe = _wcbs[ii];
        if (ii == int(_wcbs.size() / 2. + 0.5)) {
            beam = _wcbe;
            info = _pak.wc;
            return true;
        }
    }

    return false;
}

std::string EM_Parser::basicInfo_toHtml()
{
//    int _n_att_data = 0;
//    for (size_t i = 0; i < d_att_packets.size(); i++)
//        _n_att_data +=  d_att_packets[i].att_num.num_of_entries;


    std::ostringstream _ostr;
    _ostr << "<body><p>"
          << "EM Raw Data" << "<br/>"
          << "Model: EM " << d_em_modle << "<br/>";

    for (size_t i = 0; i < _types.size(); i++) {
        _ostr << find_type_name(_types[i]) << ": " << _types_nums[i] << "<br/>";
    }
//          << "Attitude packet: " << d_att_packets.size() << " Containning data:" << _n_att_data << "<br/>"
//          << "Depth (Bathy) packets " << d_dep_packets.size() + d_xyz88_paks.size() << "<br/>"
//          << "Image (Snippet) packets: " <<d_sni_packets.size() + d_img89_paks.size() << "<br/>"
//          << "Postition packet: " << d_pos_packets.size() << "<br/>"
//          << "water column data: " << d_wci_packets.size() << "<br/>"
    _ostr << "</p>"
          << "</body>" << std::endl;
    return _ostr.str();
}

mb_swa EM_Parser::to_common_swath()
{
    mb_swa _swath;

    size_t _n_att = d_att_packets.size();
    size_t _n_pos = d_pos_packets.size();
    size_t _n_pin = d_dep_packets.size() + d_xyz88_paks.size();

    if (_n_pos == 0) return _swath;

    _swath.atts.resize(0);
    _swath.poss.resize(_n_pos);
    _swath.pins.resize(_n_pin);

    for (size_t i = 0; i < _n_att; i++)
    {
        em_att_pak &_att_pak = d_att_packets[i];
        size_t _n_att_num = _att_pak.att_num.num_of_entries;

        for (size_t j = 0; j <_n_att_num; j++)
        {
            mb_att _att;
            em_att_entry _entry = _att_pak.att_data[j];

//        _att.timeTag = _att0.att_header.TimeTag;
            _att.pitch = _entry.pitch / 100.;
            _att.roll = _entry.roll / 100.;
            _att.heave = _entry.heave / 100.;
            _att.yaw = 0.0;
            _att.heading = _entry.heading / 100.;
            _att.timeTag = _att_pak.att_header.millsecond + _entry.time
                    - d_att_packets[0].att_header.millsecond;

            //! Date = year*10000 + month*100 + day; Feb 26,1995=19950226
            _att.time.year = _att_pak.att_header.date / 10000;
            _att.time.month = (_att_pak.att_header.date - _att.time.year *10000) / 100;
            _att.time.day = _att_pak.att_header.date % 100;
            _att.time.hour = (_att_pak.att_header.millsecond + _entry.time) / 3600000;
            _att.time.minutes = (_att_pak.att_header.millsecond + _entry.time - _att.time.hour * 3600000) / 60000;
            _att.time.seconds = (_att_pak.att_header.millsecond + _entry.time
                                 - _att.time.hour * 3600000 - _att.time.minutes * 60000) / 1000;
            _att.time.milliseconds = (_att_pak.att_header.millsecond + _entry.time) % 1000;

            _swath.atts.push_back(_att);
        }
    }


    //! convert position packets
    for (size_t i = 0; i < _n_pos; i++) {
        mb_pos &_pos = _swath.poss[i];
        em_pos_pak &_pos0 = d_pos_packets[i];

        _pos.latitude = _pos0.pos_data.latitude / 20000000.;
        _pos.longitude = _pos0.pos_data.longitude / 10000000.;
        _pos.timeTag = _pos0.pos_header.millsecond -
                d_pos_packets[0].pos_header.millsecond;

        _pos.time.year = _pos0.pos_header.date / 10000;
        _pos.time.month = (_pos0.pos_header.date - _pos.time.year *10000) / 100;
        _pos.time.day = _pos0.pos_header.date % 100;
        _pos.time.hour = _pos0.pos_header.millsecond / 3600000;
        _pos.time.minutes = (_pos0.pos_header.millsecond - _pos.time.hour * 3600000) / 60000;
        _pos.time.seconds = (_pos0.pos_header.millsecond
                             - _pos.time.hour * 3600000 - _pos.time.minutes * 60000) / 1000;
        _pos.time.milliseconds = _pos0.pos_header.millsecond % 1000;
    }

    size_t _n = _n_pos;
    double *_x = new double[_n];
    double *_y = new double[_n];
    double *_t = new double[_n];

    for (size_t i = 0; i < _n; i++) {
        mb_pos &_pos = _swath.poss[i];
        _x[i] = _pos.latitude;
        _y[i] = _pos.longitude;
        _t[i] = _pos.timeTag;
    }

//    gsl_interp_accel *_acc_x = gsl_interp_accel_alloc ();
//    gsl_interp_accel *_acc_y = gsl_interp_accel_alloc ();
//    gsl_spline *_spline_x = gsl_spline_alloc (gsl_interp_cspline, _n);
//    gsl_spline *_spline_y = gsl_spline_alloc (gsl_interp_cspline, _n);
//    gsl_spline_init (_spline_x, _t, _x, _n);
//    gsl_spline_init (_spline_y, _t, _y, _n);

    float _beam_width_degree;
    //! convert ping packets
    int _t0 = d_pos_packets[0].pos_header.millsecond;
    if (!d_dep_packets.empty()) {
        for (size_t i = 0; i < _n_pin; i++) {
            mb_pin &_ping = _swath.pins[i];
            em_depth_pak &_pak = d_dep_packets[i];

            _ping.time.year = _pak.pin_header.date / 10000;
            _ping.time.month = (_pak.pin_header.date - _ping.time.year *10000) / 100;
            _ping.time.day = _pak.pin_header.date % 100;
            _ping.time.hour = _pak.pin_header.millsecond / 3600000;
            _ping.time.minutes = (_pak.pin_header.millsecond - _ping.time.hour * 3600000) / 60000;
            _ping.time.seconds = (_pak.pin_header.millsecond
                                  - _ping.time.hour * 3600000 - _ping.time.minutes * 60000) / 1000;
            _ping.time.milliseconds = _pak.pin_header.millsecond % 1000;

            int _dt =  _pak.pin_header.millsecond - _t0;
            _dt = _dt < 0 ? 0 : _dt;
            _dt = _dt > _t[_n - 1] ? _t[_n - 1] : _dt;

            //! linear interpolation
            for (size_t ii = 0; ii < _n - 1; ii++) {
                if (_dt >= _t[ii] && _dt <= _t[ii + 1]) {
                    _ping.latitude = (_x[ii + 1] - _x[ii]) / (_t[ii + 1] - _t[ii]) * (_dt - _t[ii]) + _x[ii];
                    _ping.longitude = (_y[ii + 1] - _y[ii]) / (_t[ii + 1] - _t[ii]) * (_dt - _t[ii]) + _y[ii];
                    break;
                }
            }

            //        _ping.latitude  = gsl_spline_eval(_spline_x, _dt , _acc_x);
            //        _ping.longitude = gsl_spline_eval(_spline_y, _dt, _acc_y);

            _ping.heading = _pak.pin_info.heading / 100.;
            _ping.velocity = _pak.pin_info.sound_speed / 10.;
            _ping.max_beam_num = _pak.pin_info.max_beam_num;
            _ping.valid_beam_num = _pak.pin_info.valid_beam_num;
            _ping.ping_number = i + 1;

            _ping.tide = 0.;
            _ping.dynamic_draft = 0.;

            _ping.beams.resize(_ping.valid_beam_num);

            _beam_width_degree = 1.5;
            for (size_t i = 0; i < _ping.valid_beam_num; i++) {
                mb_bea &_b = _ping.beams[i];
                em_dep_entry &_b0 = _pak.ping_data[i];
                _b.beam_number = _b0.beam_number;
                _b.x = _b0.alongtrack_distance / 100.;
                _b.y = _b0.acrosstrack_distance / 100.;
                _b.z = _b0.depth / 100.;
                _b.range = _b0.range / 1000.; //ms-s
            }
        }
    }
    else if (!d_xyz88_paks.empty()) {
        for (size_t i = 0; i < _n_pin; i++) {
            mb_pin &_ping = _swath.pins[i];
            em_xyz88_pak &_pak = d_xyz88_paks[i];

            _ping.time.year = _pak.xyz88_header.date / 10000;
            _ping.time.month = (_pak.xyz88_header.date - _ping.time.year *10000) / 100;
            _ping.time.day = _pak.xyz88_header.date % 100;
            _ping.time.hour = _pak.xyz88_header.millsecond / 3600000;
            _ping.time.minutes = (_pak.xyz88_header.millsecond - _ping.time.hour * 3600000) / 60000;
            _ping.time.seconds = (_pak.xyz88_header.millsecond
                                  - _ping.time.hour * 3600000 - _ping.time.minutes * 60000) / 1000;
            _ping.time.milliseconds = _pak.xyz88_header.millsecond % 1000;

            int _dt =  _pak.xyz88_header.millsecond - _t0;
            _dt = _dt < 0 ? 0 : _dt;
            _dt = _dt > _t[_n - 1] ? _t[_n - 1] : _dt;

            //! linear interpolation
            for (size_t ii = 0; ii < _n - 1; ii++) {
                if (_dt >= _t[ii] && _dt <= _t[ii + 1]) {
                    _ping.latitude = (_x[ii + 1] - _x[ii]) / (_t[ii + 1] - _t[ii]) * (_dt - _t[ii]) + _x[ii];
                    _ping.longitude = (_y[ii + 1] - _y[ii]) / (_t[ii + 1] - _t[ii]) * (_dt - _t[ii]) + _y[ii];
                    break;
                }
            }

            //        _ping.latitude  = gsl_spline_eval(_spline_x, _dt , _acc_x);
            //        _ping.longitude = gsl_spline_eval(_spline_y, _dt, _acc_y);

            _ping.heading = _pak.xyz88_info.heading / 100.;
            _ping.velocity = _pak.xyz88_info.sound_speed / 10.;
            _ping.max_beam_num = _pak.xyz88_info.N;
            _ping.valid_beam_num = _pak.xyz88_info.valid_N;
            _ping.ping_number = i + 1;
            _ping.tt_depth = _pak.xyz88_info.tt_depth;

            _ping.tide = 0.;
            _ping.dynamic_draft = 0.;

            _ping.beams.resize(_pak.xyz88_data.size());

            _beam_width_degree = 1.5;
            for (size_t i = 0; i < _pak.xyz88_data.size(); i++) {
                mb_bea &_b = _ping.beams[i];
                em_xyz88_entry &_b0 = _pak.xyz88_data[i];
                _b.reflectivity = _b0.BS;
                _b.beam_number = i;
                _b.y = _b0.y;
                _b.x = _b0.x;
                _b.z = _b0.z;
            }
        }
    }
    //! swath info
    _swath.info.beam_width = _beam_width_degree;
    if (!_swath.pins.empty()) {
        _swath.info.max_beam = _swath.pins[0].max_beam_num;
        _swath.info.ping_start_time = _swath.pins[0].time;
        _swath.info.ping_end_time = _swath.pins[_swath.pins.size() - 1].time;
        char _em_model[10];
        itoa(d_em_modle, _em_model, 10);
        _swath.info.multibeam_model = _em_model;
    }

    //! snippet data
    size_t _n_sni = d_sni_packets.size() + d_img89_paks.size();
    _swath.snis.resize(_n_sni);
    if (!d_sni_packets.empty()) {
    for (size_t i = 0; i < _n_sni; i++) {
         mb_sni &_sni0 = _swath.snis[i];
         em_sni_pak& _sni1 = d_sni_packets[i];
         _sni0.mean_absorption_coefficient = _sni1.sni_info.mean_absorption_coefficient;
         _sni0.pulse_length = _sni1.sni_info.pulse_length;
         _sni0.range = _sni1.sni_info.range;
         _sni0.start_range_sample = _sni1.sni_info.start_range_sample;
         _sni0.stop_range_sample = _sni1.sni_info.stop_range_sample;
         _sni0.normal_incidence_BS = _sni1.sni_info.normal_incidence_BS;
         _sni0.oblique_BS = _sni1.sni_info.oblique_BS;
         _sni0.tx_beamwidth = _sni1.sni_info.tx_beamwidth;
         _sni0.TVG_law_crossover_angle = _sni1.sni_info.TVG_law_crossover_angle;
         _sni0.valid_beams_num = _sni1.sni_info.valid_beams_num;

         _sni0.backscatters.resize(_sni1.sni_data.size());

         int _sum_samples = 0;
         for (size_t j = 0; j < _sni0.backscatters.size(); j++) {
             mb_bscatter &_bs = _sni0.backscatters[j];
             em_img_entry &_img =  _sni1.sni_data[j];
             _bs.beam_index_number = _img.beam_index_number;
             _bs.sorting_direction = _img.sorting_direction;
             _bs.samples_num = _img.samples_num;
             _bs.centre_sample_num = _img.centre_sample_num;

             _bs.amplitudes.resize(_img.samples_num);
             for (size_t k = 0; k < _img.samples_num; k++)
             {
                 _bs.amplitudes[k] = _sni1.sni_amp[k + _sum_samples];
             }
             _sum_samples += _img.samples_num;
         }
    }
    }
    else if (!d_img89_paks.empty()) {
        for (size_t i = 0; i < _n_sni; i++) {
             mb_sni &_sni0 = _swath.snis[i];
             em_img89_pak& _sni1 = d_img89_paks[i];
             _sni0.range = _sni1.img89_info.range_to_normal_incidence;
             _sni0.normal_incidence_BS = _sni1.img89_info.BSN;
             _sni0.oblique_BS = _sni1.img89_info.BSO;
             _sni0.tx_beamwidth = _sni1.img89_info.tx_beamwidth;
             _sni0.TVG_law_crossover_angle = _sni1.img89_info.TVG_law_crossover_angle;
             _sni0.valid_beams_num = _sni1.img89_info.N;

             _sni0.backscatters.resize(_sni1.img89_data.size());

             int _sum_samples = 0;
             for (size_t j = 0; j < _sni0.backscatters.size(); j++) {
                 mb_bscatter &_bs = _sni0.backscatters[j];
                 em_img89_entry &_img =  _sni1.img89_data[j];
                 _bs.beam_index_number = j;
                 _bs.sorting_direction = _img.sorting_direction;
                 _bs.samples_num = _img.Ns;
                 _bs.centre_sample_num = _img.centre_sample_num;

                 _bs.amplitudes.resize(_img.Ns);
                 for (size_t k = 0; k < _img.Ns; k++)
                 {
                     _bs.amplitudes[k] = _sni1.img89_amps[k + _sum_samples];
                 }
                 _sum_samples += _img.Ns;
             }
        }
    }

//    gsl_spline_free (_spline_x);
//    gsl_spline_free (_spline_y);
//    gsl_interp_accel_free (_acc_x);
//    gsl_interp_accel_free (_acc_y);

    delete[] _x;
    delete[] _y;
    delete[] _t;

    return _swath;
}
