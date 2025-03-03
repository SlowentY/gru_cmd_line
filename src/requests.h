#include "PassiveSocket.h"
#include "requests.cpp"
void ShowError(CSimpleSocket ss, string s);

void get_API_version (CActiveSocket& SocketActive);

void get_sw_revision(CActiveSocket& SocketActive);

void get_gru_state(CActiveSocket& SocketActive);

void socket_close(CActiveSocket SocketActive);

vector <int32_t> get_unloaded_num(vector<int32_t> segment_status);

void upload_segment(CActiveSocket &SocketActive, int32_t seg_num, bool need_confirm, vector<vector<int32_t>> nodes);

void upload_traj(CActiveSocket& SocketActive, vector<vector<int32_t>> nodes);

vector <int32_t> download_traject (CActiveSocket &SocketActive, int32_t points_cnt);

vector<vector<int32_t>> get_nodes(const string& Traject_file_name);

string hex_converting(int num);
