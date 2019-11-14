# Buildroot und Raspi
1) Ziehe dir als erstes die neuste Version von Buildroot runter und entpacke es in einen Ordner
```
:~/buildroot
```
2) Kopiere das beigefügte File ```rpi_p6_defconfig``` in den Ordner configs
```
cp rpi_p6_defconfig ~/buildroot/configs
```
3) Schreibe die vorhandene Config in deine Buildrootconfig
```
:~/buildroot$ make rpi_p6_defconfig
```
4) Damit ist dein Buildroot so konfiguriert wie bei mir. Am besten kontrollierst du dies aber noch 
kurz. Wenn Qt dabei ist dann ist sicher schonmal gut. Wie du wahrscheinlich weisst änderst du die 
Config mit:
```
:~/buildroot$ make menuconfig
```
5) Wenn alles für dich stimmt kannst du mit dem Buildvorgang starten - das dauert...
```
:~/buildroot$ make
```
6) Wenn alles geklappt hat solltest du einen Ordner erhalten mit dem rootfs.tar
```
:~/buildroot/output/images
```

Mit Buildroot wars das jetzt schon jetzt muss das noch aufs Raspi.
1) Ziehe dir Raspbian-Stretch-Lite herunter und flashe dieses mit dem Etcher-Programm auf eine SD-Karte. Damit ist die SD-Karte richtig formatiert und der Bootloader ist drauf.
2) Du solltest nun folgende zwei partitionen auf deiner SD-Karte vorfinden:
```
:/media/$USER/boot
:/media/$USER/rootfs
```
3) Im Ordner boot musst du im File cmdline.txt eine Anpassung machen:
```
:~$ gedit  /media/$USER/boot/cmdline.txt
```
alt:
```
dwc_otg.lpm_enable=0 console=serial0,115200 console=tty1 root=PARTUUID=34319f92-02 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait
```
neu:
```
dwc_otg.lpm_enable=0 console=serial0,115200 console=tty1 root=PARTUUID=34319f92-02 rootfstype=ext4 elevator=deadline logo.nologo rootwait
```
4) Im File config.txt musst du die bekannten Displayeinstellungen machen. Habe mein File beigefügt. Dies hat noch Zusätzliche Änderungen wie:
- Ausschalten des Regenbogen-Sreens vor dem booten
- Boot-Delay auf 0
- alle Pullups die wir brauchen einschalten.
5) Speichere den Ordner ```/media/$USER/rootfs/lib/modules``` irgendwo lokal ab. Da ich in Buildroot keinen Kernel kompilierte fehlen die module.
6) Dein kompiliertes Rootfs auf die SD-Karte kopieren:
```
:~$ sudo rm -r /media/$USER/rootfs/*
:~$ sudo tar -xvf buildroot/output/images/rootfs.tar -C /media/$USER/rootfs
```
7) Nun sollte das Raspi booten. Passwort: "raspi". Jetzt müssen noch ein paar Anpassungen gemacht werden.
8) Fahre das Raspi nochmal runter und setzte die SD-Karte nochmals in deinem Compuer ein.
– kopiere den zuvor gespeicherten Ordner ```modules``` nach  ```/media/$USER/rootfs/lib/```
– dann kannst du wieder hochfahren.. somit sollten alle Treiber richtig laden

Damit das Ethernet-Interface beim booten eingeschaltet wird musst du folgendes in die Datei ```/etc/network/interfaces``` schreiben:
```
auto lo 
iface lo inet loopback
auto eth0
iface eth0 inet static
wait-delay 2
address -deine IP-
netmask -deine Subnet-
gateway -dein Gateway-
```
Das wait-delay ist wichtig, da das Ethernet-IF vom Raspi eigentlich ein USB-Adapter ist und die initialisierung etwas länger dauert...

