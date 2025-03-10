#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <malloc.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <bitset>
#include <windows.h>
#include "src/PassiveSocket.h"
#include <stdio.h>
#include "simplelogger.hpp"

using namespace std;

void delay(int msec)
{
    clock_t start_time = clock();
    while (clock() < start_time + msec)
    {
    }
}

//                 Decoding functions
//////////////////////////////////////////////////////////////

int decode_get_api_version(uint8_t *buf, size_t bytes_recieved) {
    /*
    Decoding response for get_api_revision
    One word is 2 bytes

    1st word: 0xAAAA (always)
    2nd word: 0X010C (always for get_api_revision)
    3rd word: Major Version (uint16_t)
    4th word: Minor Version (uint16_t)
    5th word: Patch Version (uint16_t)
    */

    // Check if buffer has at least 4 bytes for magic number and command
    if(bytes_recieved < 4)
    {
        cout << "Invalid buffer size" << endl;
        return -1;
    }

    // Extract magic number from buffer
    uint16_t magic;
    memcpy(&magic, buf, sizeof(uint16_t));

    // Check if magic number is correct
    if (magic != 0xAAAA) {
        cout << "Invalid magic number: " << std::hex << magic << endl;
        return -1;
    }

    // Extract command from buffer
    uint16_t cmd;
    memcpy(&cmd, buf + 2, sizeof(uint16_t));

    // Check if command is correct
    if (cmd != 0x010C) {
        cout << "Invalid command: " << std::hex << cmd << endl;
        return -1;
    }

    // Check if buffer has enough data for version numbers
    if(bytes_recieved < 10)
    {
        cout << "Wrong data size: " << bytes_recieved << endl;

        for(int i = 0; i < bytes_recieved; i++)
        {
            printf("0x%02X\n", (unsigned int)(buf[i]));
        }

        cout << endl;

        return -1;
    }

    // Extract version numbers from buffer
    uint16_t major_version, minor_version, patch_version;
    memcpy(&major_version, buf + 4, sizeof(uint16_t));
    memcpy(&minor_version, buf + 6, sizeof(uint16_t));
    memcpy(&patch_version, buf + 8, sizeof(uint16_t));

    // Print version
    cout << "API Version: " << major_version << "." << minor_version << "." << patch_version << endl;

    return 0;
}

