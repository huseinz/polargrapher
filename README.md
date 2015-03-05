## Polar Function Grapher

   polargrapher - plot arbitrary polar functions

#####SYNOPSIS

        polargrapher [OPTION]... [FILE]...

#####DESCRIPTION
   Plots arbitrary polar functions from file.

#####OPTIONS

        -f      fullscreen mode

        -s n
                window size, where n is the length and width in pixels
		default size is 700
		this option is ignored is -f is used

        -r n
                writes every nth frame to a PNG image 
                n=1 will record every frame, 
                n=2 will record every other frame, 
                n=3 will record every third frame, etc.
		using -r without an argument will default to n=1

Pressing 'q' will exit the program.

#####DEFINING FUNCTIONS

Functions to be plotted are first declared using the **function** keyword followed by the function itself.   
The syntax is the muparser library's syntax, which is intuitive and easy to use. It's just like using Wolfram Alpha.  

Following the function declaration are various options that control various things such as how much the value of theta increases per frame, the maximum value of theta, drawing speed, color, and line width.

	draw_speed	int
		This controls the trace speed when drawing the function.
		A value of 0 will draw the function instantly. 
		The default value is 100.
	
	color		int int int 
		This sets the RBG color. Alpha to be added eventually.
	
	theta_max 	+float
		The maximum value of theta that the function will be evaluated to.
		Use this to draw only a portion of a function. 
		50*pi is the default.

	theta_increment	+float
		The amount theta is increased for every frame (iteration).
		This can be used to draw a function as dots instead of a solid line.
		This also affects the draw speed.
		pi/3000 is the default and produces a solid line for most functions. 
		pi/100 will usually produce dots.

	line_width 	0,1,2
		Trace width. 
		0 is one pixel wide and is the default.
		1 is two pixels wide
		2 is four pixels wide 
	
	rotation_speed	+/-float
		The amount the trace rotates each frame. This is just for eyecandy.
		0 is the default.
	
Below is an example function definition block with options.

        function             200 * sin(4 * theta / 9) 
        draw_speed           0
        color                255 0 0
        theta_max            20*pi
        theta_increment      pi/3000
        line_width           1
        rotation_speed       pi/10000
        end

#####TODO

Improve documentation
