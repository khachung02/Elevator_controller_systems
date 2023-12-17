//Khai bao thu vien can dung
#include "systemc.h"
#include <iostream>

// Khai bao hang so cho so tang
#define NUM_FLOORS 4

// Khai bao kieu du lieu liet ke cac trang thai cho thang may
enum elevator_state {IDLE, UP, DOWN, RETURN_FIRST_FLOOR};
enum door_state {CLOSE, OPEN};

// Khai bao module evelator cho khoi dieu khien thang may
SC_MODULE(elevator) {
  // Khai bao cac cong vao ra cho module
  sc_in<bool> clk; // Cong vao cho xung clock
  sc_in<bool> start; // Cong vao cho nut bat dau
  sc_in<bool> call[NUM_FLOORS]; // Cong vao cho nut goi thang may o moi tang
  sc_in<bool> select[NUM_FLOORS]; // Cong vao cho nut tron tang trong thang may
  sc_out<bool> motor_up; // Cong ra cho dong co di len
  sc_out<bool> motor_down; // Cong ra cho dong co di xuong
  sc_out<bool> door_open; // Cong ra cho mo cua
  sc_out<bool> door_close; // Cong ra cho dong cua
  sc_out<int> floor_indicator; // Cong ra chi thi tang hien tai

  // Khai bao cac bien noi bo cho module
  elevator_state current_state; // Bien luu trang thai hien tai cua thang may
  door_state current_door; // Bien luu trang thai hien tai cua cua
  int current_floor; // Bien luu tang hien tai cua thang may
  bool request[NUM_FLOORS]; // Bien luu yeu cau thang may cua moi tang
  int target_floor; // Bien luu tang dich cua thang may

  // Khai bao ham khoi tao cho module
  SC_CTOR(elevator) {
    // Khoi tao cac bien noi bo va cac gia tri ban dau
    current_state = IDLE; // Thang may o trang thai doi
    current_door = CLOSE; // Cua thang may dong
    current_floor = 0; // Thang may o tang 1
    for (int i = 0; i < NUM_FLOORS; i++) {
      request[i] = false; // Khong co yeu cau nao o moi tang
    }
    target_floor = -1; // Khong co tang dich nao
    // Dang ki ham xu ly chinh theo moi xung clock
    SC_METHOD(process);
    sensitive << clk.pos();
  }

  // Khai bao ham xu ly chinh cho module
  void process() {
    // Neu nut start duoc kich hoat thi xu ly cac yeu cau:
    if (start.read() == true) {
      //Luu yeu cau cua bien call hoac bien sellect vao bien request
      for (int i = 0; i < NUM_FLOORS; i++) {
        request[i] = call[i].read() || select[i].read();
      }

      // Xu ly theo trang thai hien tai cua thang may
      switch (current_state) {
        case IDLE: // Neu thang may dang dung yen
          // Tim tang dich gan nhat voi tang hien tai
          target_floor = find_nearest_floor();
          // Neu co tang dich thi chuyen sang cac trang thai khac
          if (target_floor != -1) {
            close_door();
            if (target_floor > current_floor) {
              current_state = UP;
            }
            else if (target_floor < current_floor) {
              current_state = DOWN;
            }
            else if (target_floor == current_floor){
              current_state = RETURN_FIRST_FLOOR;
            }
          }
          // Neu khong co tang dich thi cua thang may se luon mo
          else{
            open_door();
          }
          break;
        case UP: // Neu thang may dang di len
          // Neu da den tang dich, dung thang may lai va mo cua
          if (current_floor == target_floor) {
            motor_up.write(false); // Dung dong co di len
            open_door(); // Mo cua
            current_state = IDLE; // Chuyen sang trang thai doi
            request[current_floor] = false; // Xoa yeu cau o o tang hien tai
          }
          // Neu chua den tang dich thi tiep tuc di len
          else {
            motor_up.write(true); // Bat dong co di len
            current_floor++; // Tang tang hien tai len 1
          }
          break;
        case DOWN: // Neu thang may dang di xuong
          // Neu thang may da den tang dich thi dung thang may va mo cua
          if (current_floor == target_floor) {
            motor_down.write(false); // Dung dong co di xuong
            open_door(); // Mo cua
            current_state = IDLE; // Chuyenn sang trang thai doi
            request[current_floor] = false; // Xoa yeu cau tang hien tai
          }
          // Neu chua den tang dich thif tiep tuc di xuong
          else {
            motor_down.write(true); // Bat dong co di xuong
            current_floor--; // Giam tang hien tai di 1
          }
          break;
         case RETURN_FIRST_FLOOR: //Neu co request o tang ma thang may dang dung thi thang may se di ve tang 1
                if (current_floor != 0) { // Kiem tra thang may co o tang 1 hay khong
                    if (call[current_floor - 1].read() == true) { // Kiem tra co yeu cau o tang duoi khong
                        request[current_floor - 1] = false; // Neu co thi se huy yeu cau do
                    }
                    target_floor = 0; //Dat tang dich cua thang may ve tang 1
                    current_state = DOWN; //Dat thang may ve trang thai di xuong
                } else { // Neu thang may o tang 1 thi tra thang may ve trang thai doi
                    current_state = IDLE;
                    open_door();
                }
                break;
      }
      // Cập nhật chỉ thị tầng và còi báo
      update_indicator();
      //Hien thi ra Trang thai cua thang may, Tang dich, Tang hien tai
      cout    << "Current State: " << elevator_state_to_string(current_state)
              << ", Door state: " << door_state_to_string(current_door)
              << ", Target_Floor: " << target_floor
              << ", Current Floor: " << current_floor << endl;    
    }
  }

    // Khai bao ham tim kiem tang dich gan nhat voi tang hien tai
  int find_nearest_floor() {
    int result = -1; // Mac dich la khong co tang dich nao
    // Khai bao bien luu khoang cach ngan nhat
    int min_dist = NUM_FLOORS; // Gia tri mac dinh la tang toi da
    // Duyệt qua các tầng từ 1 đến NUM_FLOORS
    for (int i = 0; i < NUM_FLOORS; i++) {
      // Neu co yeu cau o tang (i+1)
      if (request[i] == true) {
        // Tinh khoang cach tu tang 1 den tang hien tai
        int dist = abs(current_floor - i);
        // Neu khoang cach nho hon khoang cach nho nhat hien tai
        if (dist < min_dist) {
          // Cap nhat lai ket qua cua khoang cach nho nhat
          result = i;
          min_dist = dist;
        }
      }
    }
    return result;
  }

  // Ham mo cua 
  void open_door() {
    // Neu cua dang dong
    if (current_door == CLOSE) {
      // Dat trang thai cua cua thanh OPEN
      current_door = OPEN;
      // Dat tin hieu mo cua la TRUE
      door_open.write(true);
      //Dat tin hieu dong cua la FALSE
      door_close.write(false);

    }
    // Neu cua dang mo tthi giu cua mo va ngat tin hieu mo cua
    else if (current_door == OPEN){
      current_door = OPEN;
      door_open.write(false);
    }
  }

  // Ham dong cua
  void close_door() {
    // Neu cua dang mo
    if (current_door == OPEN) {
      // Dat trang thai cua thanh CLOSE
      current_door = CLOSE;
      // Dat tin hieu dong cua thanh true
      door_close.write(true);
      door_open.write(false);

    }
    // Neu canh cua dang dong
    else if (current_door == CLOSE) {
      // Giu trang thai cua cua la CLOSE
      current_door = CLOSE;
      // Dat tin hieu mo cua thanh FALSE
      door_close.write(false);
    }
  }

  void update_indicator() {
    // Dat lai gia tri hien thi tang hien tai
    floor_indicator.write(current_floor);
  }


  // Hàm chuyển đổi enum elevator_state thành chuỗi
  const char* elevator_state_to_string(elevator_state state) {
      switch (state) {
          case IDLE:
              return "IDLE";
          case UP:
              return "UP  ";
          case DOWN:
              return "DOWN";
          case RETURN_FIRST_FLOOR:
              return "RETURN_FIRST_FLOOR";
          default:
              return "UNKNOWN_STATE";
      }
  }
  
  const char* door_state_to_string(door_state state) {
    switch (state) {
        case CLOSE:
            return "CLOSE";
        case OPEN:
            return "OPEN ";
        default:
            return "UNKNOWN_STATE";
    }
  }

};