#include "networkinterface.h"

// Konstruktor
Networkinterface::Networkinterface(){
}

// Destruktor
Networkinterface::~Networkinterface (){
    close(socket_fd);
}


int Networkinterface::init(void){

    struct sockaddr_in si_client;
    struct timeval timeout_udp;
    int aton_err, bind_err, init_err = 0;
    memset((char*) &si_other, 0, sizeof(si_other));

    timeout_udp.tv_sec = 0;
    timeout_udp.tv_usec = 10;
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    si_client.sin_family = AF_INET;
    si_client.sin_addr.s_addr= htonl(INADDR_ANY);
    si_client.sin_port=htons(PORT); //source port for outgoing packets



    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);     // ohne O_NONBLOCK -> receive blockiert wenn keine Daten anliegen.
    if(socket_fd<0){
        error(socket_fd,errno,"failed create socket");
    }
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout_udp, sizeof(timeout_udp));

    aton_err=inet_aton(SERVER , &si_other.sin_addr);
    if(aton_err<0){
        error(aton_err,errno,"failed at writing ip address");
    }
    bind_err=bind(socket_fd,(struct sockaddr *)&si_client,sizeof(si_client));
    if(bind_err<0){
        error(bind_err,errno,"error at binding socket");
    }

    len = sizeof(si_other);

    if (bind_err < 0 || aton_err < 0){
        init_err = -1;
    }
    return init_err;
}



int Networkinterface::send(uint8_t bbm_dev_no, uint8_t bbm_command, uint8_t d1){
    data.clear();
    data.push_back(d1);
    builder.encode(bbm_dev_no, bbm_command, data, buffer);
    lowLevelSend();
    return send_err;
}

int Networkinterface::send(uint8_t bbm_dev_no, uint8_t bbm_command, uint8_t d1, uint8_t d2){
    data.clear();
    data.push_back(d1);
    data.push_back(d2);
    builder.encode(bbm_dev_no, bbm_command, data, buffer);
    lowLevelSend();
    return send_err;
}

int Networkinterface::send(uint8_t bbm_dev_no, uint8_t bbm_command, uint8_t d1, uint8_t d2, uint8_t d3){
    data.clear();
    data.push_back(d1);
    data.push_back(d2);
    data.push_back(d3);
    builder.encode(bbm_dev_no, bbm_command, data, buffer);
    lowLevelSend();
    return send_err;
}

int Networkinterface::send(uint8_t bbm_dev_no, uint8_t bbm_command, uint8_t d[4]){  //würg oder uint8_t* ?
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
    recv_err=recvfrom(socket_fd, (char*)rec_buffer, 10 , 0, (struct sockaddr *)&si_other, &len);
    //if(recv_err<0)
       // error(recv_err,errno,"failed at receiving");

    return recv_err;
}


int Networkinterface::lowLevelSend(){
    send_err=sendto(socket_fd, (char*)&buffer , 10 , 0 ,(struct sockaddr *)&si_other, len);
    if(send_err<0){
        error(send_err,errno,"failed at sending");
    }
    return send_err;
}
