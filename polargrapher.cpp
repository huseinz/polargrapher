/*
    Polar Equation Grapher

    plots polar equations, animated

    June 18 2014
    Zubir Husein
        last updated Mar 5 2014
*/

#include <SFML/Graphics.hpp>
#include "muParser.h"
#include <cmath>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>

//length and width of window
int size = 700;
sf::Vector2i windowsize;

//recording frameskip 
int record_frameskip = -1;

//directory to write images to, must exist
std::string record_folder = "frames";

//fullscreen flag
bool fullscreen = false;

//definition of pi
const double pi = 3.14159265359;

class Func {

	// muparser object for parsing mathemathical function
	mu::Parser p;
	//initialize graphics stuff
	sf::Image graph;
	sf::Texture graphTx;
	sf::Sprite graphSpr;

	// draw speed
	int speed;
	int line_width;
	// theta
	double t;
	// radius
	double r;
	double tmax;
	double tinc;
	double rotation;
	sf::Color color;

public:

	//constructor
	Func(std::string tag,
	     int speed_in,
	     int line_width_in,
	     double tmax_in,
	     double tinc_in,
	     double rotation_in,
	     sf::Color color_in )

		: speed( speed_in ),
		  line_width(line_width_in),
		  t (0),
		  tmax ( tmax_in ),
		  tinc ( tinc_in ),
		  rotation( rotation_in ),
		  color ( color_in )
	{
		p.DefineVar("theta", &t);
		p.SetExpr(tag);
	}

	//initialize muparser and graphics 
	void init() {
		p.DefineVar("theta", &t);
		p.DefineConst("pi", pi);
		graph.create(size,size,sf::Color::Transparent);
		graphTx.loadFromImage(graph);
		graphSpr.setTexture(graphTx);

		//center on screen
		graphSpr.setOrigin(size/2,size/2);
		graphSpr.setPosition(windowsize.x/2, windowsize.y/2);
	}
	
	//iterate function draw
	sf::Sprite& update() {
		graphTx.update(graph);
		graphSpr.rotate(rotation * 180. / pi);
		return graphSpr;
	}
	//plots point to pixel(s)
	void plot() {

		//convert polar to cartesian
		double x = r * cos(t);
		double y = r * sin(t);

		if(x + 1 + size/2 > size || x - 1 + size/2 < 0 || y + 1 + size/2 > size || y - 1 + size/2 < 0)
			return;

		graph.setPixel((x + size/2), (y + size/2), color);

		if(line_width == 1 || line_width == 2) {
			graph.setPixel((x + size/2), (y + 1 + size/2), color);
			graph.setPixel((x + size/2), (y - 1 + size/2), color);
			graph.setPixel((x + 1 + size/2), (y + size/2), color);
			graph.setPixel((x - 1 + size/2), (y + size/2), color);
		}

		if(line_width == 2) {
			graph.setPixel((x + 1 + size/2), (y + 1 + size/2), color);
			graph.setPixel((x + 1 + size/2), (y - 1 + size/2), color);
			graph.setPixel((x - 1 + size/2), (y - 1 + size/2), color);
			graph.setPixel((x - 1 + size/2), (y + 1 + size/2), color);
		}
	}

	//plots draw_speed points per frame
	void animation() {
		// p.DefineVar("theta", &t);
		for(int i = 0; (i < speed || speed == 0) && t <= tmax ; i++) {
			t += tinc;
			r = p.Eval();
			plot();
		}
	}
};

//writes frames to file 
//directory must already exist
void record(sf::RenderTexture& in, int frameskip, std::string directory) {

	static int frame = 0;

	if(frameskip > 0 && frame % frameskip == 0) {
		in.display();
		sf::Texture outTx = in.getTexture();
		sf::Image outImg = outTx.copyToImage();
		std::stringstream ugh;
		ugh << directory << "/" << frame << ".png";
		outImg.saveToFile(ugh.str());
	}
	frame++;
}

