/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()

#define BUF_LEN 65540

#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"

int main(int argc, char *argv[]) {

  if (argc != 2) {                  // Test for correct number of parameters
    cerr << "Usage: " << argv[0] << " <Server Port>" << endl;
    exit(1);
  }

  unsigned short servPort = atoi(argv[1]);     // First arg:  local port

  namedWindow("recv",CV_WINDOW_AUTOSIZE);
  try {
    UDPSocket sock(servPort);                
  
    char buffer[BUF_LEN];         // Buffer for echo string
    int recvMsgSize;                  // Size of received message
    string sourceAddress;             // Address of datagram source
    unsigned short sourcePort;        // Port of datagram source
    

    for (;;) {  // Run forever
      // Block until receive message from a client
      do{
        recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
      }while(recvMsgSize>sizeof(int));
      int total_pack=((int*) buffer)[0];

      cout<<"expecting length of packs:"<<total_pack<<endl;
      char* longbuf=new char[PACK_SIZE*total_pack];
      for(int i=0;i<total_pack;i++){
        recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
        if(recvMsgSize != PACK_SIZE){
          cerr << "Received unexpected size pack:" << recvMsgSize << endl;
          continue;
        }
        memcpy(&longbuf[i*PACK_SIZE], buffer, PACK_SIZE);
      }



      cout << "Received packet from " << sourceAddress << ":" 
        << sourcePort << endl;
      //cout << "Length:" << recvMsgSize << "w/ first Char:" 
      //  << buffer[0] <<endl;
  
      //for (int i=0;;i++){
      //  sprintf(buffer,"seq: %d:\n",i);
      //  sock.sendTo(buffer, strlen(buffer), sourceAddress, sourcePort);
      //  sleep(1);
        //cout<<ret<<endl;
      //}  
      Mat rawData = Mat( 1, PACK_SIZE*total_pack, CV_8UC1, longbuf );
      Mat frame = imdecode(rawData, CV_LOAD_IMAGE_COLOR);
      if(frame.size().width==0){
        cerr<< "decode failure!" <<endl;
        continue;
      }
      imshow("recv", frame);
      free(longbuf);
      waitKey(1);
    }
  } catch (SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }
  // NOT REACHED

  return 0;
}
