/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: ear
 *
 * Created on January 31, 2018, 4:47 PM
 */

#include <MMU.h>
int main(int argc, char** argv) {
    mem::MMU mem(16);mem::Addr vaddress = 0x3000;
    uint8_t data[] = { 'a', 'b', 'c', '\0' };
    mem.put_bytes(vaddress, sizeof(data), data);uint8_t buffer[sizeof(data)];
    mem.get_bytes(buffer, vaddress, sizeof(buffer));
}

