# Hugo-ESP8266-Plus

Hugo is a 4-button ESP8266 Wi-Fi Remote, Arduino compatible and simple to use.
This ```plus``` custom firmware aims to create an alternative firmware that simply sends out a broadcast UDP packet that requires very little setup from the device it's self.

![alt text](https://raw.githubusercontent.com/mcer12/Hugo-ESP8266/master/Images/hugo_numbered.png)

## Configuration buttons
To enter configuration mode just press and hold the on and off (1 and 4) buttons till the light comes on. When this happens simply connect to the wifi network and navigate to ```10.10.10.1``` on a web browser.

## Error messages
Sometimes problems happen.. When this occurs the board will flash x amount of times fast, wait for a second and then do it again. Below is what the amount of fast flashes means.
* ```Constant slow flashing``` - The device is in configuration mode. Connect to it's wifi network and goto ```10.10.10.1``` to configure it
* ```3 flashes``` - A wifi error occured
* ```8 flashes``` - The static ip address is invalid. The device will need to be configured correctly
* ```5 flashes``` - A file system error has occured.


## Useful links
3D printable case available here:  
https://www.thingiverse.com/thing:3641618  

Want a ready made Hugo? Get it here:  
https://www.tindie.com/products/mcer12/hugo-esp8266-4-button-wifi-remote/  

Want to make your own? Head over to Schematic directory for schematic and a list of compontents.

If you use diyHue emulator, just head to Firmwares directory for diyHueTapSwitch sketch!  
diyHue github: https://github.com/diyhue/diyHue  

# Wiki
* [Battery management](https://github.com/mcer12/Hugo-ESP8266/wiki/Battery-management)
* [Flashing & FW update](https://github.com/mcer12/Hugo-ESP8266/wiki/Flashing-&-FW-update)
* [Sketch: DiyHue](https://github.com/mcer12/Hugo-ESP8266/wiki/Sketch:-DiyHue)
* [Sketch: HTTP (Basic URL trigger)](https://github.com/mcer12/Hugo-ESP8266/wiki/Sketch:-HTTP-(Basic-URL-trigger))
* [Sketch: MQTT](https://github.com/mcer12/Hugo-ESP8266/wiki/Sketch:-MQTT)
* [Tips & Making custom sketch](https://github.com/mcer12/Hugo-ESP8266/wiki/Tips-&-Making-custom-sketch)
* [HW Modifications](https://github.com/mcer12/Hugo-ESP8266/wiki/HW-Modifications)
