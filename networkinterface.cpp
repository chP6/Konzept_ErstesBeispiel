#include "networkinterface.h"

// Konstruktor
Networkinterface::Networkinterface(){

}

// Destruktor
Networkinterface::~Networkinterface (){
    close(socket_fd);
}


int Networkinterface::init(int port){
    int aton_err, bind_err;

    memset((char*) &addr_dst, 0, sizeof(addr_dst));
    addr_dst.sin_family = AF_INET;
    addr_dst.sin_port = htons(8000);

    memset((char*) &addr_loc, 0, sizeof(addr_loc));
    addr_loc.sin_family = AF_INET;
    addr_loc.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_loc.sin_port = htons(port);


    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);     // ohne O_NONBLOCK -> receive blockiert wenn keine Daten anliegen.
    if(socket_fd<0){
        //error(socket_fd,errno,"failed create socket");
        return -1;
    }

    aton_err = inet_aton(SERVER , &addr_dst.sin_addr);      // "Server" addr auf binär übersetzen
    if(aton_err<0){
        //error(aton_err,errno,"failed at writing ip address");
        return -2;
    }
    bind_err=bind(socket_fd,(struct sockaddr*)&addr_loc,sizeof(addr_loc));
    if(bind_err<0){
        //error(bind_err,errno,"error at binding socket");
        return -3;
    }

    return 0;
}

//no data, is empty
int Networkinterface::send(int bbm_dev_no, int bbm_command){
    data.clear();
    builder.encode(bbm_dev_no, bbm_command, data, buffer);
    lowLevelSend();
    return send_err;
}

int Networkinterface::send(int bbm_dev_no, int bbm_command, int d1){
    data.clear();
    data.push_back(d1);
    builder.encode(bbm_dev_no, bbm_command, data, buffer);
    lowLevelSend();
    return send_err;
}

int Networkinterface::send(int bbm_dev_no, int bbm_command, int d1, int d2){
    data.clear();
    data.push_back(d1);
    data.push_back(d2);
    builder.encode(bbm_dev_no, bbm_command, data, buffer);
    lowLevelSend();
    return send_err;
}

int Networkinterface::send(int bbm_dev_no, int bbm_command, int d1, int d2, int d3){
    data.clear();
    data.push_back(d1);
    data.push_back(d2);
    data.push_back(d3);
    builder.encode(bbm_dev_no, bbm_command, data, buffer);
    lowLevelSend();
    return send_err;
}

int Networkinterface::send(int bbm_dev_no, int bbm_command, int d[4]){  //würg oder uint8_t* ?
    data.clear();
    data.push_back(d[0]);
    data.push_back(d[1]);
    data.push_back(d[2]);
    data.push_back(d[3]);
    builder.encode(bbm_dev_no, bbm_command, data, buffer);
    lowLevelSend();
    return send_err;
}




int Networkinterface::receive(uint8_t* rec_buffer){
    recv_err=recvfrom(socket_fd, (char*)rec_buffer, sizeof(rec_buffer) , 0, (struct sockaddr *)&addr_sender, &len_sender);
    addr_ptr = inet_ntoa(addr_sender.sin_addr);    //adresse des senders übersetzen
    sender_addr = addr_ptr;
    //if(recv_err<0)
       // error(recv_err,errno,"failed at receiving");

    return recv_err;
}


int Networkinterface::lowLevelSend(){
    send_err=sendto(socket_fd, (char*)&buffer , 10 , 0 ,(struct sockaddr *)&addr_dst, sizeof(addr_dst));
    //if(send_err<0){
        //error(send_err,errno, "failed at sending");
    //}
    return send_err;
}

void Networkinterface::getSenderAddr(std::string &addr){
    addr = sender_addr;
}