# QT-Creator
Ähnlich wie bei dem nachinstallierten Qt muss dem Qt-Creaor ein neues Kit hinzugefügt werden.
1) Neues Gerät erstellen:
Einstellungen→ Geräte→ neues generisches Linuxgerät
2) Kompiler bekanntgeben:
Einstellungen→ Kits→ Compiler→Hinzufügen
- GCC: ```buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabi-gcc```
- C++: ```buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabi-g++```
3) qmake Version angeben:
Einstellungen→Kits→Qt Versionen→ Hinzufügen
- ```buildroot/output/host/usr/bin/qmake```
4) Kit mit den entsprechenden Komponenten erstellen.

Kompilieren und debuggen sollten nun funktionieren

## Fonts:
Ich hatte das Problem das Qt die Fonts nicht fand auf dem Raspi. Die Fonts sind auf dem Raspi unter ```/usr/share/fonts``` installiert. Qt sucht unter ```/usr/lib/fonts```.
Ich habe unter ```/usr/lib/``` einen Ordner ```fonts``` erstellt und nur die DejaVu fonts dort hinein kopiert (ohne Unterordner DejaVu). Hat funktioniert :)

# Rotary-Encoder und Tastenfeld als Linux Input-Device
Basierend auf dem [Raspberry Pi RCP Controller Panel](http://git.reufer.dyndns.org/bbm/dollycontroller/tree/master/hardware/controllerpanel)

Das device tree source file (2x6matrix.dts) beschreibt das GPIO-Matrix-Keypad-Device:

```
/dts-v1/;
/plugin/;
/ {
       compatible = "brcm,bcm2835", "brcm,bcm2708", "brcm,bcm2709";

       fragment@0 {
          target-path = "/";
          __overlay__ {
             keypad: led-buttons {
                compatible = "gpio-matrix-keypad";
                debounce-delay-ms = <10>;
                col-scan-delay-us = <10>;
                linux,no-autorepeat;
		drive-inactive-cols;

                row-gpios = <&gpio 20 0
                             &gpio 21 0>;
                col-gpios = <&gpio  5 0
                             &gpio  6 0
                             &gpio 12 0
                             &gpio 13 0
                             &gpio 16 0
                             &gpio 19 0>;

                linux,keymap = <0x00000002 0x00010003 0x00020004 0x00030005 0x00040006 0x00050007   // 1..6
                                0x0100003b 0x0101003c 0x0102003d 0x0103003e 0x0104003f 0x01050040>; // F1..F6
           };
        };
     };
  };
```
Mit dem device tree compiler (dtc) das source file in ein 'overlay-blob' (2x6matrix.dtbo) übersetzen und auf die Boot-Partition kopieren:
```
$ dtc -W no-unit_address_vs_reg -I dts -O dtb -o 2x6matrix.dtbo 2x6matrix.dts
$ sudo cp 2x6matrix.dtbo /boot/overlays/
```
config.txt anpassen:
```
dtoverlay=gpio-key,gpio=24,keycode=28,label="rotary-knob"
dtoverlay=rotary-encoder,pin_a=23,pin_b=22,relative_axis=1,label="rotary"
dtoverlay=2x6matrix
```
# Versuch einer Hotplug implementation
Neue udev Rule die alle neuen inputdevices meldet.
```
ACTION=="add",SUBSYSTEM=="input",RUN=="/opt/ocp_add 'add'  %E{DEVLINKS}"
ACTION=="remove", SUBSYSTEM=="input",RUN=="/opt/ocp_add 'rm'  %E{DEVLINKS}"
```
Das Programm "ocp_add" schreibt nun die von udev übergebenen Symlinks auf einen unix domain socket.
Im Controllerprogramm werden diese informationen verarbeitet

## ocp_add
```
int main(int argc, char *argv[])
{
    char name[] = "dummy";
    char *deviceName=NULL;
    deviceName = &name[0];

    if(argc > 2){
         deviceName = argv[2];
    }

    int client_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (client_socket < 0) {
            printf("SOCKET ERROR = %s\n", strerror(errno));
            exit(1);
        }
    struct sockaddr_un remote;
    memset(&remote, 0, sizeof(struct sockaddr_un));
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path,"/tmp/hotplug");
    char buf[256];
    memset(buf,0,sizeof (buf)/sizeof (buf[0]));

    sprintf(buf,"%s:%s", argv[1],deviceName);
    if (sendto(client_socket, buf, strlen(buf), 0, (struct sockaddr *) &remote, sizeof (struct sockaddr_un) ) < 0) {
        printf("SENDTO ERROR %s : %s\n",remote.sun_path, strerror(errno));
    }
    close(client_socket);
    return  1;
}

```

# gpio Keyboard legacy
Da bei dem alten Keyboard alle gpios direkt verdrahtet sind gibt es ein zusätzliches overlay:
```
/dts-v1/;
    /plugin/;
    / {
       compatible = "brcm,bcm2835", "brcm,bcm2708", "brcm,bcm2709";
       
       fragment@0 {
          target-path = "/";
          __overlay__ {
             keypad: led-buttons-extra {
                compatible = "gpio-keys";
                #address-cells = <1>;
                #size-cells = <0>;
                no-autorepeat;
                button@27 {
                   label = "Button 1 LR";
                   linux,code = <2>; //KEY_1
                   gpios = <&gpio 27 1>;
                };
                button@22 {
                   label = "Button 2 LR";
                   linux,code = <3>; //KEY_2
                   gpios = <&gpio 22 1>;
                };
                button@5 {
                   label = "Button 3 LR";
                   linux,code = <4>; //KEY_3
                   gpios = <&gpio 5 1>;
                };
                button@6 {
                   label = "Button 4 LR";
                   linux,code = <5>; //KEY_4
                   gpios = <&gpio 6 1>;
                };
                button@13 {
                   label = "Button 5 LR";
                   linux,code = <6>; //KEY_5
                   gpios = <&gpio 13 1>;
                };
                button@19 {
                   label = "Button 6 LR";
                   linux,code = <7>; //KEY_6
                   gpios = <&gpio 19 1>;
                };
                button@26 {
                   label = "Button 1 UR";
                   linux,code = <59>; //KEY_F1
                   gpios = <&gpio 26 1>;
                };
                button@18 {
                   label = "Button 2 UR";
                   linux,code = <60>; //KEY_F2
                   gpios = <&gpio 18 1>;
                };
                button@23 {
                   label = "Button 3 UR";
                   linux,code = <61>; //KEY_F3
                   gpios = <&gpio 23 1>;
                };
                button@24 {
                   label = "Button 4 UR";
                   linux,code = <62>; //KEY_F4
                   gpios = <&gpio 24 1>;
                };
                button@25 {
                   label = "Button 5 UR";
                   linux,code = <63>; //KEY_F5
                   gpios = <&gpio 25 1>;
                };
                button@12 {
                   label = "Button 6 UR";
                   linux,code = <64>; //KEY_F6
                   gpios = <&gpio 12 1>;
                };
             };
          };
       };
    };
```
# I2C Rotary Encoder
Der alte Rotary-Encoder hat eine Senseleitung, diese wird mittels overlay auch als Inputdevice eingelesen.
```
/dts-v1/;
    /plugin/;
    / {
       compatible = "brcm,bcm2835", "brcm,bcm2708", "brcm,bcm2709";
       
       fragment@0 {
          target-path = "/";
          __overlay__ {
             keypad: rotary-sense {
                compatible = "gpio-keys";
                #address-cells = <1>;
                #size-cells = <0>;
                                no-autorepeat;
                
                button@4 {
                   label = "Rotary-Sense-1";
                   linux,code = <67>;
                   interrupts-extended = <&gpio 4 2>;
                };
                button@17 {
                   label = "Rotary-Sense-2";
                   linux,code = <68>;
                   interrupts-extended = <&gpio 17 2>;
                };

             };
          };
       };
    };

```