int decode_get_gru_state(uint8_t *buf, size_t bytes_recieved) {
    /*
    Decoding response for get_board_state
    One word is 2 bytes

    1st word: 0xAAAA (always)
    2nd word: 0x0105 (always for get_gru_state)
    3rd word: State code (uint16_t)
    4th word: not used
    5th and 6th words: Error bits (uint32_t)
    7th and 8th words: Error point number (uint32_t)
    9th and 10th words: Current setting amp (int32_t)
    11th and 12th words: Current amp (int32_t)
    13th and 14th words: First sensor value (int32_t)
    15th and 16th words: Second sensor value (int32_t)
    17th word: Debug info (uint16_t)

    5-17 words have sense only if state code is error

    State codes:
    0 - GRU_STATE_INIT
    1 - GRU_STATE_INIT_FAILED
    2 - GRU_STATE_IDLE
    3 - GRU_STATE_SELFTEST
    4 - GRU_STATE_SELFTEST_FAILED
    5 - GRU_STATE_WAIT_TRAJECT
    6 - GRU_STATE_WAIT_SYNC
    7 - GRU_STATE_WORKING
    8 - GRU_STATE_WORK_FAILED

    Error bits:
    0x00000001 - OVERCURRENT_1
    0x00000002 - OVERCURRENT_2
    0x00000004 - CURRENT_SENSORS_MISMATCH
    0x00000008 - POWER_SWITCH_SW_FAILURE
    0x00000010 - WRONG_SIMULINK_MAGIC_NUMBER
    0x00000020 - DC_OK_1
    0x00000040 - POWER_SWITCH_HW_FAILURE
    0x00000080 - SENSOR1_TRIP_HI
    0x00000100 - SENSOR1_TRIP_LO
    0x00000200 - SENSOR2_TRIP_HI
    0x00000400 - SENSOR2_TRIP_LO
    0x00000800 - TEMPER_1
    0x00002000 - DC_OK_2
    0x00004000 - TEMPER_2
    */


    // Check if buffer is big enough
    if(bytes_recieved < 4) {
        cout << "Invalid buffer size (no magic)" << endl;
        return -1;
    }

    uint16_t magic;
    memcpy(&magic, buf, sizeof(uint16_t));

    // Check if magic correct
    if (magic != 0xAAAA) {
        cout << "Invalid magic number: " << std::hex << magic << endl;
        return -1;
    }

    uint16_t cmd;
    memcpy(&cmd, buf + 2, sizeof(uint16_t));

    // Check if command are correct
    if (cmd != 0x0105) {
        cout << "Invalid command: " << std::hex << cmd << endl;
        return -1;
    }

    // Check if buffer has enough data
    if(bytes_recieved < 6) {
        cout << "Wrong state data size: " << bytes_recieved << endl;
        return -1;
    }

    // Extract state code from buffer
    uint16_t state_code;
    memcpy(&state_code, buf + 4, sizeof(uint16_t));

    // Print state code
    cout << "State code " << state_code << ": ";

    // Decode and print state code description
    switch(state_code)
    {
        case 0:
            cout << "GRU_STATE_INIT" << endl;
            break;
        case 1:
            cout << "GRU_STATE_INIT_FAILED" << endl;
            break;
        case 2:
            cout << "GRU_STATE_IDLE" << endl;
            break;
        case 3:
            cout << "GRU_STATE_SELFTEST" << endl;
            break;
        case 4:
            cout << "GRU_STATE_SELFTEST_FAILED" << endl;
            break;
        case 5:
            cout << "GRU_STATE_WAIT_TRAJECT" << endl;
            break;
        case 6:
            cout << "GRU_STATE_WAIT_SYNC" << endl;
            break;
        case 7:
            cout << "GRU_STATE_WORKING" << endl;
            break;
        case 8:
            cout << "GRU_STATE_WORK_FAILED" << endl;
            break;
        default:
            cout << "Unknown state" << endl;
            break;
    }

    // If there are more than 6 bytes, extract error data
    uint32_t error_bits, error_point_number;
    int32_t current_setting_amp, current_amp, first_sensor_value, second_sensor_value;
    uint16_t debug_info;

    if(bytes_recieved > 6) {
        // Check if buffer has enough data for error information
        if(bytes_recieved < 34) {
            cout << "Wrong error data size: " <<  bytes_recieved << endl;
            return -1;
        }
        // Extract error data from buffer
        memcpy(&error_bits, buf + 8, sizeof(uint32_t));
        memcpy(&error_point_number, buf + 12, sizeof(uint32_t));
        memcpy(&current_setting_amp, buf + 16, sizeof(int32_t));
        memcpy(&current_amp, buf + 20, sizeof(int32_t));
        memcpy(&first_sensor_value, buf + 24, sizeof(int32_t));
        memcpy(&second_sensor_value, buf + 28, sizeof(int32_t));
        memcpy(&debug_info, buf + 32, sizeof(uint16_t));
    }
    else
    {
        // Initialize error data to zero if not enough bytes received
        error_bits = 0;
        error_point_number = 0;
        current_setting_amp = 0;
        current_amp = 0;
        first_sensor_value = 0;
        second_sensor_value = 0;
        debug_info = 0;
    }

    // If state code indicates an error, print error details
    if(state_code == 1 || state_code == 4 || state_code == 8)
    {
        cout << "Error bits: " << error_bits << endl;
        cout << "Error point number: " << error_point_number << endl;
        cout << "Current setting amp: " << current_setting_amp << endl;
        cout << "Current amp: " << current_amp << endl;
        cout << "First sensor value: " << first_sensor_value << endl;
        cout << "Second sensor value: " << second_sensor_value << endl;
        cout << "Debug info: " << debug_info << endl;

        return -1;
    }

    return 0;
}

