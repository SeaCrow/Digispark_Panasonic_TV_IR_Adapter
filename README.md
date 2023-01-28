# Digispark_Panasonic_TV_IR_Adapter

Using Digispark (ATTiny85) with Panasonic TV remote as USB Keyboard

## Hardware
* Digispark (ATTiny85)
* VS1839 / HX1838 - 37,9KHz IR Reciver

## Software
* Project code for Arduino IDE

## Description
Using Panasonic TV as PC monitor for watching Movies/Netflix/Youtube is nice. But what's not nice is needing to walk to PC from the couch to pause the video, and then rewind cause you walked to pause and missed the action. Luckily the TV remote has some extra buttons that could be used to remotely control the PC (At least mine has).

![Panasonic TV Remote](https://github.com/SeaCrow/Digispark_Panasonic_TV_IR_Adapter/raw/main/Img/PanasonicTV_Remote.png)

Said remote.


![Digispar with IR](https://github.com/SeaCrow/Digispark_Panasonic_TV_IR_Adapter/raw/main/Img/Digispark_IR.png)

Digispark with IR Reciver


![Button mapping](https://github.com/SeaCrow/Digispark_Panasonic_TV_IR_Adapter/raw/main/Img/Remote_button_names.png)

Buttons used for remote keyboard.


### Preparing Arduino IDE for Digispark
* Add DigiStump to Board Manager: `http://digistump.com/package_digistump_index.json` and install them.
* Download and install DigiStump Drivers
* Upload the code.
[Full Explanation](https://startingelectronics.org/tutorials/arduino/digispark/digispark-windows-setup/) 

### Why?
Because I could. Also any information I managed to find about Panasonic TV remote IR signal encoding was not working for this remote. Ended conecting the reciver to oscilloscope and reverse engineering it on my own.


#### Buy me a beer
If you find the code useful and wish to buy me a beer, You can here: [Ko-Fi](https://ko-fi.com/qaazar#paymentModal)
