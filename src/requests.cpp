#pragma once
#include <iostream>
#include <fstream>
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
#include <simplelogger.hpp>
using namespace std;

extern std::ostream out(std::cout.rdbuf());
stringstream mystream;
extern SimpleLogger newlogger = SimpleLogger(out, "sync");
SimpleLogger errorlogger = SimpleLogger(cerr);
SimpleLogger stringlogger = SimpleLogger(mystream);

namespace request
{
constexpr size_t MAX_PACKET{4096};
const int32_t NODES_PER_PACKET{200};          // Number of points for 1 package
const uint32_t PACKETS_WO_CONFIRM{1};        // How many packages could be sent without confirmation
const double CONFIRM_TIMEOUT_SEC{0.1};
string hex_converting(int32_t num)
{
    stringlogger << hex << num;
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

    newlogger << " " << s << " : " << " = " << ss.DescribeError() << '\n';
    newlogger << " IsSocketValid() = " << ss.IsSocketValid() << '\n\n';
} //ShowError


void get_API_version (CActiveSocket& SocketActive){ //This function send API version request and shows response
    uint8 buf[MAX_PACKET] ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x0C);
    buf[3] = uint8(0x00);

    newlogger << "GET API VERSION";
    newlogger << "SocketActive.Send = " << SocketActive.Send(buf, 4) << '\n';
    ShowError(SocketActive, "SocketActive.Send");


    newlogger << "listening..." << '\n\n';
    newlogger << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

/// @return number of bytes actually received.
/// @return of zero means the connection has been shutdown on the other side.
/// @return of -1 means that an error has occurred.

    newlogger << "Bytes Received : " ;
    for(int32_t i=0; i<SocketActive.GetBytesReceived(); i++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
            newlogger << hex << buf[i];
        } //for
        newlogger << '\n\n';
}// get_API_version



void get_sw_revision(CActiveSocket& SocketActive){ //This function send revision version request and shows response
    uint8 buf[MAX_PACKET] ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x0D);
    buf[3] = uint8(0x00);

    newlogger << "GET SW REVISION";
    newlogger << "SocketActive.Send = " << SocketActive.Send(buf, 4) << '\n';
    ShowError(SocketActive, "SocketActive.Send");


    newlogger << "listening..." << '\n\n';
    newlogger << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

/// @return number of bytes actually received.
/// @return of zero means the connection has been shutdown on the other side.
/// @return of -1 means that an error has occurred.

    newlogger << "Bytes Received : " ;
    for(int32_t ii=0; ii<SocketActive.GetBytesReceived(); ii++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
            newlogger << hex << buf[ii];
        } //for
        newlogger << '\n\n';
}//get_sw_revision

void get_gru_state(CActiveSocket& SocketActive){ //This function send request of state gradient amplifier and shows response
    uint8 buf[MAX_PACKET] ;
    buf[0] = uint8(0xAA);
    buf[1] = uint8(0xAA);
    buf[2] = uint8(0x05);
    buf[3] = uint8(0x00);

    newlogger << "GET GRU STATE";
    newlogger << "SocketActive.Send = " << SocketActive.Send(buf, 4) << '\n';
    ShowError(SocketActive, "SocketActive.Send");


    newlogger << "listening..." << '\n\n';
    newlogger << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

/// @return number of bytes actually received.
/// @return of zero means the connection has been shutdown on the other side.
/// @return of -1 means that an error has occurred.

    newlogger << "Bytes Received : " ;
    for(int32_t ii=0; ii<SocketActive.GetBytesReceived(); ii++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << '\n';
            newlogger << hex << buf[ii];
        } //for
        newlogger << '\n\n';

}//get_gru_state



void socket_close(CActiveSocket SocketActive){ // This function closes socket
newlogger << "SocketActive.Close() = " << SocketActive.Close() << '\n';
    ShowError(SocketActive, "SocketActive.Close");
    newlogger << "closed" << '\n';
}