int decode_get_sw_revision(uint8_t *buf, size_t bytes_recieved) {
    /*
    Decoding response for get_sw_revision
    One word is 2 bytes

    1st word: 0xAAAA (always)
    2nd word: 0x010D (always for get_sw_revision)
    Other words: Unicode string (wchar_t*)
    */

    // Check if buffer has at least 4 bytes for magic number and command
    if(bytes_recieved < 4) {
        cout << "Invalid buffer size" << endl;
        return -1;
    }

    // Extract magic number from buffer
    uint16_t magic;
    memcpy(&magic, buf, sizeof(uint16_t));

    // Check if magic number is correct
    if (magic != 0xAAAA) {
        cout << "Invalid magic number: " << std::hex << magic << endl;
        return -1;
    }

    // Extract command from buffer
    uint16_t cmd;
    memcpy(&cmd, buf + 2, sizeof(uint16_t));

    // Check if command is correct
    if (cmd != 0x010D) {
        cout << "Invalid command: " << std::hex << cmd << endl;
        return -1;
    }

    // Check if buffer has enough data and is even-sized
    if(bytes_recieved < 6 || bytes_recieved % 2 != 0) {
        cout << "Wrong data size: " << bytes_recieved << endl;
        return -1;
    }

    // Extract software revision string from buffer
    char16_t sw_revision[1024];
    memcpy(sw_revision, buf + 4, bytes_recieved - 4);
    sw_revision[(bytes_recieved - 4) / 2] = u'\0'; // Null-terminate the string

    // Print software revision string
    #ifdef _WIN32 // Convert to wide string on Windows
        wcout << L"Software revision: " << reinterpret_cast<wchar_t*>(sw_revision) << endl;
    #elif __linux__ // Using UTF-8 on Linux (converted by codecvt)
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        std::string converted_str = converter.to_bytes(sw_revision);
        cout << "Software revision: " << converted_str << endl;
    #endif

    return 0;
}

extern std::ostream out(std::cout.rdbuf());
SimpleLogger* newlogger;

///         functions declarations
//////////////////////////////////////////////////////////////
void get_API_version (CActiveSocket& SocketActive);

void get_sw_revision(CActiveSocket& SocketActive);

void get_gru_state(CActiveSocket& SocketActive);

void turn_ps_on(CActiveSocket& SocketActive);

void turn_ps_off(CActiveSocket& SocketActive);

void socket_close(CActiveSocket SocketActive);

vector <int32_t> get_unloaded_num(vector<int32_t> segment_status);

void upload_traj(CActiveSocket& SocketActive, vector<vector<int32_t>> nodes);

vector <int32_t> download_traject (CActiveSocket &SocketActive, int32_t points_cnt);

vector<vector<int32_t>> get_nodes(const string& Traject_file_name);

string hex_converting(int num);
//////////////////////////////////////////////////////////////


///         request functions
//////////////////////////////////////////////////////////////
namespace request
{
constexpr size_t MAX_PACKET{4096};
const int32_t NODES_PER_PACKET{200};          // Number of points for 1 package
const uint32_t PACKETS_WO_CONFIRM{1};        // How many packages could be sent without confirmation
const double CONFIRM_TIMEOUT_SEC{0.1};
const int32_t POINTS_PER_PACKET{500};

string hex_converting(int32_t num)
{
    stringstream mystream;
    mystream << hex << num;
    if(num < 0)
    {
        return mystream.str().substr(4);
    }
    else
    {
        return mystream.str();
    }
}

void ShowError(CSimpleSocket ss, string s)
{

    *(newlogger) << " " << s << " : " << " = " << ss.DescribeError() << '\n';
    *(newlogger) << " IsSocketValid() = " << ss.IsSocketValid() << '\n';
} //ShowError


void get_API_version (CActiveSocket& SocketActive){ //This function send API version request and shows response
    uint8 buf[MAX_PACKET] ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x0C);
    buf[3] = uint8(0x00);

    *(newlogger) << "GET API VERSION";
    *(newlogger) << "SocketActive.Send = " << SocketActive.Send(buf, 4) << '\n';
    ShowError(SocketActive, "SocketActive.Send");


