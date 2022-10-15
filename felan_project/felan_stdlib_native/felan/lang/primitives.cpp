//
// Created by a404m on 9/8/22.
//

#include <felan/native_library_loader/NativeLibraryLoader.h>
#include <felan/native_interface/environment/Environment.h>
#include <felan/native_interface/felanTypes.h>
#include <iostream>

extern "C" void init(){

}

extern "C" int sum(int a,int b){
    return a+b;
}