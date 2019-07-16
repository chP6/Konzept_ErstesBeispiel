# Konzept_ErstesBeispiel

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