    *(newlogger) << "listening..." << '\n';
    *(newlogger) << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

    /// @return number of bytes actually received.
    /// @return of zero means the connection has been shutdown on the other side.
    /// @return of -1 means that an error has occurred.

    stringstream ss;
    for(int32_t i=0; i<SocketActive.GetBytesReceived(); i++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
            ss << hex << buf[i];
        } //for

    *(newlogger) << "Bytes recieved: " << ss.str() << '\n';

    decode_get_api_version(buf,SocketActive.GetBytesReceived());
}// get_API_version



void get_sw_revision(CActiveSocket& SocketActive){ //This function send revision version request and shows response
    uint8 buf[MAX_PACKET] ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x0D);
    buf[3] = uint8(0x00);

    *(newlogger) << "GET SW REVISION";
    *(newlogger) << "SocketActive.Send = " << SocketActive.Send(buf, 4) << '\n';
    ShowError(SocketActive, "SocketActive.Send");


    *(newlogger) << "listening..." << '\n';
    *(newlogger) << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

    /// @return number of bytes actually received.
    /// @return of zero means the connection has been shutdown on the other side.
    /// @return of -1 means that an error has occurred.

    stringstream ss;
    for(int32_t ii=0; ii<SocketActive.GetBytesReceived(); ii++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
            ss << hex << buf[ii];
        } //for

    *(newlogger) << "Bytes Received : " << ss.str() << '\n' << endl;

    decode_get_sw_revision(buf, SocketActive.GetBytesReceived());
}//get_sw_revision

void get_gru_state(CActiveSocket& SocketActive){ //This function send request of state gradient amplifier and shows response
    uint8 buf[MAX_PACKET] ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x05);
    buf[3] = uint8(0x00);

    *(newlogger) << "GET GRU STATE";
    *(newlogger) << "SocketActive.Send = " << SocketActive.Send(buf, 4) << '\n';
    ShowError(SocketActive, "SocketActive.Send");


    *(newlogger) << "listening..." << '\n';
    *(newlogger) << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

    /// @return number of bytes actually received.
    /// @return of zero means the connection has been shutdown on the other side.
    /// @return of -1 means that an error has occurred.

    stringstream ss;
    for(int32_t ii=0; ii<SocketActive.GetBytesReceived(); ii++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
            ss << hex << buf[ii];
        } //for

    *(newlogger) << "Bytes Received : " << ss.str() << '\n' << endl;

    decode_get_gru_state(buf, SocketActive.GetBytesReceived());
} //get_gru_state

void turn_ps_on(CActiveSocket& SocketActive){
    uint8 buf[MAX_PACKET] ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x03);
    buf[3] = uint8(0x00);

    *(newlogger) << "TURN PS ON";
    *(newlogger) << "SocketActive.Send = " << SocketActive.Send(buf, 4) << '\n';
    ShowError(SocketActive, "SocketActive.Send");

    *(newlogger) << "listening..." << '\n';
    *(newlogger) << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

    stringstream ss;
    for(int32_t ii=0; ii<SocketActive.GetBytesReceived(); ii++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
            ss << hex << buf[ii];
        } //for

    *(newlogger) << "Bytes Received : " << ss.str() << '\n';
}

void turn_ps_off(CActiveSocket& SocketActive){
    uint8 buf[MAX_PACKET] ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x04);
    buf[3] = uint8(0x00);

    *(newlogger) << "TURN PS OFF";
    *(newlogger) << "SocketActive.Send = " << SocketActive.Send(buf, 4) << '\n';
    ShowError(SocketActive, "SocketActive.Send");

    *(newlogger) << "listening..." << '\n';
    *(newlogger) << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

    stringstream ss;
    for(int32_t ii=0; ii<SocketActive.GetBytesReceived(); ii++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
            ss << hex << buf[ii];
        } //for

    *(newlogger) << "Bytes Received : " << ss.str() << '\n';
}

