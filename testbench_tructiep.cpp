#include "systemc.h"
#include "elevator.cpp"

#define time_select 1
#define time_next_event 5

// Ham main
int sc_main(int argc, char* argv[]) {
  // Khai bao cac doi tuong
  sc_clock clk("clk", 1, SC_SEC); // Xung nhip 1 giay
  sc_signal<bool> start; 
  sc_signal<bool> call[NUM_FLOORS];
  sc_signal<bool> select[NUM_FLOORS];
  sc_signal<bool> motor_up; 
  sc_signal<bool> motor_down;
  sc_signal<bool> door_open;
  sc_signal<bool> door_close;
  sc_signal<int> floor_indicator;

  // Khoi tao module thang may
  elevator e("elevator");
  // Ket noi cac cong vao ra
  e.clk(clk);
  e.start(start);
  for (int i = 0; i < NUM_FLOORS; i++) {
    e.call[i](call[i]);
    e.select[i](select[i]);
  }
  e.motor_up(motor_up);
  e.motor_down(motor_down);
  e.door_open(door_open);
  e.door_close(door_close);
  e.floor_indicator(floor_indicator);

  // Thiet lap cac tham so mo phong
  sc_trace_file* tf = sc_create_vcd_trace_file("elevator"); // Tao file VCD de xem wave form
  sc_trace(tf, clk, "clk"); // Theo doi xung clk
  sc_trace(tf, start, "start"); // Theo doi tin hieu start
  for (int i = 0; i < NUM_FLOORS; i++) {
    char name[10];
    sprintf(name, "call(%d)", i); // Tao ten cho cac tin hieu call o moi tang
    sc_trace(tf, call[i], name); // Theo doi tin hieu call moi tang
    sprintf(name, "select(%d)", i); // Tao ten cho cac tin hieu sellect
    sc_trace(tf, select[i], name); // Theo doi cac tin hieu sellect
  }
  sc_trace(tf, motor_up, "motor_up"); // Theo doi tin hieu motor_up
  sc_trace(tf, motor_down, "motor_down"); // Theo doi tin hieu motor down
  sc_trace(tf, door_open, "door_open"); // Theo doi tin hieu door_open
  sc_trace(tf, door_close, "door_close"); // Theo doi tin hieu door_close
  sc_trace(tf, floor_indicator, "floor_indicator"); // Theo doi tin hieu floor_indicator

  // Bat dau mo phong
  start.write(true); // Dat tin hieu start la true
  for (int i = 0; i < NUM_FLOORS; i++) {
    call[i].write(false); // Dat cac tin hieu call la false
    select[i].write(false); // Dat cac tin hieu sellect sellect
  }

  //Mo phong bang cach day truc tiep cac call va sellect vào khoi dieu khien
    sc_start(time_next_event, SC_SEC); 
    select[3].write(true); 
    sc_start(time_select, SC_SEC);
    select[3].write(false);
    sc_start(time_next_event, SC_SEC); 
    call[3].write(true); 
    sc_start(time_select, SC_SEC); 
    call[3].write(false); 
    sc_start(time_next_event, SC_SEC); 
    select[1].write(true); 
    sc_start(time_select, SC_SEC); 
    select[1].write(false); 
    sc_start(time_next_event, SC_SEC); 
    call[2].write(true); 
    sc_start(time_select, SC_SEC); 
    call[2].write(false); 
    sc_start(5, SC_SEC); 
    sc_close_vcd_trace_file(tf); 
  // Kết thúc quá trình mô phỏng
  return 0;
}