vector<vector<int32_t>> get_nodes(const string& Traject_file_name) {
    vector<vector<int32_t>> nodes;
     ifstream myfile(Traject_file_name);
    if (!myfile.is_open()) { // ��������, ������� �� ������ ����
        errorlogger << "Unable to open file\n"; // ����� �� ��������� � �������
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

        for(int j = hexString1.length()-1; j > 0; j=j- 2)
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


    newlogger << "SocketActive.Send = " << SocketActive.Send(buf, counter) << '\n';
    ShowError(SocketActive, "SocketActive.Send");

}

void switch_func (string hexString, int32_t& counter, uint8 (&buf)[MAX_PACKET]){

    switch(hexString.length())
{
    case 1:
        {
        buf[counter+1] = uint8(stoi(hexString, nullptr, 16));
        break;
        }

    case 2:
        {
        buf[counter+1] = uint8(stoi(hexString, nullptr, 16));
        break;
        }
    case 3:
        {
        string part1 = hexString.substr(0,1);
        string part2 = hexString.substr(1);
        buf[counter] = uint8(stoi(part2, nullptr, 16));
        buf[counter+1] = uint8(stoi(part1, nullptr, 16));
        break;
        }
    case 4:
        {
        string part11 = hexString.substr(0,2);
        string part22 = hexString.substr(2);
        buf[counter] = uint8(stoi(part22, nullptr, 16));
        buf[counter+1] = uint8(stoi(part11, nullptr, 16));
        break;
        }

    case 5:
        {
        string part11 = hexString.substr(0,2);
        string part22 = hexString.substr(2,2);
        string part31 = hexString.substr(4,1);
        buf[counter] = uint8(stoi(part31, nullptr, 16));
        buf[counter+1] = uint8(stoi(part22, nullptr, 16));
        buf[counter+2] = uint8(stoi(part11, nullptr, 16));
        break;
        }
    case 6:
        {
        string part11 = hexString.substr(0,2);
        string part22 = hexString.substr(2,2);
        string part31 = hexString.substr(4,2);
        buf[counter] = uint8(stoi(part31, nullptr, 16));
        buf[counter+1] = uint8(stoi(part22, nullptr, 16));
        buf[counter+2] = uint8(stoi(part11, nullptr, 16));
        break;
        }
    case 7:
        {
        string part11 = hexString.substr(0,2);
        string part22 = hexString.substr(2,2);
        string part31 = hexString.substr(4,2);
        string part32 = hexString.substr(6,1);
        buf[counter] = uint8(stoi(part32, nullptr, 16));
        buf[counter+1] = uint8(stoi(part31, nullptr, 16));
        buf[counter+2] = uint8(stoi(part22, nullptr, 16));
        buf[counter+3] = uint8(stoi(part11, nullptr, 16));
        break;
        }
    case 8:
        {
        string part11 = hexString.substr(0,2);
        string part22 = hexString.substr(2,2);
        string part31 = hexString.substr(4,2);
        string part32 = hexString.substr(6,2);
        buf[counter] = uint8(stoi(part32, nullptr, 16));
        buf[counter+1] = uint8(stoi(part31, nullptr, 16));
        buf[counter+2] = uint8(stoi(part22, nullptr, 16));
        buf[counter+3] = uint8(stoi(part11, nullptr, 16));
        break;
        }
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

    newlogger << "UPLOADING TRAJECTORY";
    newlogger << "SocketActive.Send = " << SocketActive.Send(buf, 12) << '\n';
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
        seg_num = uploaded_nums.back();
        uploaded_nums.pop_back();

        if (segment_status[seg_num] != -2){
            newlogger << "Repeating upload segment" << seg_num << "with status" <<segment_status[seg_num] << '\n\n';
        }//if

        //����������� �� ������������� ������?

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

        this_thread::sleep_for(chrono::microseconds(10));
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
    newlogger << hex << '\t' << points_cnt << '\n';
    switch_func(hex_converting(points_cnt), counter, buf);
    newlogger << "DOWNLOADING TRAJECTORY";
    newlogger << "SocketActive.Send = " << SocketActive.Send(buf, 12) << '\n\n';
    ShowError(SocketActive, "SocketActive.Send");
    uint32_t expected_packets_cnt = points_cnt / NODES_PER_PACKET;
    if (points_cnt % NODES_PER_PACKET != 0)
    {
        expected_packets_cnt += 1;
    }
    vector<int32_t> downloaded_segments;
    downloaded_segments.assign(expected_packets_cnt, 0);
    points.assign(points_cnt, 0);
    newlogger << "listening..." << '';
    newlogger << "SocketActive.Receive = " << hex <<SocketActive.Receive(MAX_PACKET, buf) << '\n';
    ShowError(SocketActive, "SocketActive.Receive");

/// @return number of bytes actually received.
/// @return of zero means the connection has been shutdown on the other side.
/// @return of -1 means that an error has occurred.

    newlogger << "Bytes Received : " ;
    for(int32_t ii=0; ii<SocketActive.GetBytesReceived(); ii++)
        {
            //cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
            newlogger << hex << buf[ii];
        } //for
        newlogger << '\n\n';

    ofstream outFile("data.txt");
    SimpleLogger filelogger = SimpleLogger(outFile);
    if(!outFile.is_open())
    {
        errorlogger << "Error open file" << '\n';
    }
    for(int32_t ii=0; ii<SocketActive.GetBytesReceived(); ii++)
        {
            filelogger << buf[ii] << '\n';
        } //for
        outFile.close();
}



}