void socket_close(CActiveSocket SocketActive){ // This function closes socket
    *(newlogger) << "SocketActive.Close() = " << SocketActive.Close() << '\n';
    ShowError(SocketActive, "SocketActive.Close");
    *(newlogger) << "closed" << '\n';
}

vector<vector<int32_t>> get_nodes(const string& Traject_file_name) {
    vector<vector<int32_t>> nodes;
    ifstream myfile(Traject_file_name);
    if (!myfile.is_open()) { // check if file is open
        *(newlogger) << LogPref::Flag(ERROR) << "Unable to open file\n";
    }
    int32_t num1, num2;
    while(myfile >> num1 >> num2) {
        nodes.push_back({num1, num2});
    }

    return nodes;
}//get nodes

vector <int32_t> get_unloaded_num(vector<int32_t> segment_status){
    vector <int32_t> res;
    for (uint32_t i = 0; i< segment_status.size();i++){
        if (segment_status[i] != 0){
            res.push_back(i);
        }
    }
    return res;

}//get_unloaded_num

//function to upload a segment of traject
void upload_segment(CActiveSocket &SocketActive, int32_t seg_num, bool need_confirm, vector<vector<int32_t>> nodes)
{
    int32_t counter=0;
    uint8 buf[MAX_PACKET]{0};
    string str1 = hex_converting(seg_num);
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x07);
    buf[3] = uint8(0x00);
    counter+=4;

    //data of segment traject
    need_confirm = true;
    if(need_confirm)
    {

        buf[4] = uint8(stoi(str1, nullptr, 16));
        buf[5] = uint8(0x80);
        counter+=2;

    }
    else{
        buf[4] = uint8(stoi(str1, nullptr, 16));
        counter++;
    }


    int32_t NULL32=0;
    int32_t NODES_SIZE = nodes.size();
    int32_t first_node_idx = max( seg_num * NODES_PER_PACKET - seg_num, NULL32 );
    int32_t last_node_idx = min( first_node_idx + NODES_PER_PACKET, NODES_SIZE) - 1;
    int32_t nodes_in_this_packet = last_node_idx - first_node_idx + 1;

    string str2 = hex_converting(nodes_in_this_packet);
    buf[counter++] = uint8(stoi(str2, nullptr, 16));
    buf[counter++] = uint8(0x00);

    for(int i = first_node_idx; i <last_node_idx+1; i++)
    {

        string hexString1 = hex_converting(nodes[i][0]);
        uint32_t tempcounter = counter;
        string temp1, temp2;

        for(int j = hexString1.length()-1; j > 0; j = j - 2)
        {
            buf[tempcounter++] = uint8(stoi(hexString1.substr(j-1,2), nullptr, 16) );
        }
        if(hexString1.length() % 2 != 0)
        {
            temp1 = hexString1[0];
            hexString1.erase(0);
            buf[tempcounter++] = uint8(stoi(temp1, nullptr, 16) );
        }

        counter+=4;

        uint32_t tempcounter2 = counter;

        string hexString2 = hex_converting(nodes[i][1]);

        for(int j = hexString2.length()-1; j > 0; j -= 2)
        {
            buf[tempcounter2++] = uint8(stoi(hexString2.substr(j-1,2), nullptr, 16) );
        }
        if(hexString2.length() % 2 != 0)
        {
            temp2 = hexString2[0];
            hexString1.erase(0);
            buf[tempcounter2++] = uint8(stoi(temp2, nullptr, 16) );
        }

        counter+=2;
    }


    *(newlogger) << "SocketActive.Send = " << SocketActive.Send(buf, counter) << '\n';
    ShowError(SocketActive, "SocketActive.Send");

}

void switch_func (string hexString, int32_t& counter, uint8 (&buf)[MAX_PACKET]){
    int tempcounter = counter;
    string temp1;
    for(int j = hexString.length()-1; j > 0; j=j-2)
        {
            buf[tempcounter++] = uint8(stoi(hexString.substr(j-1,2), nullptr, 16) );
        }
        if(hexString.length() % 2 != 0)
        {
            temp1 = hexString[0];
            hexString.erase(0);
            buf[tempcounter++] = uint8(stoi(temp1, nullptr, 16) );
        }
}

