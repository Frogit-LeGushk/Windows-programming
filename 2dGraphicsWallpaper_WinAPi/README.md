### 2d graphics via winapi on wallpaper of desktop
- `If u want just run test example:` run desctop.exe file, which location at the path: *desctop/bin/Debug/desctop.exe
- `If u want sort out in realization or improve code` most simple case: to start project in IDE Code::Blocks

### result
![Image alt](https://github.com/Acool4ik/windows/tree/master/2dGraphicsWallpaper_WinAPi/desctop/bitmap.jpg)

About keyboard handlers:
- button `'I'` => increase size of grid cell 
- button `'D'` => decrease size of grid cell 
- button `'F'` => fasterer render
- button `'S'` => slower render
- button `[1 - 8]` => detalization of drawing

Introduction: This project can be used as template for creating interactive wallpaper (paint app or games directly in desctop). Code containes working example, which you can look on your computer (on condition you have windows 10 or more) by running exe file.

Description of logic: after some timeout expiration is creating BMP-file, then  throu winapi function update wallpaper (indicate path to a new updated file) and so every time. Pressing keyboard and mouse (on descktop) it processing via virtual keyboard (winapi function). For rise performanse i use double bufferization, howewer functions for drawing in this project enough slower (many reccomend use GTK+), that's why couldn't get more than 10 fps. 

Note: in the program was be static defined scale of desktop, which equal 125%. If you have other scale, i recommend at launch time set scale = 125% or change the property in code and compile programm again.