void parse_file(char* filename, std::vector<Func>& funcs) {

		std::ifstream f(filename);

		std::string line;

		//default function attributes
		sf::Color color = sf::Color::Cyan;
		std::string func_literal = "100*cos(theta)";
		int speed = 100;
		int line_width = 0;
		double tmax = 50 * pi;
		double tinc = pi/3000;
		double rotation = 0;

		if(f.is_open()) {

			while(std::getline(f, line)) {

				//ignore comments
				if(line[0] == '#')
					continue;
				//write function to vector
				else if(line == "end") {
					funcs.push_back(Func(func_literal, speed, line_width, tmax, tinc, rotation, color));
					//reset default values
					func_literal = "100*cos(theta)";
					speed = 100;
					line_width = 0;
					color = sf::Color::Cyan;
					tmax = 50 * pi;
					tinc = pi/3000;
					rotation = 0;
				}
				//check for fullscreen
				else if(line == "-f") {
					fullscreen = true;
				}
				//parse other statements
				else {
					//grab keyword
					std::string::size_type pos;
					pos = line.find(' ', 0);
					std::string tag = line.substr(0, pos);

					//get function literal
					if(tag == "function")
						func_literal = line.substr(pos+1);
					//draw speed
					else if(tag == "draw_speed") {
						std::stringstream s;
						s << line.substr(pos+1);
						s >> speed;
					}
					//line color
					else if(tag == "color") {
						std::stringstream s;
						s << line.substr(pos+1);
						int r = 0, g = 0, b = 0;
						s >> r >> g >> b;
						color = sf::Color(r, g, b);
					}
					//window/graph size
					else if(tag == "-s") {
						std::stringstream s;
						s << line.substr(pos+1);
						s >> size;
					}
					//theta max
					else if(tag == "theta_max") {
						mu::Parser p;
						p.DefineConst("pi", pi);
						p.SetExpr(line.substr(pos+1));
						tmax = p.Eval();
					}
					//theta increment
					else if(tag == "theta_increment") {
						mu::Parser p;
						p.DefineConst("pi", pi);
						p.SetExpr(line.substr(pos+1));
						tinc = p.Eval();
					}
					//line width
					else if(tag == "line_width") {
						std::stringstream s;
						s << line.substr(pos+1);
						s >> line_width;
					}
					else if(tag == "rotation_speed") {
						mu::Parser p;
						p.DefineConst("pi", pi);
						p.SetExpr(line.substr(pos+1));
						rotation = p.Eval();
					}
					else if(tag == "-r") {
						std::stringstream s;
						s << line.substr(pos+1);
						s >> record_frameskip >> record_folder;
					}
				}
			}

			f.close();
		}
		else
			std::cerr << filename << " is not a valid filename.\n";
}

void parse_options(int argc, char* argv[], std::vector<Func>& funcs){
	
	int c;
	std::string frameskip_opt = "-1";
	std::string size_opt = "-1";
	std::string directory_opt = "";
//	extern char *optarg;
//	extern int optind, optopt;
	
	//using getopts 
	while((c = getopt(argc, argv, "fs:r:d:")) != -1){

		int f = -1, s = -1;

		switch(c){
			
			case 'f':{
				fullscreen = true;
				break;}
			case 'r':
				frameskip_opt.assign( optarg );
				std::istringstream (frameskip_opt) >> f;
				if(f <= -1){
					std::cerr << "invalid argument \'" << frameskip_opt << "\' to option -r\n";
					optind--;
				}
				record_frameskip = f > 0 ? f : 0;
				break;
			case 's':
				size_opt.assign( optarg );
				std::istringstream (size_opt) >> s;
				if(s <= 0){
					std::cerr << "invalid argument \'" << size_opt << "\' to option -s\n"; 
					optind--;
				}
				size = s > 0  ? s : size;
				break;
			case 'd':
				directory_opt.assign(optarg);
				record_folder = directory_opt;
				break;
			case ':':
				switch(optopt){
					case 's':
						std::cout << "option -s requires argument, using default size 700\n";
						break;
					case 'r':
						std::cerr << "using default frameskip of 0 for option -r\n";
						break;
					case 'd':
						std::cerr << "option -d requires argument, disabling recording.\n";
						record_frameskip = -1;
						break;
				}
				break;
			case '?':
				std::cerr <<  "Unrecognized option: '-" << optopt << "\n";
				break;
		}
		
		
	}
	for(int i = optind; i < argc; i++)
		parse_file(argv[i], funcs);
}

int main(int argc, char* argv[]) {

	//vector to store functions
	std::vector<Func> funcs;

	//parse input file
	parse_options(argc, argv, funcs);

	//declare renderwindow
	sf::RenderWindow window;
	if(fullscreen == false)
		window.create(sf::VideoMode(size, size, 32), "Polar Grapher", sf::Style::Default);
	else
		window.create(sf::VideoMode::getDesktopMode(), "Polar Grapher", sf::Style::Fullscreen);
	window.setFramerateLimit(60);
	windowsize.x = window.getSize().x;
	windowsize.y = window.getSize().y;

	//initialize function graphics
	for(unsigned i = 0; i < funcs.size(); i++)
		funcs[i].init();

	//initialize rendertexture for output image
	sf::RenderTexture out;
	out.create(size,size);

	//main loop
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.key.code == sf::Keyboard::Q)
				window.close();
		}

		window.clear(sf::Color::Black);
		out.clear(sf::Color::Black);
		for(unsigned i = 0; i < funcs.size(); i++) {
			funcs[i].animation();
			sf::Sprite spr = funcs[i].update();
			window.draw(spr);
			spr.setPosition(size/2, size/2);
			out.draw(spr);
		}
		window.display();

            record(out, record_frameskip, record_folder);
	}

	//write to image file
	out.display();
	sf::Texture outTx = out.getTexture();
	sf::Image outImg = outTx.copyToImage();
	outImg.saveToFile("out.png");

	return 0;
}