void upload_traj(CActiveSocket& SocketActive, vector<vector<int32_t>> nodes){
    //This function send trajectory to gradient amplifier
    int32_t counter = 0;
    uint8 buf[MAX_PACKET]{0} ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x06);
    buf[3] = uint8(0x00);
    counter+=4;

    string hexString = hex_converting(nodes.size());
    string hexString2 = hex_converting(nodes[nodes.size()-1][0]);

    switch_func(hexString2, counter, buf);
    counter+=4;
    switch_func(hexString, counter, buf);
    counter+=4;

    *(newlogger) << "UPLOADING TRAJECTORY";
    *(newlogger) << "SocketActive.Send = " << SocketActive.Send(buf, 12) << '\n';
    ShowError(SocketActive, "SocketActive.Send");

    int32_t nodes_cnt = nodes.size();
    int32_t segments_cnt = nodes_cnt / NODES_PER_PACKET; // segments_count

    if (nodes_cnt % NODES_PER_PACKET != 0)
        segments_cnt += 1;

    vector<int32_t> segment_status(segments_cnt); //���������� �� ���������
    segment_status.assign(segments_cnt, -2);
    int32_t left_wo_confirm{PACKETS_WO_CONFIRM};

    //bool confirm_timeout_detected = false;
    //string prev_debug_info = " ";

    vector <int32_t> uploaded_nums;
    uploaded_nums = get_unloaded_num(segment_status);
    int32_t counter_uploaded_nums = uploaded_nums.size()-1;

    while (!uploaded_nums.empty()){

        int32_t seg_num;
        seg_num = uploaded_nums.front();

        if (segment_status[seg_num] != -2){
            *(newlogger) << "Repeating upload segment" << seg_num << "with status" <<segment_status[seg_num] << '\n';
        }//if

        bool need_confirm = false;    //by default

        if (left_wo_confirm == 0)
            {
                left_wo_confirm = PACKETS_WO_CONFIRM;
                need_confirm = true;
            }
        else left_wo_confirm -= 1;    //for next iteration

        if (seg_num == counter_uploaded_nums)
            need_confirm = true;

        upload_segment(SocketActive, seg_num, need_confirm, nodes);

        *(newlogger) << "listening..." << '\n\n';
        *(newlogger) << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
        ShowError(SocketActive, "SocketActive.Receive");

        if(int32_t(buf[4]) == seg_num + 1)
        {
            uploaded_nums.erase(uploaded_nums.begin());
            *(newlogger) <<"Uploaded: " << seg_num + 1 << '\n';
        }
        else
        {
            *(newlogger) << LogPref::Flag(ERROR) << "Fatal error 404" << '\n';
        }

    }//while


}//upload_traj

void download_traject (CActiveSocket &SocketActive, int32_t points_cnt)
{
    int32_t counter=0;
    vector<int32_t> points;
    uint8 buf[MAX_PACKET]{0} ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x09);
    buf[3] = uint8(0x00);
    buf[4] = uint8(0x00);
    buf[5] = uint8(0x00);
    buf[6] = uint8(0x00);
    buf[7] = uint8(0x00);
    counter+=8;
    switch_func(hex_converting(points_cnt), counter, buf);

    *(newlogger) << "DOWNLOADING TRAJECTORY" << endl;
    *(newlogger) << "SocketActive.Send = " << SocketActive.Send(buf, 12) << '\n';
    ShowError(SocketActive, "SocketActive.Send");
    uint32_t expected_packets_cnt = points_cnt / POINTS_PER_PACKET;
    if (points_cnt % POINTS_PER_PACKET != 0)
    {
        expected_packets_cnt += 1;
    }
    vector<int32_t> downloaded_segments;
    downloaded_segments.assign(expected_packets_cnt, 0);
    points.assign(points_cnt, 0);

    int32_t for_cnt=0;
    int32_t skiped_nums;


    FILE *file;
    char filename[] = "downloaded_traj.txt";
    file = fopen(filename, "a");
    if (file == NULL)
    {
        printf("Error\n", filename);
    }

    int null_counter=0;
    while(!downloaded_segments.empty())
    {
        skiped_nums = for_cnt == 0 ? 10 : 12;
        for_cnt++;
        downloaded_segments.pop_back();
        *(newlogger) << "listening..." << '\n';
        *(newlogger) << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
        ShowError(SocketActive, "SocketActive.Receive");


        for(int32_t ii=10; ii<SocketActive.GetBytesReceived(); ii+=2)
            {
                 fprintf(file, "%d\n", int16_t(buf[ii+1] << 8 | buf[ii])) ;
            } //for
    }




}
} //namespace request
//////////////////////////////////////////////////////////////


