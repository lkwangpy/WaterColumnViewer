#ifndef EM_PARSER_H
#define EM_PARSER_H


#include "raw_parser.h"
#include "em_datagram.h"
#include "types.h"

typedef std::vector<long long> pak_file_offsets;

class EM_Parser : public Raw_Parser
{
public:
    EM_Parser(std::string file_path = "");

    bool pre_parse();
    bool pre_parse_wci_packet();

    virtual bool parse();
    virtual std::string basicInfo_toHtml();
    virtual mb_swa to_common_swath();

    int find_type_id(u8 type)
    {
        for( size_t i = 0; i < _types.size(); i++ ){
            if (type == _types[i] ) return i;
        }
        return -1;
    }

    int find_type_num(u8 type)
    {
        int id = find_type_id(type);
        if (id > 0) return _types_nums[id];
        return 0;
    }

    em_wci_pak get_wci_packet(long long offset);
    em_wci_pak get_full_wci(size_t index);
    bool get_wci_center_beam(size_t index, em_wcb& beam, em_wc_datagram& info);

    em_pos_pak get_nav_packet(long long offset);
    NavData get_navigation();

public:
    bool b_little_endian;
    FILE *_file;

    std::vector<em_sni_pak> d_sni_packets;

    long long d_file_offset;
    std::vector<long long> d_file_offsets;
//private:
    std::string d_file_path;
    u16 d_em_modle;

    std::vector<em_att_pak> d_att_packets;    
    std::vector<em_pos_pak> d_pos_packets;
    std::vector<em_depth_pak> d_dep_packets;
    std::vector<em_wci_pak> d_wci_packets;

    em_ssp_pak d_ssp;
    std::vector<em_xyz88_pak> d_xyz88_paks;
    std::vector<em_img89_pak> d_img89_paks;
    std::vector<em_rrba78_pak> d_rrba78_paks;

    //!
    std::vector<u8> _types;
    std::vector<u32> _types_nums;
    std::vector<pak_file_offsets> _type_pak_file_offsets;
    std::vector<u8> _type_one_by_one;
    std::vector<int> d_wc_pak_id;
};

inline std::string find_type_name(u8 type)
{
    for( int i = 0; i < EM_TYRES_NUM; i++ ){
        if (type == G_EM_TYRES_STR[i].first ) return G_EM_TYRES_STR[i].second;
    }
    return "";
}

#endif // EM_PARSER_H