int main(int argc, char** argv)
{
    newlogger = new SimpleLogger(out, "gra", std::string("gralogs-cmd"));
    if(argc > 1)
    {
        if(strcmp(argv[1], "--debug") == 0)
        {
            newlogger->enableConsoleOutput(true);
        }
        else {
            newlogger->enableConsoleOutput(false);
        }
    }

    string Traject_file_name;

    /*
    if(argc >= 3)
        Traject_file_name = argv[1];
    else
    {
        *(newlogger) << LogPref::Flag(ERROR) << "Wrong arguments!";
        return -1;
    }
    */


    *(newlogger) << "Sync delay: 450 msec" << endl;
    delay(450);

    string addr;
    cout << "Enter IP address: ";
    getline(cin, addr);

    SetConsoleCP(866);
    SetConsoleOutputCP(866);

    int32_t points_cnt = 0;
    CActiveSocket SocketActive( CSimpleSocket::CSocketType::SocketTypeUdp) ;
    *(newlogger) << "starting" << endl;
    // Initialize our socket object
    *(newlogger) << "SocketActive.Initialize = " << SocketActive.Initialize() << endl;
    request::ShowError(SocketActive, "SocketActive.Initialize");
    *(newlogger) << "SocketActive.Open = " << SocketActive.Open(addr.c_str(), 5002) << '\n';
    request::ShowError(SocketActive, "SocketActive.Open");

    cout << "\n" << "Command list: \n \n";
    cout << "a - get_api_version \n";
    cout << "v - get_sw_revision \n";
    cout << "s - get_gru_state \n";
    cout << "1 - turn_ps_on \n";
    cout << "0 - turn_ps_off \n";
    cout << "d <points> - download_traject \n";
    cout << "u <file_name> - upload_traj \n";
    cout << "q - socket_close and exit \n \n";

    bool exit = false;
    Traject_file_name = "";

    while(!exit) {
        string cmd;
        cout << "Enter command: ";

        delay(100);

        getline(cin, cmd, '\n');

        *(newlogger) << "Command: " << cmd << endl;

        stringstream ss(cmd);
        string arg;

        getline(ss, arg, ' ');
        getline(ss, arg, ' ');

        switch(cmd[0])
        {
            case 'a':
                request::get_API_version(SocketActive);
                break;
            case 'v':
                request::get_sw_revision(SocketActive);
                break;
            case 's':
                request::get_gru_state(SocketActive);
                break;
            case '1':
                request::turn_ps_on(SocketActive);
                break;
            case '0':
                request::turn_ps_off(SocketActive);
                break;
            case 'd':
            {
                cout << "Downloading traject..." << endl;
                int points;

                try
                {
                    points = stoi(arg);
                }
                catch(invalid_argument e)
                {
                    cout << "Exception: " << e.what() << endl;
                    break;
                }

                request::download_traject(SocketActive, points);
                break;
            }
            case 'u':
            {
                cout << "Uploading trajectory..." << endl;
                request::upload_traj(SocketActive, request::get_nodes(arg));
                break;
            }
            case 'q':
                request::socket_close(SocketActive);
                exit = true;
                break;
            default:
                cout << "Unknown command" << endl;
                break;
        }

    }

    newlogger->closeLogger();

    return 0;
}//main
